#ifndef EDITOR_HPP
#define EDITOR_HPP
#include <glm/glm.hpp>
#include "Map.hpp"
#include <GL/Window.hpp>
#include <string>
class LevelEditor{
public:
	LevelEditor(ObjMap& map);
	void Draw(Window window);
private:
	ObjMap& map;
	int editorSpriteID;
	int editorSurfaceID;
	int editorLedgeID;
};
bool Mat2GUI(glm::mat2& mdata, const std::string& tag, const float& max_value);
#endif