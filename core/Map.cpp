#include <fstream>
#include <iostream>
#include "Map.hpp"
#include "../imgui/imgui.h"
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
	std::ifstream mapfile(filename);
	std::string line;
	std::getline(mapfile,line);
	std::istringstream is(line);
	int surfacesize = 0;
	int sprssize = 0;
	int ledgesize = 0;
	is >> surfacesize >> sprssize >> ledgesize;
	for (int i=0;i<surfacesize;i++){
		std::getline(mapfile,line);
		std::istringstream iss(line);
		Surface s;
		iss >> s.x >> s.y >> s.length;
		std::string type;
		std::getline(iss,type);
		std::string::iterator end_pos = std::remove(type.begin(),type.end(),' ');
		type.erase(end_pos,type.end());
		if (type == "F"){
			s.type = WallType::FLOOR;
		}else if (type == "C"){
			s.type = WallType::CEIL;
		}else if (type == "L"){
			s.type = WallType::LWALL;
		}else if (type == "R"){
			s.type = WallType::RWALL;
		}else if (type == "O"){
			s.type = WallType::ONEWAY;
		}else{
			std::cout << type;
			s.type = WallType::FLOOR;
		}
		addSurface(s);
	}
	for (int i=0;i<sprssize;i++){
		std::getline(mapfile,line);
		std::istringstream iss(line);
		glm::vec2 sprscale;
		glm::vec2 sprposition;
		std::string fname;
		iss >> sprposition.x >> sprposition.y >> sprscale.x >> sprscale.y;
		std::getline(iss,fname);
		this->addBGTexture(sprposition,sprscale,fname);
	}
	for (int i=0;i<ledgesize;i++){
		std::getline(mapfile,line);
		std::istringstream iss(line);
		glm::vec2 ledge;
		iss >> ledge.x >> ledge.y;
		ledges.emplace_back(ledge);
	}
	mapfile.close();
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
