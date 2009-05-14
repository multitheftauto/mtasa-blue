/// \file
/// \brief DataCompressor does compression on a block of data.  Not very good compression, but it's small and fast so is something you can use per-message at runtime.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.



#ifndef __DATA_COMPRESSOR_H
#define __DATA_COMPRESSOR_H

#include "RakMemoryOverride.h"
#include "DS_HuffmanEncodingTree.h"
#include "Export.h"

/// \brief Does compression on a block of data.  Not very good compression, but it's small and fast so is something you can compute at runtime.
class RAK_DLL_EXPORT DataCompressor
{
public:
	static void Compress( unsigned char *userData, unsigned sizeInBytes, RakNet::BitStream * output );
	static unsigned DecompressAndAllocate( RakNet::BitStream * input, unsigned char **output );
};

#endif
