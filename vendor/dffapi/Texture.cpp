#pragma once

#include "Texture.h"
#include "StdInc.h"

gtaRwTexture::gtaRwTexture()
{
	memset(this, 0, sizeof(gtaRwTexture));
}

bool gtaRwTexture::StreamRead(gtaRwStream *stream)
{
	gtaRwUInt32 length, entryLength, type;
	memset(this, 0, sizeof(gtaRwTexture));
	if(!gtaRwStreamFindChunk(stream, rwID_TEXTURE, NULL, NULL))
		return false;
	if(!gtaRwStreamFindChunk(stream, rwID_STRUCT, NULL, NULL))
		return false;
	if(!gtaRwStreamRead(stream, this, 4))
		return false;
	if(!name.StreamRead(stream))
		return false;
	if(!maskName.StreamRead(stream))
	{
		Destroy();
		return false;
	}
	if(!gtaRwStreamFindChunk(stream, rwID_EXTENSION, &length, NULL))
	{
		Destroy();
		return false;
	}
	while(length && gtaRwStreamReadChunkHeader(stream, &type, &entryLength, NULL, NULL))
	{
		switch(type)
		{
		case rwID_ANISOT:
			if(!Extension.anisot.StreamRead(stream))
			{
				Destroy();
				return false;
			}
			break;
		case rwID_SKYMIPMAP:
			if(!Extension.skyMipmap.StreamRead(stream))
			{
				Destroy();
				return false;
			}
			break;
		default:
			if(!gtaRwStreamSkip(stream, entryLength))
			{
				Destroy();
				return false;
			}
		}
		length += -12 - entryLength;
	}
	return true;
}

bool gtaRwTexture::StreamWrite(gtaRwStream *stream)
{
	if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_TEXTURE, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
		return false;
	if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRUCT, 4, gtaRwVersion, gtaRwBuild))
		return false;
	if(!gtaRwStreamWrite(stream, this, 4))
		return false;
	if(!name.StreamWrite(stream))
		return false;
	if(!maskName.StreamWrite(stream))
		return false;
	if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_EXTENSION, Extension.anisot.GetStreamSize(), gtaRwVersion, gtaRwBuild))
		return false;
	if(!Extension.skyMipmap.StreamWrite(stream))
		return false;
	if(!Extension.anisot.StreamWrite(stream))
		return false;
	return true;
}

unsigned int gtaRwTexture::GetStreamSize()
{
	return 40 + name.GetStreamSize() + maskName.GetStreamSize() + Extension.skyMipmap.GetStreamSize() + Extension.anisot.GetStreamSize();
}

void gtaRwTexture::Initialise(gtaRwTextureFilterMode Filtering, gtaRwTextureAddressMode UAddressing, gtaRwTextureAddressMode VAddressing,
	gtaRwBool AutoMipMapping, gtaRwChar *Name, gtaRwChar *MaskName)
{
	memset(this, 0, sizeof(gtaRwTexture));
	filtering = Filtering;
	uAddressing = UAddressing;
	vAddressing = VAddressing;
	name.Initialise(Name);
	maskName.Initialise(MaskName);
}

void gtaRwTexture::Destroy()
{
	name.Destroy();
	maskName.Destroy();
	Extension.anisot.Destroy();
	Extension.skyMipmap.Destroy();
	memset(this, 0, sizeof(gtaRwTexture));
}