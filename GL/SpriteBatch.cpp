#include "SpriteBatch.hpp"
#include "SpriteBatchImpl.hpp"
SpriteBatch::SpriteBatch(TextureAtlas& atlas, WindowState& ws) : m_pImpl(new SpriteBatchImpl(atlas, ws)){}
SpriteBatch::SpriteBatch(SpriteBatch &&) noexcept = default;
SpriteBatch& SpriteBatch::SpriteBatch::operator=(SpriteBatch &&) noexcept = default;
int SpriteBatch::loadPrograms(const std::string& filename,int num_shaders,GLuint* VAOs){
	return Pimpl()->loadPrograms(filename,num_shaders,VAOs);
}
void SpriteBatch::Draw(Sprite* spr){
	Pimpl()->Draw(spr);
}
void SpriteBatch::ChangeMap(TileMap* tm){
	Pimpl()->ChangeMap(tm);
}
void SpriteBatch::Draw(GLFWwindow* target){
	Pimpl()->Draw(target);
}
void SpriteBatchImplDeleter::operator()(SpriteBatchImpl *p){
	delete p;
}