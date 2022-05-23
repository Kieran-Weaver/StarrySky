#include <file/JSON.hpp>
#include <file/PlainText.hpp>
#include <game/ObjMap.hpp>
#include <GL/SpriteBatch.hpp>
#include <util/PRNG.hpp>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <fstream>
#include <sstream>
#ifndef NDEBUG
#include <iostream>
#endif
VISITABLE_STRUCT(glm::vec2, x, y);

TileMap& ObjMap::getTM(const std::string& id) {
	return this->internal_tms[id];
}

ObjMap::ObjMap(const std::string_view filename, TextureAtlas& atlas) : m_atlas(atlas) {
	this->rng = SeedRNG();
	this->loadFromFile(filename);
}

void ObjMap::loadTileMap(TileMap& tomodify, JSONParser tilemapNode) {
	tomodify.load(tilemapNode, m_atlas);
}

void ObjMap::loadFromFile(const std::string_view filename) {
	ledges.clear();
	sprs.clear();

	std::string jsondata = readWholeFile(filename);
	JSONReader document(jsondata.c_str());
	std::vector<Surface> tempSurfaces = document["surfaces"];
	ledges = static_cast<decltype(ledges)>(document["ledges"]);
	JSONParser spritesNode = document["sprites"];
	JSONParser tilemapsNode = document["tilemaps"];

	this->m_cmap.load(tempSurfaces);

	for (int i = 0; i < spritesNode.size(); i++){
		const auto& spriteNode = spritesNode[i];
		JSONParser posnode(spriteNode);
		JSONParser tnode(spriteNode["t"]);
		glm::mat2 sprtransform;
		glm::vec2 sprposition;
		sprposition = posnode;
		sprtransform = tnode;
		std::string fname = static_cast<std::string>(spriteNode["name"]);
		this->addBGTexture(sprposition,sprtransform,fname);
	}
	
	std::vector<std::string_view> tmKeys = tilemapsNode.getKeys();
	for (auto& tmKey : tmKeys){
		loadTileMap(internal_tms[std::string(tmKey)], tilemapsNode[tmKey]);
	}
	tm_changed = true;
}

void ObjMap::addBGTexture(const glm::vec2& sprPosition, const glm::mat2& sprTransform, const std::string_view fname) {
	uint32_t i;
	do{
		i = rng();
	} while (sprs.count(i) != 0);
	std::string filename(fname);
	auto end_pos = std::remove(filename.begin(),filename.end(),' ');
	filename.erase(end_pos,filename.end());
	sprs[i] = {sprPosition,filename,{}};
	Texture temp = m_atlas.findSubTexture(filename);
	sprs[i].spr.setTexture(temp);
	sprs[i].spr.setPosition(sprPosition);
	sprs[i].spr.transform(sprTransform);
}

void ObjMap::SetPosition(float x, float y) {
	this->position.x = x;
	this->position.y = y;
	for (auto& i : sprs){
		i.second.spr.setPosition(i.second.iPosition.x + x, i.second.iPosition.y + y);
	}
	this->internal_tms["default"].Attrs[2] = x;
	this->internal_tms["default"].Attrs[3] = y;
	tm_changed = true;
}

const CMap& ObjMap::getCollision(void) {
	return this->m_cmap;
}

template<>
void JSONWriter::store<MapSprite>(const MapSprite& i) {
	this->StartObject();
	this->store("x");
	this->store(i.iPosition.x);
	this->store("y");
	this->store(i.iPosition.y);
	this->store("t");
	this->StartArray();
	auto mat = i.spr.getMat2();
	this->store(mat[0][0]);
	this->store(mat[0][1]);
	this->store(mat[1][0]);
	this->store(mat[1][1]);
	this->EndArray();
	this->Key("name");
	this->store(i.filename.c_str());
	this->EndObject();
}

template<>
void JSONWriter::store<TMType>(const TMType& i) {
	if (i == TMType::Normal){
		this->store("normal");
	} else if (i == TMType::Effect){
		this->store("effect");
	}
}

void ObjMap::WriteToFile(const std::string_view filename) {
	std::ofstream ofs{std::string(filename)};
	JSONWriter writer;
	writer.StartObject();
	writer.Key("surfaces");
	writer.StartArray();
	for (const auto& val : m_cmap.getSurfaces()) {
		writer.store(val.second);
	}
	writer.EndArray();
	writer.Key("sprites");
	writer.store(sprs);
	writer.Key("ledges");
	writer.store(ledges);
	writer.Key("tilemaps");
	writer.store(internal_tms);
	writer.EndObject();
	ofs << writer.GetString() << std::endl;
	ofs.close();
}

void ObjMap::Draw(SpriteBatch& frame) {
	for (auto& i : sprs){
		frame.Draw(i.second.spr);
	}
	for (auto& tmap : internal_tms){
		frame.Draw(tmap.second);
	}
}
