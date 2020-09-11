#include <GL/Tilemap.hpp>
#include <gl.h>
template<>
JSONParser::operator TMType() const{
	std::string data{internal.GetString()};
	if (data == "normal"){
		return TMType::Normal;
	} else if (data == "effect"){
		return TMType::Effect;
	} else {
		return TMType::Normal;
	}
}
TileMap::TileMap(){
	this->atlasTexture.type = GL_TEXTURE_2D;
	this->tileBufferTBO.type = GL_TEXTURE_BUFFER;
	this->tileTextureTBO.type = GL_TEXTURE_BUFFER;
}
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
void TileMap::addTile(const std::string& filename, const Texture& tile){
	this->filenames.emplace_back(filename);
	this->atlasTexture.m_texture = tile.m_texture;
	this->tileData.emplace_back();
	this->tileData.back()[0] = tile.m_rect.left / 65536.f;
	this->tileData.back()[1] = tile.m_rect.top / 65536.f;
	this->tileData.back()[2] = tile.m_rect.width / 65536.f;
	this->tileData.back()[3] = tile.m_rect.height / 65536.f;
	this->numTiles++;
}
void TileMap::load(const JSONParser& node, const TextureAtlas& atlas){
	*this = node;
	this->atlasTexture.type = GL_TEXTURE_2D;
	this->tileBufferTBO.type = GL_TEXTURE_BUFFER;
	this->tileTextureTBO.type = GL_TEXTURE_BUFFER;
	std::vector<std::string> fnames;
	fnames.swap(filenames);
	filenames.emplace_back("empty");
	for (auto& tfile : fnames){
		this->addTile(tfile, atlas.findSubTexture(tfile));
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
