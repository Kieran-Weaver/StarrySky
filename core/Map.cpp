#include <fstream>
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include "Map.hpp"
#include "ImGuiHelper.hpp"
#include <sstream>
ObjMap::ObjMap(const std::string& filename, TextureAtlas& atlas) : m_atlas(atlas){
	this->loadFromFile(filename);
	this->SetPosition(0,0);
}
void ObjMap::loadFromFile(const std::string& filename){
	ledges.clear();
	surfaces.clear();
	sprs.clear();

	std::string jsondata = readWholeFile(filename);
	rapidjson::Document document;
	rapidjson::ParseResult result = document.Parse(jsondata.c_str());
	if (!result) {
		std::cerr << "ERROR" << std::endl;
	}
	const rapidjson::Value& surfacesNode = document["surfaces"];
	const rapidjson::Value& spritesNode = document["sprites"];
	const rapidjson::Value& ledgesNode = document["ledges"];
	const rapidjson::Value& tilemapNode = document["tilemap"];
	for (auto& surfaceNode : surfacesNode.GetArray()){
		Surface s;
		s.x = surfaceNode["x"].GetFloat();
		s.y = surfaceNode["y"].GetFloat();
		s.length = surfaceNode["l"].GetInt();
		std::string type = surfaceNode["t"].GetString();
		switch (type[0]){
		case 'F':
			s.type = WallType::FLOOR;
			break;
		case 'C':
			s.type = WallType::CEIL;
			break;
		case 'L':
			s.type = WallType::LWALL;
			break;
		case 'R':
			s.type = WallType::RWALL;
			break;
		case 'O':
			s.type = WallType::ONEWAY;
			break;
		default:
			std::cout << type << std::endl;
			s.type = WallType::FLOOR;
			break;
		}
		addSurface(s);
	}
	for (auto& spriteNode : spritesNode.GetArray()){
		glm::vec2 sprscale;
		glm::vec2 sprposition;
		sprposition.x = spriteNode["x"].GetFloat();
		sprposition.y = spriteNode["y"].GetFloat();
		sprscale.x = spriteNode["sx"].GetFloat();
		sprscale.y = spriteNode["sy"].GetFloat();
		std::string fname = spriteNode["name"].GetString();
		this->addBGTexture(sprposition,sprscale,fname);
	}
	for (auto& ledgeNode : ledgesNode.GetArray()){
		glm::vec2 pos;
		pos.x = ledgeNode["x"].GetFloat();
		pos.y = ledgeNode["y"].GetFloat();
		ledges.emplace_back(pos);
	}
	const rapidjson::Value& ATNode = tilemapNode["AffineT"];
	for (int i = 0; i < 4; i++){
		internal_tm.affineT[i] = ATNode[i].GetFloat();
	}
	const rapidjson::Value& tilesNode = tilemapNode["tiles"];
	rapidjson::SizeType numTiles = tilesNode.Size() & 0xff;
	internal_tm.numTiles = numTiles;
	for (int i = 0; i < numTiles; i++){
		const Texture tempTex = m_atlas.findSubTexture(tilesNode[i].GetString());
		internal_tm.tiles[i][0] = tempTex.m_rect.left / 65536.f;
		internal_tm.tiles[i][1] = tempTex.m_rect.top / 65536.f;
		internal_tm.tiles[i][2] = tempTex.m_rect.width / 65536.f;
		internal_tm.tiles[i][3] = tempTex.m_rect.height / 65536.f;
		internal_tm.filenames.emplace_back(tilesNode[i].GetString());
	}
	const rapidjson::Value& sizeNode = tilemapNode["tileSize"];
	for (int i = 0; i < 2; i++){
		internal_tm.packedtileSize[i] = sizeNode[i].GetFloat();
	}
	const rapidjson::Value& posNode = tilemapNode["position"];
	for (int i = 0; i < 2; i++){
		internal_tm.packedtileSize[2 + i] = posNode[i].GetFloat();
	}
	const rapidjson::Value& drawnNode = tilemapNode["drawntiles"];
	for (auto& tileNode : drawnNode.GetArray()){
		Tile temp;
		int px = tileNode["x"].GetInt();
		int py = tileNode["y"].GetInt();
		int index = tileNode["index"].GetInt();
		temp = ((px & 0xFFF) << 20) | ((py & 0x0FFF) << 8) | (index & 0xFF);
		internal_tm.drawn.emplace_back(temp);
	}
	tm_changed = true;
}
void ObjMap::addBGTexture(const glm::vec2& sprPosition, const glm::vec2& sprScale, const std::string& fname){
	int i = sprs.size();
	std::string filename(fname);
	auto end_pos = std::remove(filename.begin(),filename.end(),' ');
	filename.erase(end_pos,filename.end());
	sprs.emplace_back(sprPosition,filename);
	Texture temp = m_atlas.findSubTexture(filename);
	sprs[i].spr.setTexture(&temp);
	sprs[i].spr.setPosition(sprPosition);
	sprs[i].spr.m_model = glm::scale(sprs[i].spr.m_model,glm::vec3(sprScale,1.f));
}
void ObjMap::addSurface(const Surface& wall){
	surfaces.emplace_back(wall);
}
void ObjMap::SetPosition(float x, float y) {
	this->position.x = x;
	this->position.y = y;
	for (auto& i : sprs){
		i.spr.setPosition(i.iPosition.x + x, i.iPosition.y + y);
	}
}
void ObjMap::WriteToFile(const std::string& filename){
	std::ofstream ofs(filename);
	rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
	writer.Key("surfaces");
	
	writer.StartArray();
	for (auto& i : surfaces){
		writer.StartObject();
		writer.Key("x");
		writer.Int(i.x);
		writer.Key("y");
		writer.Int(i.y);
		writer.Key("l");
		writer.Int(i.length);
		writer.Key("t");
		switch (i.type){
		case LWALL:
			writer.String("L");
			break;
		case RWALL:
			writer.String("R");
			break;
		case FLOOR:
			writer.String("F");
			break;
		case CEIL:
			writer.String("C");
			break;
		case ONEWAY:
			writer.String("O");
			break;
		default:
			std::cout << "Unknown type\n";
			break;
		}
		writer.EndObject();
	}
	writer.EndArray();
	
	writer.Key("sprites");
	glm::quat orientation = glm::quat();
	glm::vec3 scale = glm::vec3(), translation = glm::vec3(), skew = glm::vec3();
	glm::vec4 perspective = glm::vec4();
	
	writer.StartArray();
	for (auto& i : sprs){
		writer.StartObject();
		glm::decompose(i.spr.m_model,scale,orientation,translation,skew,perspective);
		writer.Key("x");
		writer.Int(i.iPosition.x);
		writer.Key("y");
		writer.Int(i.iPosition.y);
		writer.Key("sx");
		writer.Double(scale.x);
		writer.Key("sy");
		writer.Double(scale.y);
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
	
	writer.Key("tilemap");
	writer.StartObject();
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
		writer.StartObject();
		writer.Key("x");
		writer.Int(i >> 20);
		writer.Key("y");
		writer.Int((i >> 8) & 0xFFF);
		writer.Key("index");
		writer.Int(i & 0xFF);
		writer.EndObject();
	}
	writer.EndArray();
	writer.EndObject();
	
	writer.EndObject();
	ofs << s.GetString() << std::endl;
	ofs.close();
}
void ObjMap::Draw(SpriteBatch& frame) {
	for (auto& i : sprs){
		frame.Draw(&(i.spr));
	}
	if (tm_changed){
		frame.ChangeMap(internal_tm);
		tm_changed = false;
	}
}
