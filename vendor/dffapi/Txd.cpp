#pragma once

#include "Txd.h"
#include "Memory.h"

void gtaRwTexDictionary::Initialise(gtaRwUInt16 NumTextures)
{
	memset(this, 0, sizeof(gtaRwTexDictionary));
	_zero = 0;
	deviceId = 2;
	numTextures = NumTextures;
	if(numTextures > 0)
		textures = (gtaRwTextureNative *)gtaMemAlloc(sizeof(gtaRwTextureNative) * NumTextures);
}

void gtaRwTexDictionary::Destroy()
{
	if(textures)
	{
		for(gtaRwInt32 i = 0; i < numTextures; i++)
			textures[i].Destroy();
		free(textures);
	}
}

gtaRwTexDictionary::gtaRwTexDictionary()
{
	memset(this, 0, sizeof(gtaRwTexDictionary));
}

gtaRwBool gtaRwTexDictionary::StreamRead(gtaRwStream *stream)
{
	memset(this, 0, sizeof(gtaRwTexDictionary));
	if(!gtaRwStreamFindChunk(stream, rwID_TEXDICTIONARY, NULL, &version, &build))
		return false;
	if(!gtaRwStreamFindChunk(stream, rwID_STRUCT, NULL, NULL))
		return false;
	if(gtaRwStreamRead(stream, &numTextures, 4) != 4)
		return false;
	if(numTextures > 0)
	{
		textures = (gtaRwTextureNative *)gtaMemAlloc(sizeof(gtaRwTextureNative) * numTextures);
		for(gtaRwInt32 i = 0; i < numTextures; i++)
		{
			if(!textures[i].StreamRead(stream))
			{
				Destroy();
				return false;
			}
		}
	}
	//if(!gtaRwStreamFindChunk(stream, rwID_EXTENSION, NULL, NULL))
	//	return false;
	return true;
}

gtaRwBool gtaRwTexDictionary::StreamWrite(gtaRwStream *stream)
{
	if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_TEXDICTIONARY, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
		return false;
	if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRUCT, 4, gtaRwVersion, gtaRwBuild))
		return false;
	if(!gtaRwStreamWrite(stream, &numTextures, 4))
		return false;
	for(gtaRwInt32 i = 0; i < numTextures; i++)
	{
		if(!textures[i].StreamWrite(stream))
			return false;
	}
	if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_EXTENSION, 0, gtaRwVersion, gtaRwBuild))
		return false;
	return true;
}

gtaRwUInt32 gtaRwTexDictionary::GetStreamSize()
{
	gtaRwUInt32 size = 40;
	for(gtaRwInt32 i = 0; i < numTextures; i++)
		size += textures[i].GetStreamSize();
	return size;
}