#pragma once

#include "Data.h"

namespace LZMA2{

	data::Raw Compress(const data::Raw& in);
	data::Raw Decompress(const data::Raw& in);
}
