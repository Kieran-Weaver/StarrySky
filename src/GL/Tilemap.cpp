#include <GL/Tilemap.hpp>
#include <gl.h>
TileMap::TileMap(const JSONParser& node, const TextureAtlas& atlas){
	this->load(node, atlas);
}
TileMap& TileMap::operator=(TileMap&& other){
	this->tileBufferTBO = std::move(other.tileBufferTBO);
	other.tileBufferTBO = 0;
	this->tileTextureTBO = std::move(other.tileTextureTBO);
	other.tileTextureTBO = 0;
	this->initialized = std::move(other.initialized);
	other.initialized = false;

	
	this->AffineT = std::move(other.AffineT);
	this->Attrs = std::move(other.Attrs);
	this->metadata = std::move(other.metadata);
	this->texData = std::move(other.texData);

	this->tileData = std::move(other.tileData);
	this->drawn = std::move(other.drawn);
	this->tileBuffer = std::move(other.tileBuffer);
	this->tileTexture = std::move(other.tileTexture);
	this->numTiles = std::move(other.numTiles);
	this->filenames = std::move(other.filenames);
	this->type = std::move(other.type);

	return *this;
}
TileMap::~TileMap(){
	glDeleteTextures(1, &tileBufferTBO);
	glDeleteTextures(1, &tileTextureTBO);
}
void TileMap::load(const JSONParser& node, const TextureAtlas& atlas){
	*this = node;
	for (auto& tfile : filenames){
		const Texture tempTex = atlas.findSubTexture(tfile);
		tileData.emplace_back();
		tileData.back()[0] = tempTex.m_rect.left / 65536.f;
		tileData.back()[1] = tempTex.m_rect.top / 65536.f;
		tileData.back()[2] = tempTex.m_rect.width / 65536.f;
		tileData.back()[3] = tempTex.m_rect.height / 65536.f;
		numTiles++;
	}
	this->loadTiles();
}
void TileMap::loadTiles(){
	if (!tileBufferTBO){
		glGenTextures(1, &tileBufferTBO);
	}
	tileBuffer.setType(GL_TEXTURE_BUFFER);
	tileBuffer.bind();
	tileBuffer.update(tileData, 0);
	if (!tileTextureTBO){
		glGenTextures(1, &tileTextureTBO);
	}
	tileTexture.setType(GL_TEXTURE_BUFFER);
	tileTexture.bind();
	tileTexture.update(drawn, 0);
	this->initialized = true;
}
void TileMap::bind(Buffer& UBO, uint32_t offset) const{
	if (initialized){
		UBO.bind();
		UBO.update(this, sizeof(UBOData), offset);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_BUFFER, tileBufferTBO);
		tileBuffer.bind(GL_RGBA32F);
		
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_BUFFER, tileTextureTBO);
		tileTexture.bind(GL_R16UI);
	}
}
