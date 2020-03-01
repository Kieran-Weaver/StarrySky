#ifndef EDITOR_HPP
#define EDITOR_HPP
#include <glm/glm.hpp>
#include "Map.hpp"
#include <GL/Window.hpp>
#include <GL/SpriteBatch.hpp>
#include <GL/Sprite.hpp>
#include <string>
class LevelEditor{
public:
	LevelEditor(ObjMap& map);
	void Draw(SpriteBatch& batch);
private:
	ObjMap& map;
	Texture selectedTex;
	std::vector<Sprite> sprites;
	std::vector<Tile> tiles;
	std::vector<Rect<int>> rects;
};
bool Mat2GUI(glm::mat2& mdata, const std::string& tag, const float& max_value);
#endif
