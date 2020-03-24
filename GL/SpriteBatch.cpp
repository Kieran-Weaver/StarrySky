#include "SpriteBatch.hpp"
#include "SpriteBatchImpl.hpp"
SpriteBatch::SpriteBatch(TextureAtlas& atlas, const std::string& shaderfile) : m_pImpl(new SpriteBatchImpl(atlas, shaderfile)){}
SpriteBatch::SpriteBatch(SpriteBatch &&) noexcept = default;
SpriteBatch& SpriteBatch::SpriteBatch::operator=(SpriteBatch &&) noexcept = default;
int SpriteBatch::loadPrograms(int num_shaders,GLuint* VAOs){
	return Pimpl()->loadPrograms(num_shaders,VAOs);
}
void SpriteBatch::addMap(const std::string& id, const TileMap& tm){
	Pimpl()->addMap(id, tm);
}
void SpriteBatch::Draw(Sprite& spr){
	Pimpl()->Draw(spr);
}
void SpriteBatch::Draw(const Window& target){
	Pimpl()->Draw(target);
}
void SpriteBatchImplDeleter::operator()(SpriteBatchImpl *p){
	delete p;
}
