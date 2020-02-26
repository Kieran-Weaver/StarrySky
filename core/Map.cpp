#include <fstream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include "PRNG.hpp"
#include "Map.hpp"
#include "ImGuiHelper.hpp"
#include <sstream>
#include <minilog/minilog.h>
ObjMap::ObjMap(const std::string& filename, TextureAtlas& atlas) : m_atlas(atlas){
	this->internal_tms.set_empty_key("");
	this->surfaces.set_empty_key(-1);
	this->sprs.set_empty_key(-1);
	this->rng = SeedRNG();
	this->loadFromFile(filename);
}
std::string ObjMap::loadTileMap(TileMap& tomodify, const rapidjson::Value& tilemapNode){
	const rapidjson::Value& ATNode = tilemapNode["AffineT"];
	for (int i = 0; i < 4; i++){
		tomodify.affineT[i] = ATNode[i].GetFloat();
	}
	const rapidjson::Value& tilesNode = tilemapNode["tiles"];
	rapidjson::SizeType numTiles = tilesNode.Size() & 0xff;
	tomodify.numTiles = numTiles;
	for (int i = 0; i < numTiles; i++){
		const Texture tempTex = this->m_atlas.findSubTexture(tilesNode[i].GetString());
		tomodify.tiles[i][0] = tempTex.m_rect.left / 65536.f;
		tomodify.tiles[i][1] = tempTex.m_rect.top / 65536.f;
		tomodify.tiles[i][2] = tempTex.m_rect.width / 65536.f;
		tomodify.tiles[i][3] = tempTex.m_rect.height / 65536.f;
		tomodify.filenames.emplace_back(tilesNode[i].GetString());
	}
	const rapidjson::Value& sizeNode = tilemapNode["tileSize"];
	for (int i = 0; i < 2; i++){
		tomodify.packedtileSize[i] = sizeNode[i].GetFloat();
	}
	const rapidjson::Value& posNode = tilemapNode["position"];
	for (int i = 0; i < 2; i++){
		tomodify.packedtileSize[2 + i] = posNode[i].GetFloat();
	}
	const rapidjson::Value& drawnNode = tilemapNode["drawntiles"];
	for (auto& tileNode : drawnNode.GetArray()){
		int px = tileNode["x"].GetInt();
		int py = tileNode["y"].GetInt();
		int index = tileNode["index"].GetInt();
		tomodify.drawn.emplace_back(makeTile(px, py, index));
	}
	std::string temptype = tilemapNode["type"].GetString();
	if (temptype == "normal"){
		tomodify.type = TMType::Normal;
	} else if (temptype == "effect"){
		tomodify.type = TMType::Effect;
	}
	return tilemapNode["name"].GetString();
}
void ObjMap::loadFromFile(const std::string& filename){
	ledges.clear();
	surfaces.clear();
	sprs.clear();

	std::string jsondata = readWholeFile(filename);
	rapidjson::Document document;
	rapidjson::ParseResult result = document.Parse(jsondata.c_str());
	if (!result) {
		MINILOG(logERROR) << "ERROR: Invalid JSON file " << filename;
		std::exit(1);
	}
	const rapidjson::Value& surfacesNode = document["surfaces"];
	const rapidjson::Value& spritesNode = document["sprites"];
	const rapidjson::Value& ledgesNode = document["ledges"];
	const rapidjson::Value& tilemapsNode = document["tilemaps"];
	for (auto& surfaceNode : surfacesNode.GetArray()){
		Surface s;
		s.hitbox = {surfaceNode["x"].GetFloat(),surfaceNode["y"].GetFloat(),surfaceNode["w"].GetFloat(),surfaceNode["h"].GetFloat()};
		s.flags = surfaceNode["f"].GetInt() & 0x1F;
		if (s.flags == 0){
			MINILOG(logWARNING) << " Warning: Invalid surface flags: " << surfaceNode["f"].GetInt();
			s.flags = 1;
		}
		addSurface(s);
	}
	for (auto& spriteNode : spritesNode.GetArray()){
		glm::mat2 sprtransform;
		glm::vec2 sprposition;
		sprposition.x = spriteNode["x"].GetFloat();
		sprposition.y = spriteNode["y"].GetFloat();
		sprtransform[0][0] = spriteNode["t"][0].GetFloat();
		sprtransform[0][1] = spriteNode["t"][1].GetFloat();
		sprtransform[1][0] = spriteNode["t"][2].GetFloat();
		sprtransform[1][1] = spriteNode["t"][3].GetFloat();
		std::string fname = spriteNode["name"].GetString();
		this->addBGTexture(sprposition,sprtransform,fname);
	}
	for (auto& ledgeNode : ledgesNode.GetArray()){
		glm::vec2 pos;
		pos.x = ledgeNode["x"].GetFloat();
		pos.y = ledgeNode["y"].GetFloat();
		ledges.emplace_back(pos);
	}
	for (auto& tilemapNode : tilemapsNode.GetArray()){
		TileMap tm;
		std::string tmname = loadTileMap(tm,tilemapNode);
		internal_tms[tmname] = tm;
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
	sprs[i] = {sprPosition,filename};
	Texture temp = m_atlas.findSubTexture(filename);
	sprs[i].spr.setTexture(temp);
	sprs[i].spr.setPosition(sprPosition);
	sprs[i].spr.transform(sprTransform);
}
uint32_t ObjMap::addSurface(const Surface& wall){
	uint32_t seed;
	do{
		seed = rng();
	} while (surfaces.count(seed) != 0);
	surfaces[seed] = wall;
	return seed;
}
void ObjMap::SetPosition(float x, float y) {
	this->position.x = x;
	this->position.y = y;
	for (auto& i : sprs){
		i.second.spr.setPosition(i.second.iPosition.x + x, i.second.iPosition.y + y);
	}
	this->internal_tms["default"].packedtileSize[2] = x;
	this->internal_tms["default"].packedtileSize[3] = y;
	tm_changed = true;
}
void ObjMap::WriteToFile(const std::string& filename){
	std::ofstream ofs(filename);
	rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
	writer.Key("surfaces");
	
	writer.StartArray();
	for (auto& surf : surfaces){
		auto& i = surf.second;
		writer.StartObject();
		writer.Key("x");
		writer.Int(i.hitbox.left);
		writer.Key("y");
		writer.Int(i.hitbox.top);
		writer.Key("w");
		writer.Int(i.hitbox.width);
		writer.Key("h");
		writer.Int(i.hitbox.height);
		if (i.flags == 0){
			MINILOG(logWARNING) << "Unknown surface flags: " << i.flags;
			i.flags = 1;
		}
		writer.Key("f");
		writer.Int(i.flags);
		writer.EndObject();
	}
	writer.EndArray();
	
	writer.Key("sprites");
	
	writer.StartArray();
	for (auto& spr: sprs){
		auto& i = spr.second;
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
	writer.EndArray();
	
	writer.Key("ledges");
	
	writer.StartArray();
	for (auto& i : ledges){
		writer.StartObject();
		writer.Key("x");
		writer.Int(i.x);
		writer.Key("y");
		writer.Int(i.y);
		writer.EndObject();
	}
	writer.EndArray();
	
	writer.Key("tilemaps");
	writer.StartArray();
	for (auto& tmpair : internal_tms){
		auto& internal_tm = tmpair.second;

		writer.StartObject();

		writer.Key("Name");
		writer.String("tmpair.first");

		writer.Key("AffineT");
		writer.StartArray();
		for (int i = 0; i < 4; i++){
			writer.Double(internal_tm.affineT[i]);
		}
		writer.EndArray();
		
		writer.Key("tiles");
		writer.StartArray();
		for (int i = 0; i < internal_tm.numTiles; i++){
			writer.String(internal_tm.filenames[i].c_str());
		}
		writer.EndArray();
	
		writer.Key("tileSize");
		writer.StartArray();
		writer.Double(internal_tm.packedtileSize[0]);
		writer.Double(internal_tm.packedtileSize[1]);
		writer.EndArray();

		writer.Key("position");
		writer.StartArray();
		writer.Double(internal_tm.packedtileSize[2]);
		writer.Double(internal_tm.packedtileSize[3]);
		writer.EndArray();

		writer.Key("drawntiles");
		writer.StartArray();
		for (auto& i : internal_tm.drawn){
			uint16_t x, y;
			uint8_t index;
			unpackTile(i, x, y, index);
			writer.StartObject();
			writer.Key("x");
			writer.Int(x);
			writer.Key("y");
			writer.Int(y);
			writer.Key("index");
			writer.Int(index);
			writer.EndObject();
		}
		writer.EndArray();

		writer.EndObject();
	}
	writer.EndArray();
	
	writer.EndObject();
	ofs << s.GetString() << std::endl;
	ofs.close();
}
void ObjMap::Draw(SpriteBatch& frame) {
	for (auto& i : sprs){
		frame.Draw(&(i.second.spr));
	}
	if (tm_changed){
		for (auto& tmap : internal_tms){
			frame.addMap(tmap.first, tmap.second);
		}
		tm_changed = false;
	}
}
