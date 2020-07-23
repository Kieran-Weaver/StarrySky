#include <GL/SpriteBatch.hpp>
#include <GL/SpriteBatchImpl.hpp>
SpriteBatch::SpriteBatch(TextureAtlas& atlas, const std::string& shaderfile) : m_pImpl(new SpriteBatchImpl(atlas, shaderfile)){}
SpriteBatch::SpriteBatch(SpriteBatch &&) noexcept = default;
SpriteBatch& SpriteBatch::SpriteBatch::operator=(SpriteBatch &&) noexcept = default;
int SpriteBatch::loadPrograms(int num_shaders){
	return Pimpl()->loadPrograms(num_shaders);
}
void SpriteBatch::Draw(TileMap& tm){
	Pimpl()->Draw(tm);
}
void SpriteBatch::Draw(Sprite& spr){
	Pimpl()->Draw(spr);
}
void SpriteBatch::EndFrame(const Window& target){
	Pimpl()->EndFrame(target);
}
void SpriteBatchImplDeleter::operator()(SpriteBatchImpl *p){
	delete p;
}
