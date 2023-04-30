#pragma once

#include <memory>
#include <filesystem>

namespace data {

	//typedefs for simpler reference.
	typedef std::filesystem::path PATH;

	/*
	* Generic Data object containing arbitrary data as const void*
	*/
	class RawObject : std::enable_shared_from_this<RawObject> {
	public:
		//Shared Pointer interface implementation
		std::shared_ptr<RawObject> GetPtr() { return shared_from_this(); }
		[[nodiscard]] static std::shared_ptr<RawObject> Create() { return std::make_shared<RawObject>(); }
		[[nodiscard]] static std::shared_ptr<RawObject> Create(const PATH& p, const void* data, const size_t size) { return std::make_shared<RawObject>(p, data, size); }

		//Create data from file path.
		static std::shared_ptr<RawObject> LoadFile(const PATH& p);
		//Write stored data to file at stored path.
		bool SaveFile(PATH p = {});
		//Adopt data from outside facility.
		static std::shared_ptr<RawObject> Adopt(const PATH& p, const void* data, size_t length);
		//create data from other data.
		static std::shared_ptr<RawObject> Copy(const PATH& p, const void* data, size_t length);
		//creates a new empty data.
		static std::shared_ptr<RawObject> MakeEmpty();

		//raw pointer to data.
		inline const void* GetData() const { return _data; }
		//reference to path.
		inline const PATH GetPath() const { return _path; }
		inline void SetPath(const PATH& p){ _path = p; }
		//uint8 byte for easy offset.
		inline const char* GetBytes() const
			{ return static_cast<const char*>(_data); }
		inline size_t size() const { return _size; }
	private:
		RawObject() = default;
		RawObject(const PATH& p, const void* data, const size_t size)
			:_path(p), _data(data), _size(size) {}
		const void* _data = nullptr;
		PATH _path = PATH();
		size_t _size = 0;
	};
	typedef std::shared_ptr<RawObject> Raw;

}