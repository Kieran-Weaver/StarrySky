#include <fstream>
#include <iostream>
#include <GL/JSONHelper.hpp>
#include "Map.hpp"
#include "../imgui/imgui.h"
#include "ImGuiHelper.hpp"
#include <sstream>
ObjMap::ObjMap(const std::string& filename, TextureAtlas& atlas) : m_atlas(atlas){
	this->loadFromFile(filename);
	this->SetPosition(0,0);
}
glm::vec2 get_xy(const sajson::value& value){
	return glm::vec2(get_int(value, "x"), get_int(value, "y"));
}
void ObjMap::loadFromFile(const std::string& filename){
	ledges.clear();
	surfaces.clear();
	sprs.clear();

	std::string jsondata = readWholeFile(filename);
	char *jdata = new char[jsondata.length()+1];
	std::strncpy(jdata, jsondata.c_str(), jsondata.length());
	const sajson::document &document = sajson::parse(sajson::dynamic_allocation(), sajson::mutable_string_view(jsondata.length(), jdata));
	const sajson::value surfacesNode = get_node(document.get_root(), "surfaces");
	const sajson::value spritesNode = get_node(document.get_root(), "sprites");
	const sajson::value ledgesNode = get_node(document.get_root(), "ledges");
	int surfacesize = surfacesNode.get_length();
	int sprssize = spritesNode.get_length();
	int ledgesize = ledgesNode.get_length();
	for (int i=0;i<surfacesize;i++){
		const sajson::value surfaceNode = surfacesNode.get_array_element(i);
		Surface s;
		glm::vec2 pos = get_xy(surfaceNode);
		s.x = pos.x;
		s.y = pos.y;
		s.length = get_int(surfaceNode, "l");
		std::string type = get_string(surfaceNode, "t");
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
	for (int i=0;i<sprssize;i++){
		const sajson::value spriteNode = spritesNode.get_array_element(i);
		glm::vec2 sprscale;
		glm::vec2 sprposition = get_xy(spriteNode);
		sprscale.x = get_double(spriteNode, "sx");
		sprscale.y = get_double(spriteNode, "sy");
		std::string fname = get_string(spriteNode, "name");
		this->addBGTexture(sprposition,sprscale,fname);
	}
	for (int i=0;i<ledgesize;i++){
		const sajson::value ledgeNode = ledgesNode.get_array_element(i);
		ledges.emplace_back(get_xy(ledgeNode));
	}
	delete[] jdata;
}
void ObjMap::addBGTexture(const glm::vec2& sprPosition, const glm::vec2& sprScale, const std::string& fname){
	int i = sprs.size();
	std::string filename(fname);
	auto end_pos = std::remove(filename.begin(),filename.end(),' ');
	filename.erase(end_pos,filename.end());
	sprs.emplace_back(sprPosition,filename,Sprite());
	sprs[i].spr.setTexture(m_atlas.findSubTexture(filename));
	sprs[i].spr.setPosition(sprPosition);
	sprs[i].spr.m_model = glm::scale(sprs[i].spr.m_model,glm::vec3(sprScale,1.f));
}
void ObjMap::addSurface(const Surface& wall){
	surfaces.emplace_back(wall);
}
void ObjMap::SetPosition(float x, float y) {
	this->position.x = x;
	this->position.y = y;
	for (unsigned int i=0;i<sprs.size();i++){
		sprs[i].spr.setPosition(sprs[i].iPosition.x + x, sprs[i].iPosition.y + y);
	}
}
void ObjMap::WriteToFile(const std::string& filename){
	std::ofstream ofs(filename);
	ofs << surfaces.size() << " " << sprs.size() << " " << ledges.size() << "\n";
	for (auto& i : surfaces){
		ofs << i.x << " " << i.y << " " << i.length << " ";
		switch (i.type){
		case LWALL:
			ofs << "L\n";
			break;
		case RWALL:
			ofs << "R\n";
			break;
		case FLOOR:
			ofs << "F\n";
			break;
		case CEIL:
			ofs << "C\n";
			break;
		case ONEWAY:
			ofs << "O\n";
			break;
		default:
			std::cout << "Unknown type\n";
			break;
		}
	}
	glm::quat orientation = glm::quat();
	glm::vec3 scale = glm::vec3(), translation = glm::vec3(), skew = glm::vec3();
	glm::vec4 perspective = glm::vec4();
	for (int i = (sprs.size()-1);i>=0;i--){
		glm::decompose(sprs[i].spr.m_model,scale,orientation,translation,skew,perspective);
		ofs << sprs[i].iPosition.x << " " << sprs[i].iPosition.y << " " << scale.x << " " << scale.y << " " << sprs[i].filename << "\n";
	}
	for (auto& i : ledges){
		ofs << i.x << " " << i.y << "\n";
	}
	ofs << std::endl;
	ofs.close();
}
void ObjMap::Draw(SpriteBatch& frame) {
	for (auto& i : sprs){
		frame.Draw(&(i.spr));
	}
}
