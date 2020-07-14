#include <GL/SpriteBatch.hpp>
#include <GL/SpriteBatchImpl.hpp>
SpriteBatch::SpriteBatch(TextureAtlas& atlas, const std::string& shaderfile) : m_pImpl(new SpriteBatchImpl(atlas, shaderfile)){}
SpriteBatch::SpriteBatch(SpriteBatch &&) noexcept = default;
SpriteBatch& SpriteBatch::SpriteBatch::operator=(SpriteBatch &&) noexcept = default;
int SpriteBatch::loadPrograms(int num_shaders){
	return Pimpl()->loadPrograms(num_shaders);
}
void SpriteBatch::addMap(const std::string& id, const TileMap& tm){
	Pimpl()->addMap(id, tm);
}
void SpriteBatch::Draw(Sprite& spr){
	Pimpl()->Draw(spr);
}
#ifndef NO_IMGUI
void SpriteBatch::Draw(const ImDrawData* overlay){
	Pimpl()->Draw(overlay);
}
#endif
void SpriteBatch::Draw(const Window& target){
	Pimpl()->Draw(target);
}
void SpriteBatchImplDeleter::operator()(SpriteBatchImpl *p){
	delete p;
}
