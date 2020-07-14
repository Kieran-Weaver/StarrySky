#ifndef STARRYSKY_SPRITEBATCH_H
#define STARRYSKY_SPRITEBATCH_H
#include <GL/TextureAtlas.hpp>
#include <GL/Window.hpp>
struct Sprite;
struct TileMap;
struct ImDrawData;
class SpriteBatchImpl;
struct SpriteBatchImplDeleter{
	void operator()(SpriteBatchImpl *p);
};
class SpriteBatch{
public:
	SpriteBatch(TextureAtlas& atlas, const std::string& shaderfile);
	~SpriteBatch() = default;
	SpriteBatch(SpriteBatch &&) noexcept;
	SpriteBatch& operator=(SpriteBatch &&) noexcept;
	SpriteBatch(const SpriteBatch& rhs) = delete;
	SpriteBatch& operator=(const SpriteBatch& rhs) = delete;
	int loadPrograms(int num_shaders);
	void addMap(const std::string& id, const TileMap& tm);
	void Draw(Sprite& spr);
	void Draw(const Window& target);
	void Draw(const ImDrawData* overlay);
private:
	const SpriteBatchImpl* Pimpl() const { return m_pImpl.get(); }
	SpriteBatchImpl* Pimpl(){ return m_pImpl.get(); }
	std::unique_ptr<SpriteBatchImpl,SpriteBatchImplDeleter> m_pImpl;
};
#endif
