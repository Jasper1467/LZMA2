
#include "Data.h"
#include <fstream>

namespace data {

    std::shared_ptr<RawObject> RawObject::LoadFile(const PATH& p){
		std::ifstream fi(p,std::ios::in | std::ios_base::binary);
		if (!fi.is_open()) { return nullptr; }
		size_t length = std::filesystem::file_size(p);
		unsigned char* buffer = new (std::nothrow) unsigned char[length + 1];
		if (buffer == nullptr) { return nullptr; }
		buffer[length] = '\0';
		fi.read((char*)(&buffer[0]), length);
		return std::shared_ptr<RawObject>()->Create(p, buffer, length);
    }

	bool RawObject::SaveFile(PATH p){
		if (p.empty()) { p = _path; }
		if (!std::filesystem::exists(p.parent_path())) {
			if (!std::filesystem::create_directory(p.parent_path())) return false;
		}
		std::ofstream fo(p,std::ios::out | std::ios::binary);
		if (!fo.is_open()) { return false; }
		fo.write((char*)(_data), _size);
		return true;
	}

	std::shared_ptr<RawObject> RawObject::Adopt(const PATH& p, const void* data, size_t length){
		if (length == 0) { return MakeEmpty(); }
		return std::make_shared<RawObject>(p, data, length);
	}

	std::shared_ptr<RawObject> RawObject::Copy(const PATH& p, const void* data, size_t length) {
		if (length == 0) { return MakeEmpty(); }
		unsigned char* buffer = new (std::nothrow) unsigned char[length + 1];
		if (buffer == nullptr) { return nullptr; }
		buffer[length] = '\0';
		memcpy_s(buffer, length, data, length);
		return std::shared_ptr<RawObject>()->Create(p, buffer, length);
	}

	std::shared_ptr<RawObject> RawObject::MakeEmpty(){
		return std::shared_ptr<RawObject>()->Create();
	}

}