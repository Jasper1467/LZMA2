#include "Data.h"
#include "LZMA2.h"
//for testing only.
int main()
{
	data::PATH cur = std::filesystem::current_path();
	data::Raw in = data::RawObject::LoadFile(cur / "Example.json");
	data::Raw compressed = LZMA2::Compress(in);
	compressed->SetPath(cur / "Out.xz");
	compressed->SaveFile();
	data::Raw in2 = data::RawObject::LoadFile(cur / "Out.xz");
	data::Raw decompressed = LZMA2::Decompress(compressed);
	decompressed->SetPath(cur / "Out.json");
	decompressed->SaveFile();
	return 0;
}