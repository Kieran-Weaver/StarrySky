#include <GL/TextureAtlas.hpp>
#include <file/PlainText.hpp>
#include <file/JSON.hpp>
#include <zlib.h>
#include <glbinding/gl/gl.h>
#include <fstream>

using namespace gl;

template<>
JSONParser::operator Texture() const{
	Texture data;
	return data;
}
TextureAtlas::TextureAtlas(const std::string_view file_path){
	std::string jsondata = readWholeFile(file_path);
	JSONReader document(jsondata.c_str());
	auto texturesNode = document["textures"];
	std::string path(file_path.substr(0, file_path.find_last_of("\\/") + 1));
	for (int i = 0; i < texturesNode.size(); i++){
		auto atlasNode = texturesNode[i];
		auto& data = this->m_atlas_list.emplace_back();
		glGenTextures(1, &(data.m_texture));
		this->m_texture_handles.emplace_back(data.m_texture);
		std::string textureName = static_cast<std::string>(atlasNode["name"]);
		if (!loadDDSgz(path + textureName + ".dds.gz", data)){
			throw std::invalid_argument("Invalid DDS File: " + path + textureName + ".dds.gz");
		}
		if (!loadBINgz(path + textureName + ".bin.gz",data)){
			throw std::invalid_argument("Invalid hitbox file: " + path + textureName + ".bin.gz");
		}
		for (int j = 0; j < atlasNode["images"].size(); j++){
			auto textureNode = atlasNode["images"][j];
			JSONParser tNode(textureNode);
			std::string tname(tNode["n"]);
			auto& tex = data.m_texture_table[tname];
			Rect<uint16_t> tmp = {
				tNode["x"], tNode["y"],
				tNode["w"], tNode["h"]
			};
			tmp.right = tmp.left + tmp.right;
			tmp.bottom = tmp.top + tmp.bottom;
			float width = data.width;
			float height = data.height;
			tex.width = tmp.right - tmp.left;
			tex.height = tmp.bottom - tmp.top;
			tex.rotated = tNode["r"];
			if (tex.rotated){
				tex.m_rect = {
					static_cast<uint16_t>(tmp.left/width*65536.f), static_cast<uint16_t>(tmp.top/height*65536.f),
					static_cast<uint16_t>((tmp.left + (tmp.bottom - tmp.top))/width*65536.f), static_cast<uint16_t>((tmp.top + (tmp.right - tmp.left))/height*65536.f)
				};
			} else {
				tex.m_rect = {
					static_cast<uint16_t>(tmp.left/width*65536.f), static_cast<uint16_t>(tmp.top/height*65536.f),
					static_cast<uint16_t>(tmp.right/width*65536.f), static_cast<uint16_t>(tmp.bottom/height*65536.f)
				};
			}
		}
	}
	if (m_atlas_list.empty()){
		throw std::invalid_argument("Invalid Texture Atlas");
	}
}
TextureAtlas::~TextureAtlas(){
	glDeleteTextures(m_texture_handles.size(),m_texture_handles.data());
}
bool TextureAtlas::loadDDSgz(const std::string_view path,Atlas& atlas){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,atlas.m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gzFile gzhandle = nullptr;
	gzhandle = gzopen(path.data(),"rb");
	bool return_code = true;
	if (gzhandle != nullptr){
		auto header = new uint32_t[32];
		if ((gzread(gzhandle, header, 128) != -1) && (header[0] == 0x20534444)){
			atlas.height = header[3];
			atlas.width = header[4];
			uint32_t width = atlas.width;
			uint32_t height = atlas.height;
			uint32_t mipmapcount = header[7];
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL, mipmapcount-1);
			uint32_t fourCC = header[21];
			uint32_t blockSize = 16;
			uint32_t size = ((width + 3)/4)*((height + 3)/4)*blockSize;
			switch(fourCC){
				case 0x31545844: // DXT1
					atlas.format = static_cast<uint32_t>(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT);
					blockSize = 8;
					break;
				case 0x33545844: // DXT3
					atlas.format = static_cast<uint32_t>(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT);
					break;
				case 0x35545844: // DXT5
					atlas.format = static_cast<uint32_t>(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT);
					break;
				default:
					atlas.format = 0;
					return_code = false;
					break;
			}
			if (atlas.format != 0){
				auto data = new uint8_t[size];
				for (uint32_t level=0;level<mipmapcount;level++){
					if (gzread(gzhandle,data,size) == -1){
						return_code = false;
						break;
					}else{
						glCompressedTexImage2D(GL_TEXTURE_2D,level,static_cast<gl::GLenum>(atlas.format),width,height,0,size,data);
						width = std::max(width/2, 1U);
						height = std::max(height/2,1U);
						size = ((width + 3)/4)*((height + 3)/4)*blockSize;
					}
				}
				delete[] data;
			}
		}else{
			return_code = false;
		}
		gzclose_r(gzhandle);
		delete[] header;
	}else{
		return_code = false;
	}
	return return_code;
}
bool TextureAtlas::loadBINgz(const std::string_view path, const Atlas& atlas){
	Bitmask maskwrapper;
	maskwrapper.width = atlas.width;
	maskwrapper.height = atlas.height;
	gzFile gzhandle = nullptr;
	gzhandle = gzopen(path.data(),"rb");
	bool return_code = false;
	if (gzhandle != nullptr){
		size_t bytes = atlas.width*atlas.height/8;
		auto *data = new uint32_t[bytes/sizeof(uint32_t)];
		if (gzread(gzhandle,data,bytes) != -1){
			maskwrapper.mask = std::shared_ptr<uint32_t>(data, std::default_delete<uint32_t[]>());
			Bitmasks.insert(std::make_pair(atlas.m_texture,maskwrapper));
			return_code = true;
		}
		gzclose_r(gzhandle);
	}
	return return_code;
}

// Finds a Texture inside the atlas table.
// Returns a valid Texture if found in the TextureAtlas, otherwise returns a null Texture.
const Texture TextureAtlas::findSubTexture(const std::string& name) const{
	Texture texture;
	for(auto& atlas : m_atlas_list){
		const auto& node = atlas.m_texture_table.find(name);
		if(node != atlas.m_texture_table.end()){
			texture.m_texture = atlas.m_texture;
			texture.m_rect = node->second.m_rect;
			texture.width = node->second.width;
			texture.height = node->second.height;
			texture.rotated = node->second.rotated;
			texture.m_bitmask = Bitmasks.at(atlas.m_texture);
			texture.type = GL_TEXTURE_2D;
			break;
		}
	}
	return texture;
}

// This function gets a list of all the seperate image names that are in the TextureAtlas.
// Returns a std::vector<std::string> full of filenames corresponding to images in the TextureAtlas.
std::vector<std::string> TextureAtlas::getSubTextureNames() const{
	std::vector<std::string> names;
	for(const auto& atlas : m_atlas_list)
	{
		std::transform(atlas.m_texture_table.begin(), atlas.m_texture_table.end(), std::back_inserter(names), [](const auto& itr){ return itr.first; });
	}
	return names;
}
