#include "LZMA2.h"
#include <iostream>
#include <sstream>
extern "C" {
#include <lzma.h>
}

namespace LZMA2 {

	static bool init_encoder(lzma_stream* strm) {
		lzma_ret ret = lzma_easy_encoder(strm, 7, LZMA_CHECK_CRC32);

		if (ret == LZMA_OK)
			return true;
		return false;
	}

	static bool init_decoder(lzma_stream* strm) {
		lzma_ret ret = lzma_stream_decoder(strm, UINT64_MAX, LZMA_CONCATENATED);
		if (ret == LZMA_OK) { return true; }
		return false;
	}
	/*
	* Taken primarily from the examples in XZ lzmalib. However special thanks to https://github.com/elsamuko for his example using istream and ostream to point me in the right direction for using stringstream as a buffer.
	* Compressor function is identical for compression and decompression, only taking a different lzma_stream and different input. 
	*/
	static int compressor(lzma_stream* strm, std::stringstream& istream, std::stringstream& ostream){
		lzma_action action = LZMA_RUN;

		uint8_t inbuf[BUFSIZ];
		uint8_t outbuf[BUFSIZ];

		strm->next_in = NULL;
		strm->avail_in = 0;
		strm->next_out = outbuf;
		strm->avail_out = sizeof(outbuf);

		while (true) {
			if (strm->avail_in == 0 && !istream.eof()) {
				strm->next_in = inbuf;
				istream.read((char*)inbuf, sizeof(inbuf));
				strm->avail_in = istream.gcount();
				if(istream.eof()){ action = LZMA_FINISH; }
			}
			lzma_ret ret = lzma_code(strm, action);
			if (strm->avail_out == 0 || ret == LZMA_STREAM_END) {
				size_t write_size = sizeof(outbuf) - strm->avail_out;
				ostream.write((char*)outbuf, write_size);
				if (!ostream.good()) { return false; }
				strm->next_out = outbuf;
				strm->avail_out = sizeof(outbuf);
			}
			if (ret != LZMA_OK) {
				if (ret != LZMA_STREAM_END) { std::cout << "We encountered an error: Code " << std::to_string(ret) << std::endl; }
				return ret;
			}
		}
	}
	/*
	* Decompress and Compress functions use my own object store method. 
	* I don't like this function, but it works for now.
	* There is probably a better way to get the data into stringstream, 
	* but the fiddliness of const void* to nonconst stringstream... I decided to copy the data. 
	* I'm also doing no error hanadling/checking, but in production I'd be handling the output from compressor
	*/
	data::Raw Decompress(const data::Raw& in){
		char* tmp = new char[in->size()];
		memcpy(tmp,in->GetData(),in->size());
		std::stringstream inbuf;
		inbuf.write(tmp, in->size());
		std::stringstream outbuf;
		
		lzma_stream strm = LZMA_STREAM_INIT;
		if (init_decoder(&strm)) { compressor(&strm, inbuf, outbuf); }
		lzma_end(&strm);
		delete[] tmp;
		return data::RawObject::Copy(in->GetPath(),(void*)outbuf.str().c_str(),outbuf.str().length());
	}

	data::Raw Compress(const data::Raw& in){
		char* tmp = new char[in->size()];
		memcpy(tmp, in->GetData(), in->size());
		std::stringstream inbuf;
		inbuf.write(tmp, in->size());
		std::stringstream outbuf;

		lzma_stream strm = LZMA_STREAM_INIT;
		if (init_encoder(&strm)) { compressor(&strm, inbuf, outbuf); }
		lzma_end(&strm);
		delete[] tmp;
		return data::RawObject::Copy(in->GetPath(), (void*)outbuf.str().c_str(), outbuf.str().length());
	}
}