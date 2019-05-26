#define TRIM
#define ROTATE
#include <fstream>
#include <iostream>
#include <Magick++.h>
#include <zlib.h>
std::fstream::pos_type getFSize(const std::string& path){
	std::ifstream handle(path,std::ios::binary|std::ios::in|std::ios::ate);
	if (handle){
		return handle.tellg();
	}else{
		return 0;
	}
}
uint16_t getUint16_t(const uint8_t *data, int pos){
	return data[pos] + (static_cast<uint16_t>(data[pos+1])<<8);
}
std::string getString(const uint8_t *data,int start, int length){
	int end=start;
	const char* chardata = reinterpret_cast<const char*>(data);
	while (end<length && (chardata[end] != 0)){
		end++;
	}
	return std::string(chardata+start,end-start);
}
void recrunch(const std::string& atlasname){
	std::ifstream atlashandle(atlasname + ".bin", std::ios::binary|std::ios::in);
	std::ofstream temphandle;
	uint16_t num_images = 0, num_textures=0;
	int pos = 2;
	if (atlashandle){
		atlashandle.seekg(0,atlashandle.end);
		int length = atlashandle.tellg();
		atlashandle.seekg(0,atlashandle.beg);
		uint8_t * data = new uint8_t[length];
		atlashandle.read(reinterpret_cast<char*>(data),length);
		num_textures = getUint16_t(data,0);
		std::cout << num_textures << std::endl;
		for (int i=0;i<num_textures;i++){
			std::string name = getString(data,pos,length);
			std::cout << name << std::endl;
			pos += name.size() + 1;
			num_images = getUint16_t(data,pos);
			pos += 2;
			for (int j=0;j<num_images;j++){
				std::string img_name = getString(data,pos,length);
				pos += img_name.size() + 9;
#ifdef TRIM
				pos += 8;
#endif
#ifdef ROTATE
				pos++;
#endif
			}
			Magick::Image image;
			Magick::Blob blob;
			try{
				image.read(name + ".png");
				image.magick("DDS");
				image.defineValue("DDS","compression","dxt5");
				image.write(&blob,"DDS");
				std::string outputfname = name + ".dds.gz";
				gzFile gzhandle = gzopen(outputfname.c_str(),"wb");
				gzwrite(gzhandle,blob.data(),blob.length());
				gzclose_w(gzhandle);
			}catch( std::exception& error_){
				std::cout << "Caught Exception: " << error_.what() << std::endl;
			}
		}
	}
	atlashandle.close();
}
int main(int argc, char**argv){
	Magick::InitializeMagick(*argv);
	if (argc < 2){
		std::cout << "Usage: ./recrunch FILENAME" << std::endl;
	}else{
		std::string fname(argv[1]);
		recrunch(fname);
	}
	return 0;
}
