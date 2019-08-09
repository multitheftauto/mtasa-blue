#pragma once

#include "RwTypes.h"
#include "Stream.h"
#include "Anisot.h"

struct gtaRwRasterLevel
{
	gtaRwUInt32 size;
	gtaRwUInt8 *pixels;

	gtaRwRasterLevel();

	void Destroy();
};

struct gtaRwTextureNative
{
	gtaRwUInt32 platformId;
	gtaRwUInt32 filterMode : 8;
	gtaRwUInt32 uAddressing : 4;
	gtaRwUInt32 vAddressing : 4;
	gtaRwChar name[32];
	gtaRwChar maskName[32];
	gtaRwUInt32 rasterFormat;
	union
	{
		gtaRwUInt32 d3dFormat; // SA
		gtaRwBool hasAlpha; // GTA 3 & VC
	};
	gtaRwInt16 width;
	gtaRwInt16 height;
	gtaRwUInt8 depth;
	gtaRwUInt8 numLevels;
	gtaRwUInt8 rasterType;
	union {
		gtaRwUInt8 compression; // GTA 3 & VC
		struct { // SA
			gtaRwUInt8 alpha : 1;
			gtaRwUInt8 cubeTexture : 1;
			gtaRwUInt8 autoMipMaps : 1;
			gtaRwUInt8 compressed : 1;
		};
	};
	gtaRwUInt8 *palette;
	gtaRwRasterLevel *levels;

	struct{
		gtaRwTextureAnisot anisot;
	} Extension;

	gtaRwTextureNative();

	// 3 & VC
	void Initialise(gtaRwUInt32 PlatformId, gtaRwUInt32 FilterMode, gtaRwUInt32 UAddressing, gtaRwUInt32 VAddressing, gtaRwChar *Name,
		gtaRwChar *MaskName, gtaRwUInt32 RasterFormat, gtaRwBool HasAlpha, gtaRwInt16 Width, gtaRwInt16 Height, gtaRwUInt8 Depth, 
		gtaRwUInt8 NumLevels, gtaRwUInt8 RasterType, gtaRwUInt8 Compression);
	// SA
	void Initialise(gtaRwUInt32 PlatformId, gtaRwUInt32 FilterMode, gtaRwUInt32 UAddressing, gtaRwUInt32 VAddressing, gtaRwChar *Name,
		gtaRwChar *MaskName, gtaRwUInt32 RasterFormat, gtaRwUInt32 D3dFormat, gtaRwInt16 Width, gtaRwInt16 Height, gtaRwUInt8 Depth, 
		gtaRwUInt8 NumLevels, gtaRwUInt8 RasterType, gtaRwBool HasAlpha, gtaRwBool IsCubeTexture, gtaRwBool UseAutoMipMaps, gtaRwBool IsCompressed);

	void Destroy();

	gtaRwBool StreamRead(gtaRwStream *Stream);

	gtaRwBool StreamReadD3D9(gtaRwStream *Stream);

	gtaRwBool StreamReadD3D8(gtaRwStream *Stream);

	gtaRwBool StreamWriteD3D8(gtaRwStream *Stream);

	gtaRwBool StreamWriteD3D9(gtaRwStream *Stream);

	gtaRwBool StreamWrite(gtaRwStream *Stream);

	gtaRwUInt32 GetStreamSize();
};