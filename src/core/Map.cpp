#include <fstream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <core/Map.hpp>
#include <core/ImGuiHelper.hpp>
#include <sstream>
#include <core/RTree.cpp>
#include <util/PRNG.hpp>
#include <file/PlainText.hpp>
VISITABLE_STRUCT(glm::vec2, x, y);
#include <file/JSON.hpp>
#ifndef NDEBUG
#include <iostream>
#endif
#include <gl.h>
TileMap& ObjMap::getTM(const std::string& id){
	return this->internal_tms[id];
}
template<>
JSONParser::operator TMType() const{
	std::string data{internal.GetString()};
	if (data == "normal"){
		return TMType::Normal;
	} else if (data == "effect"){
		return TMType::Effect;
	} else {
		return TMType::Normal;
	}
}
template<>
JSONParser::operator glm::mat2() const{
	glm::mat2 data;
	data[0][0] = internal[0].GetFloat();
	data[0][1] = internal[1].GetFloat();
	data[1][0] = internal[2].GetFloat();
	data[1][1] = internal[3].GetFloat();
	return data;
}
ObjMap::ObjMap(const std::string& filename, TextureAtlas& atlas) : m_atlas(atlas){
	this->rng = SeedRNG();
	this->loadFromFile(filename);
}
void ObjMap::loadTileMap(TileMap& tomodify, JSONParser tilemapNode){
	tomodify.load(tilemapNode, m_atlas);
}
void ObjMap::loadFromFile(const std::string& filename){
	ledges.clear();
	sprs.clear();

	std::string jsondata = readWholeFile(filename);
	JSONReader document(jsondata.c_str());
	std::vector<Surface> tempSurfaces = document["surfaces"];
	ledges = static_cast<decltype(ledges)>(document["ledges"]);
	rapidjson::Value& spritesNode = document["sprites"];
	rapidjson::Value& tilemapsNode = document["tilemaps"];

	this->surfaces.load(tempSurfaces);

	for (auto& spriteNode : spritesNode.GetArray()){
		JSONParser posnode(spriteNode);
		JSONParser tnode(spriteNode["t"]);
		glm::mat2 sprtransform;
		glm::vec2 sprposition;
		sprposition = posnode;
		sprtransform = tnode;
		std::string fname = spriteNode["name"].GetString();
		this->addBGTexture(sprposition,sprtransform,fname);
	}
	for (auto& tilemapNode : tilemapsNode.GetObject()){
		std::string tmname = tilemapNode.name.GetString();
		loadTileMap(internal_tms[tmname], tilemapNode.value);
	}
	tm_changed = true;
}
void ObjMap::addBGTexture(const glm::vec2& sprPosition, const glm::mat2& sprTransform, const std::string& fname){
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
template<>
void JSONWriter::store<MapSprite>(const MapSprite& i){
	writer.StartObject();
	writer.Key("x");
	writer.Int(i.iPosition.x);
	writer.Key("y");
	writer.Int(i.iPosition.y);
	writer.Key("t");
	writer.StartArray();
	auto mat = i.spr.getMat2();
	writer.Double(mat[0][0]);
	writer.Double(mat[0][1]);
	writer.Double(mat[1][0]);
	writer.Double(mat[1][1]);
	writer.EndArray();
	writer.Key("name");
	writer.String(i.filename.c_str());
	writer.EndObject();
}
template<>
void JSONWriter::store<TMType>(const TMType& i){
	if (i == TMType::Normal){
		writer.String("normal");
	} else if (i == TMType::Effect){
		writer.String("effect");
	}
}
void ObjMap::WriteToFile(const std::string& filename){
	std::ofstream ofs(filename);
	JSONWriter writer;
	writer.StartObject();
	writer.Key("surfaces");
	writer.store(surfaces.get_elements());
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
