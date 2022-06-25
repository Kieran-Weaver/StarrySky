#include <GL/TextureAtlas.hpp>
#include <file/PlainText.hpp>
#include <zlib.h>
#include <glbinding/gl/gl.h>
#include <fstream>
#include <map>

using namespace gl;

struct __attribute__((packed)) tex {
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
	uint8_t  rotated;
};

TextureAtlas::TextureAtlas(const std::string_view file_path){
	MMAPFile fmap(file_path);
	const char* ptr = fmap.getString().data();
	std::map<std::string, size_t> temp_names;
	std::vector<uint16_t> temp_layers;
	std::vector<tex> temp_in_texs;
	std::vector<tex> temp_out_texs;
	
	uint16_t num_layers, num_images;
	
	ptr += read(ptr, num_layers);
	this->num_layers = num_layers;
	if (!num_layers)
		throw std::invalid_argument("Invalid texture atlas");
	
	glGenTextures(1, &handle_);
	glBindTexture(GL_TEXTURE_2D_ARRAY, handle_);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	uint32_t w = 2048, h = 1024;	
	uint32_t level = 0;
	gl::GLenum format = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
	while ((w != 1) || (h != 1)) {
		glTexImage3D(GL_TEXTURE_2D_ARRAY, level, format,
			w, h, num_layers, 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);

		w = std::max(w / 2, 1U);
		h = std::max(h / 2, 1U);
		level++;
	}
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, level - 1);

	std::string path(file_path.substr(0, file_path.find_last_of("\\/") + 1));
	std::string_view textureName;
	for (uint16_t layer = 0; layer < num_layers; layer++) {
		ptr += read(ptr, textureName);

		std::string dds = path;
		dds.append(textureName);
		dds.append(".dds.gz");

		std::string bin = path;
		bin.append(textureName);
		bin.append(".bin.gz");
		
		if (!loadDDSgz(dds, layer))
			throw std::invalid_argument("Invalid DDS file: " + dds);
		
		masks.emplace_back();
		if (!loadBINgz(bin, masks.back()))
			throw std::invalid_argument("Invalid hitbox file: " + bin);
		
		ptr += read(ptr, num_images);
		for (uint16_t image = 0; image < num_images; image++) {
			std::string_view name;
			tex in, out;

			ptr += read(ptr, name);
			ptr += read(ptr, in);

			out.x = static_cast<uint16_t>((in.x * 65536.f) / width);
			out.y = static_cast<uint16_t>((in.y * 65536.f) / height);			

			if (in.rotated) {
				out.w = static_cast<uint16_t>((in.h * 65536.f) / width);
				out.h = static_cast<uint16_t>((in.w * 65536.f) / height);
			} else {
				out.w = static_cast<uint16_t>((in.w * 65536.f) / width);
				out.h = static_cast<uint16_t>((in.h * 65536.f) / height);
			}

			size_t idx = temp_names.size();
			std::string temp_name(name);

			temp_names[temp_name] = idx;
			temp_layers.push_back(layer);
			temp_in_texs.push_back(in);
			temp_out_texs.push_back(out);
		}
	}	

	/* Sort by name */
	names_interned = {};
	for (const auto& [name, idx] : temp_names) {
		const auto& in = temp_in_texs[idx];
		const auto& out = temp_out_texs[idx];
		const auto  name_start = names_interned.size();

		names_interned.append(name);
		names.emplace_back(name_start, name.length());
		layers.push_back(temp_layers[idx]);
		rotated.push_back(in.rotated);
		sizes.push_back(in.w);
		sizes.push_back(in.h);
		uvs.push_back(out.x);
		uvs.push_back(out.y);
		uvs.push_back(out.x + out.w);
		uvs.push_back(out.y + out.h);
	}
}

TextureAtlas::~TextureAtlas(){
	glDeleteTextures(1, &handle_);
}

uint32_t TextureAtlas::handle() const{
	return this->handle_;
}

struct __attribute__((packed)) DDSHdr {
	uint32_t magic;
	uint32_t size;
	uint32_t flags;
	uint32_t height;
	uint32_t width;
	uint32_t unused1[2];
	uint32_t mips;
	uint32_t reserved1[11];
	uint32_t unused2[2];
	uint32_t fourCC;
	uint32_t unused3[10];
};

bool TextureAtlas::loadDDSgz(const std::string_view path, uint16_t layer) {
	gzFile gzhandle = nullptr;
	DDSHdr header;

	gzhandle = gzopen(path.data(), "rb");

	if (gzhandle == nullptr) return false;
	if ((gzread(gzhandle, &header, sizeof(header)) == -1) || (header.magic != 0x20534444)) {
		gzclose_r(gzhandle);
		return false;
	}

	this->width = header.width;
	this->height = header.height;
	
	gl::GLenum format;
	uint32_t blockSize;
	
	switch (header.fourCC) {
		case 0x31545844: // DXT1
			format = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
			blockSize = 8;
			break;
		case 0x33545844: // DXT3
			format = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
			blockSize = 16;
			break;
		case 0x35545844: // DXT5
			format = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
			blockSize = 16;
			break;
		default:
			gzclose_r(gzhandle);
			return false;
			break;
	}
	
	uint32_t size = ((header.width + 3)/4)*((header.height + 3)/4)*blockSize;
	auto pixels = new uint8_t[size];
	for (uint32_t level = 0; level < header.mips; level++) {
		if (gzread(gzhandle, pixels, size) == -1) {
			gzclose_r(gzhandle);
			return false;
		}
		
		glCompressedTexSubImage3D(GL_TEXTURE_2D_ARRAY, level,
			0, 0, layer,
			header.width, header.height, 1,
			format, size, pixels);
		
		header.width = std::max(header.width / 2, 1U);
		header.height = std::max(header.height / 2, 1U);
		size = ((header.width + 3)/4)*((header.height + 3)/4)*blockSize;
	}
	
	delete[] pixels;
	
	gzclose_r(gzhandle);
	return true;
}

bool TextureAtlas::loadBINgz(const std::string_view path, Bitmask& maskwrapper){
	maskwrapper.width = width;
	maskwrapper.height = height;
	gzFile gzhandle = nullptr;
	
	gzhandle = gzopen(path.data(),"rb");
	if (!gzhandle) return false;
	
	size_t bytes = width*height/8;
	maskwrapper.mask = std::shared_ptr<uint32_t>(new uint32_t[bytes/sizeof(uint32_t)]);

	int rt = gzread(gzhandle, maskwrapper.mask.get(), bytes);
	gzclose_r(gzhandle);
	
	return rt != -1;
}

// Finds a Texture inside the atlas table.
// Returns a valid Texture if found in the TextureAtlas, otherwise returns a null Texture.
const Texture TextureAtlas::findSubTexture(const std::string_view name) const{
	Texture texture;
	auto it = std::lower_bound(names.begin(), names.end(), name, [&](const auto& a, const auto& b) {
		const std::string_view view(names_interned.data() + a.first, a.second);
		return view < b;
	});

	if (it == names.end()) return texture;
	
	size_t idx = std::distance(names.begin(), it);
	
	texture.m_texture = handle_;
	texture.m_rect = {
		uvs[idx * 4 + 0], uvs[idx * 4 + 1],
		uvs[idx * 4 + 2], uvs[idx * 4 + 3]
	};
	texture.width = sizes[idx * 2 + 0];
	texture.height = sizes[idx * 2 + 1];
	texture.rotated = rotated[idx];
	texture.layer = layers[idx];
	texture.m_bitmask = masks[layers[idx]];
	texture.type = GL_TEXTURE_2D_ARRAY;

	return texture;
}

// This function gets a list of all the seperate image names that are in the TextureAtlas.
// Returns a std::vector<std::string> full of filenames corresponding to images in the TextureAtlas.
std::vector<std::string> TextureAtlas::getSubTextureNames() const{
	std::vector<std::string> name_vec;

	std::transform(names.begin(), names.end(), std::back_inserter(name_vec), [&](const auto& itr){
		return names_interned.substr(itr.first, itr.second);
	});

	return name_vec;
}
