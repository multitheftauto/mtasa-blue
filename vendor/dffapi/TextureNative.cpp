#pragma once

#include "TextureNative.h"
#include "Memory.h"
#include "StdInc.h"

gtaRwRasterLevel::gtaRwRasterLevel()
{
	gtaMemZero(this, sizeof(gtaRwRasterLevel));
}

void gtaRwRasterLevel::Destroy()
{
	if(pixels)
		gtaMemFree(pixels);
	gtaMemZero(this, sizeof(gtaRwRasterLevel));
}

gtaRwTextureNative::gtaRwTextureNative()
{
	gtaMemZero(this, sizeof(gtaRwTextureNative));
}

// 3 & VC
void gtaRwTextureNative::Initialise(gtaRwUInt32 PlatformId, gtaRwUInt32 FilterMode, gtaRwUInt32 UAddressing, gtaRwUInt32 VAddressing, gtaRwChar *Name,
	gtaRwChar *MaskName, gtaRwUInt32 RasterFormat, gtaRwBool HasAlpha, gtaRwInt16 Width, gtaRwInt16 Height, gtaRwUInt8 Depth, 
	gtaRwUInt8 NumLevels, gtaRwUInt8 RasterType, gtaRwUInt8 Compression)
{
	memset(this, 0, sizeof(gtaRwTextureNative));
	platformId = PlatformId;
	filterMode = FilterMode;
	uAddressing = UAddressing;
	vAddressing = VAddressing;
	if(Name)
		strcpy(name, Name);
	if(MaskName)
		strcpy(maskName, MaskName);
	rasterFormat = RasterFormat;
	hasAlpha = HasAlpha;
	width = Width;
	height = Height;
	depth = Depth;
	numLevels = NumLevels;
	if(numLevels)
		levels = (gtaRwRasterLevel *)gtaMemAlloc(NumLevels * sizeof(gtaRwRasterLevel));
	rasterType = RasterType;
	compression = Compression;
}
// SA
void gtaRwTextureNative::Initialise(gtaRwUInt32 PlatformId, gtaRwUInt32 FilterMode, gtaRwUInt32 UAddressing, gtaRwUInt32 VAddressing, gtaRwChar *Name,
	gtaRwChar *MaskName, gtaRwUInt32 RasterFormat, gtaRwUInt32 D3dFormat, gtaRwInt16 Width, gtaRwInt16 Height, gtaRwUInt8 Depth, 
	gtaRwUInt8 NumLevels, gtaRwUInt8 RasterType, gtaRwBool HasAlpha, gtaRwBool IsCubeTexture, gtaRwBool UseAutoMipMaps, gtaRwBool IsCompressed)
{
	memset(this, 0, sizeof(gtaRwTextureNative));
	platformId = PlatformId;
	filterMode = FilterMode;
	uAddressing = UAddressing;
	vAddressing = VAddressing;
	if(Name)
		strcpy(name, Name);
	if(MaskName)
		strcpy(maskName, MaskName);
	rasterFormat = RasterFormat;
	d3dFormat = D3dFormat;
	width = Width;
	height = Height;
	depth = Depth;
	numLevels = NumLevels;
	gtaRwUInt32 totalLevels = NumLevels * (IsCubeTexture? 6 : 1);
	if(numLevels)
		levels = (gtaRwRasterLevel *)gtaMemAlloc(totalLevels * sizeof(gtaRwRasterLevel));
	rasterType = RasterType;
	alpha = HasAlpha;
	cubeTexture = IsCubeTexture;
	autoMipMaps = UseAutoMipMaps;
	compressed = IsCompressed;
}

void gtaRwTextureNative::Destroy()
{
	if(palette)
		gtaMemFree(palette);
	if(levels)
	{
		for(int i = 0; i < numLevels; i++)
			levels[i].Destroy();
		gtaMemFree(levels);
	}
	Extension.anisot.Destroy();
	gtaMemZero(this, sizeof(gtaRwTextureNative));
}

gtaRwBool gtaRwTextureNative::StreamRead(gtaRwStream *Stream)
{
	memset(this, 0, sizeof(gtaRwTextureNative));
	if(!gtaRwStreamFindChunk(Stream, rwID_TEXTURENATIVE, NULL, NULL))
		return false;
	if(!gtaRwStreamFindChunk(Stream, rwID_STRUCT, NULL, NULL))
		return false;
	if(gtaRwStreamRead(Stream, &platformId, 88) != 88)
		return false;
	switch(platformId)
	{
	case rwID_PCD3D9:
		if(!StreamReadD3D9(Stream))
			return false;
		break;
	case rwID_PCD3D8:
		if(!StreamReadD3D8(Stream))
			return false;
		break;
	default:
		gtaRwErrorSet("Reding Native Texture: This platform Id (%d) is not supported", platformId);
		return false;
	}

	gtaRwUInt32 length, type, entryLength;
	if(!gtaRwStreamFindChunk(Stream, rwID_EXTENSION, &length, NULL))
	{
		Destroy();
		return false;
	}
	while(length && gtaRwStreamReadChunkHeader(Stream, &type, &entryLength, NULL, NULL))
	{
		switch(type)
		{
		case rwID_ANISOT:
			if(!Extension.anisot.StreamRead(Stream))
			{
				Destroy();
				return false;
			}
			break;
		default:
			if(!gtaRwStreamSkip(Stream, entryLength))
			{
				Destroy();
				return false;
			}
		}
		length += -12 - entryLength;
	}

	return true;
}

gtaRwBool gtaRwTextureNative::StreamReadD3D9(gtaRwStream *Stream)
{
	// read palette
	if(rasterFormat & rwRASTERFORMATPAL4)
	{
		palette = (gtaRwUInt8 *)gtaMemAlloc(128);
		if(gtaRwStreamRead(Stream, palette, 128) != 128)
		{
			Destroy();
			return false;
		}
	}
	else if(rasterFormat & rwRASTERFORMATPAL8)
	{
		palette = (gtaRwUInt8 *)gtaMemAlloc(1024);
		if(gtaRwStreamRead(Stream, palette, 1024) != 1024)
		{
			Destroy();
			return false;
		}
	}
	// read levels
	int totalLevels = numLevels * (cubeTexture? 6 : 1);
	levels = (gtaRwRasterLevel *)gtaMemAlloc(totalLevels * sizeof(gtaRwRasterLevel));
	for(int i = 0; i < totalLevels; i++)
	{
		if(gtaRwStreamRead(Stream, &levels[i].size, 4) != 4)
		{
			Destroy();
			return false;
		}
		levels[i].pixels = (gtaRwUInt8 *)gtaMemAlloc(levels[i].size);
		if(gtaRwStreamRead(Stream, levels[i].pixels, levels[i].size) != levels[i].size)
		{
			Destroy();
				return false;
		}
	}
	return true;
}

gtaRwBool gtaRwTextureNative::StreamReadD3D8(gtaRwStream *Stream)
{
	// read palette
	if(rasterFormat & rwRASTERFORMATPAL4)
	{
		palette = (gtaRwUInt8 *)gtaMemAlloc(128);
		if(gtaRwStreamRead(Stream, palette, 128) != 128)
		{
			Destroy();
			return false;
		}
	}
	else if(rasterFormat & rwRASTERFORMATPAL8)
	{
		palette = (gtaRwUInt8 *)gtaMemAlloc(1024);
		if(gtaRwStreamRead(Stream, palette, 1024) != 1024)
		{
			Destroy();
			return false;
		}
	}
	// read levels
	levels = (gtaRwRasterLevel *)gtaMemAlloc(numLevels * sizeof(gtaRwRasterLevel));
	for(int i = 0; i < numLevels; i++)
	{
		if(gtaRwStreamRead(Stream, &levels[i].size, 4) != 4)
		{
			Destroy();
			return false;
		}
		levels[i].pixels = (gtaRwUInt8 *)gtaMemAlloc(levels[i].size);
		if(gtaRwStreamRead(Stream, levels[i].pixels, levels[i].size) != levels[i].size)
		{
			Destroy();
				return false;
		}
	}
	return true;
}

gtaRwBool gtaRwTextureNative::StreamWriteD3D8(gtaRwStream *Stream)
{
	for(int i = 0; i < numLevels; i++)
	{
		if(!gtaRwStreamWrite(Stream, &levels[i].size, 4))
		{
			Destroy();
			return false;
		}
		if(!gtaRwStreamWrite(Stream, levels[i].pixels, levels[i].size))
		{
			Destroy();
				return false;
		}
	}
	return true;
}

gtaRwBool gtaRwTextureNative::StreamWriteD3D9(gtaRwStream *Stream)
{
	int totalLevels = numLevels * (cubeTexture? 6 : 1);
	for(int i = 0; i < totalLevels; i++)
	{
		if(!gtaRwStreamWrite(Stream, &levels[i].size, 4))
		{
			Destroy();
			return false;
		}
		if(!gtaRwStreamWrite(Stream, levels[i].pixels, levels[i].size))
		{
			Destroy();
				return false;
		}
	}
	return true;
}

gtaRwBool gtaRwTextureNative::StreamWrite(gtaRwStream *Stream)
{
	if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_TEXTURENATIVE, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
		return false;
	if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_STRUCT, GetStreamSize() - 36, gtaRwVersion, gtaRwBuild))
		return false;
	if(!gtaRwStreamWrite(Stream, &platformId, 88))
		return false;
	if(rasterFormat & rwRASTERFORMATPAL4)
	{
		if(!gtaRwStreamWrite(Stream, palette, 128))
		{
			Destroy();
			return false;
		}
	}
	else if(rasterFormat & rwRASTERFORMATPAL8)
	{
		if(!gtaRwStreamWrite(Stream, palette, 1024))
		{
			Destroy();
			return false;
		}
	}
	switch(platformId)
	{
	case rwID_PCD3D9:
		if(!StreamWriteD3D9(Stream))
			return false;
		break;
	case rwID_PCD3D8:
		if(!StreamWriteD3D8(Stream))
			return false;
		break;
	default:
		gtaRwErrorSet("Writing Native Texture: This platform Id (%d) is not supported", platformId);
		return false;
	}
	if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_EXTENSION, Extension.anisot.GetStreamSize(), gtaRwVersion, gtaRwBuild))
		return false;
	if(!Extension.anisot.StreamWrite(Stream))
		return false;
	return true;
}

gtaRwUInt32 gtaRwTextureNative::GetStreamSize()
{
	gtaRwUInt32 size = 124; // 88 + (struct) 12 + (header) 12 + (extension) 12
	int totalLevels;
	if(rasterFormat & rwRASTERFORMATPAL4)
		size += 128;
	else if(rasterFormat & rwRASTERFORMATPAL8)
		size += 1024;
	size += Extension.anisot.GetStreamSize();
	switch(platformId)
	{
	case rwID_PCD3D9:
		totalLevels = numLevels * (cubeTexture? 6 : 1);
		for(gtaRwInt32 i = 0; i < totalLevels; i++)
			size += 4 + levels[i].size;
		break;
	case rwID_PCD3D8:
		for(gtaRwInt32 i = 0; i < numLevels; i++)
			size += 4 + levels[i].size;
		break;
	default:
		size = 0;
		gtaRwErrorSet("Native Texture Size: This platform Id (%d) is not supported", platformId);
	}
	return size;
}