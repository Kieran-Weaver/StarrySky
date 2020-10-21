#include <string_view>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <file/JSONWriter.hpp>

JSONWriter::JSONWriter(){
	buffer.reset(new StringBuffer_t);
	writer.reset(new decltype(writer)::element_type(*buffer));
}

void JSONWriter::Key(const std::string& name){
	writer->Key(name.c_str());
}

void JSONWriter::StartArray(){
	writer->StartArray();
}

void JSONWriter::EndArray(){
	writer->EndArray();
}

void JSONWriter::StartObject(){
	writer->StartObject();
}

void JSONWriter::EndObject(){
	writer->EndObject();
}

std::string_view JSONWriter::GetString(){
	return buffer->GetString();
}

void JSONWriter::store(std::string_view data){
	writer->String(data.data());
}

void JSONWriter::store(double data){
	writer->Double(data);
}

void JSONWriter::store(int64_t data){
	writer->Int64(data);
}

void JSONWriter::store(bool data){
	writer->Bool(data);
}

void SBDeleter::operator()(rapidjson::StringBuffer* sb){
	delete sb;
}

void WrDeleter::operator()(rapidjson::Writer<rapidjson::StringBuffer>* wr){
	delete wr;
}
