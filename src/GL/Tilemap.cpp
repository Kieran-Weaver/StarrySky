#include <GL/Tilemap.hpp>
#include <gl.h>
#include <iostream>
TileMap::TileMap(const JSONParser& node, const TextureAtlas& atlas){
	this->load(node, atlas);
}
TileMap& TileMap::operator=(TileMap&& other){
	this->tileBufferTBO = std::move(other.tileBufferTBO);
	other.tileBufferTBO = {};
	this->tileTextureTBO = std::move(other.tileTextureTBO);
	other.tileTextureTBO = {};
	this->tileBuffer = std::move(other.tileBuffer);
	other.tileBuffer = {};
	this->tileTexture = std::move(other.tileTexture);
	other.tileTexture = {};
	this->initialized = std::move(other.initialized);
	other.initialized = false;

	
	this->AffineT = std::move(other.AffineT);
	this->Attrs = std::move(other.Attrs);
	this->metadata = std::move(other.metadata);
	this->texData = std::move(other.texData);

	this->tileData = std::move(other.tileData);
	this->drawn = std::move(other.drawn);
	this->numTiles = std::move(other.numTiles);
	this->filenames = std::move(other.filenames);
	this->type = std::move(other.type);

	return *this;
}
TileMap::~TileMap(){
	glDeleteTextures(1, &tileBufferTBO.m_texture);
	glDeleteTextures(1, &tileTextureTBO.m_texture);
}
void TileMap::load(const JSONParser& node, const TextureAtlas& atlas){
	*this = node;
	this->atlasTexture.type = GL_TEXTURE_2D;
	this->tileBufferTBO.type = GL_TEXTURE_BUFFER;
	this->tileTextureTBO.type = GL_TEXTURE_BUFFER;
	for (auto& tfile : filenames){
		const Texture tempTex = atlas.findSubTexture(tfile);
		this->atlasTexture.m_texture = tempTex.m_texture;
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
	if (!tileBufferTBO.m_texture){
		glGenTextures(1, &tileBufferTBO.m_texture);
	}
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, this->tileBufferTBO.m_texture);
	tileBuffer.setType(GL_TEXTURE_BUFFER);
	tileBuffer.bind(GL_RGBA32F);
	tileBuffer.update(tileData, 0);
	if (!tileTextureTBO.m_texture){
		glGenTextures(1, &tileTextureTBO.m_texture);
	}
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_BUFFER, this->tileTextureTBO.m_texture);
	tileTexture.setType(GL_TEXTURE_BUFFER);
	tileTexture.bind(GL_R16UI);
	tileTexture.update(drawn, 0);
	this->initialized = true;
}
