#ifndef TEXTUREATLAS_H
#define TEXTUREATLAS_H

#include <fstream>
#include <vector>
#include <sparsehash/dense_hash_map>
#include <cstring>
#include <zlib.h>
#include <gl.h>
#include <GL/Rect.hpp>
#include "Texture.h"

/* Class for loading .bin files and associated .dds.gz files, generated by recrunch */
class TextureAtlas{
public:
	bool loadFromFile(const std::string& file_path);
	const Texture findSubTexture(const std::string& name);
	std::vector<std::string> getSubTextureNames();
	~TextureAtlas(){
		if (m_texture_handles != nullptr){
			glDeleteTextures(m_num_textures,m_texture_handles);
			delete[] m_texture_handles;
		}
	}
	GLuint* m_texture_handles = nullptr;
	int m_num_textures;
	private:
	struct Atlas
	{
		Atlas() : m_texture(nullptr){
			m_texture_table.set_empty_key("");
		}
		google::dense_hash_map<std::string, Texture> m_texture_table;
		GLuint* m_texture = nullptr;
		GLuint format;
		uint16_t width=0, height=0;
	};
	int loadDDSgz(const std::string& path, Atlas& atlas);
	std::vector<Atlas> m_atlas_list;
};

#endif
