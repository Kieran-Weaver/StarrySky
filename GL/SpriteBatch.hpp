#ifndef STARRYSKY_SPRITEBATCH_H
#define STARRYSKY_SPRITEBATCH_H
#include "WindowState.h"
#include "TextureAtlas.hpp"
#include <GLFW/glfw3.h>
// Rendering Modes
#define SPRITE2D 0
#define TILEMAP 1
#define SPRITE3D 2

struct Sprite;
struct TileMap;
class SpriteBatchImpl;
struct SpriteBatchImplDeleter{
	void operator()(SpriteBatchImpl *p);
};
class SpriteBatch{
public:
	SpriteBatch(TextureAtlas& atlas, WindowState& ws, const std::string& shaderfile);
	~SpriteBatch() = default;
	SpriteBatch(SpriteBatch &&) noexcept;
	SpriteBatch& operator=(SpriteBatch &&) noexcept;
	SpriteBatch(const SpriteBatch& rhs) = delete;
	SpriteBatch& operator=(const SpriteBatch& rhs) = delete;
	int loadPrograms(int num_shaders,GLuint* VAOs);
	void Draw(Sprite* spr);
	void ChangeMap(const TileMap& tm);
	void Draw(GLFWwindow* target);
private:
	const SpriteBatchImpl* Pimpl() const { return m_pImpl.get(); }
	SpriteBatchImpl* Pimpl(){ return m_pImpl.get(); }
	std::unique_ptr<SpriteBatchImpl,SpriteBatchImplDeleter> m_pImpl;
};
#endif
