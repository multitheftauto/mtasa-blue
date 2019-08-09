#pragma once

#include "Material.h"

gtaRwMaterial::gtaRwMaterial()
{
	memset(this, 0, sizeof(gtaRwMaterial));
}

bool gtaRwMaterial::StreamRead(gtaRwStream *stream)
{
	gtaRwUInt32 length, entryLength, type;
	memset(this, 0, sizeof(gtaRwMaterial));
	if(!gtaRwStreamFindChunk(stream, rwID_MATERIAL, NULL, NULL))
		return false;
	if(!gtaRwStreamFindChunk(stream, rwID_STRUCT, NULL, NULL))
		return false;
	if(gtaRwStreamRead(stream, &flags, 28) != 28)
		return false;
	if(textured)
	{
		if(!texture.StreamRead(stream))
		{
			Destroy();
			return false;
		}
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
		case rwID_UVANIM:
			if(!Extension.uvAnim.StreamRead(stream))
			{
				Destroy();
				return false;
			}
			break;
		case rwID_MATFX:
			if(!Extension.matFx.StreamRead(stream))
			{
				Destroy();
				return false;
			}
			break;
		case rwID_NORMALMAP:
			if(!Extension.normalMap.StreamRead(stream))
			{
				Destroy();
				return false;
			}
			break;
		case gtaID_ENVMAP:
			if(!Extension.envMap.StreamRead(stream))
			{
				Destroy();
				return false;
			}
			break;
		case gtaID_SPECMAP:
			if(!Extension.specMap.StreamRead(stream))
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

bool gtaRwMaterial::StreamWrite(gtaRwStream *stream)
{
	if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_MATERIAL, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
		return false;
	if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRUCT, 28, gtaRwVersion, gtaRwBuild))
		return false;
	if(!gtaRwStreamWrite(stream, &flags, 28))
		return false;
	if(textured)
	{
		if(!texture.StreamWrite(stream))
			return false;
	}
	if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_EXTENSION, Extension.uvAnim.GetStreamSize() + Extension.matFx.GetStreamSize() +
		Extension.normalMap.GetStreamSize() + Extension.envMap.GetStreamSize() + Extension.specMap.GetStreamSize(), gtaRwVersion, gtaRwBuild))
		return false;
	if(!Extension.matFx.StreamWrite(stream))
		return false;
	if(!Extension.uvAnim.StreamWrite(stream))
		return false;
	if(!Extension.envMap.StreamWrite(stream))
		return false;
	if(!Extension.specMap.StreamWrite(stream))
		return false;
	if(!Extension.normalMap.StreamWrite(stream))
		return false;
	return true;
}

unsigned int gtaRwMaterial::GetStreamSize()
{
	unsigned int size = 64 + Extension.envMap.GetStreamSize() + Extension.matFx.GetStreamSize() + Extension.normalMap.GetStreamSize() +
		Extension.specMap.GetStreamSize() + Extension.uvAnim.GetStreamSize();
	if(textured)
		size += texture.GetStreamSize();
	return size;
}

void gtaRwMaterial::Initialise(gtaRwUInt8 Red, gtaRwUInt8 Green, gtaRwUInt8 Blue, gtaRwUInt8 Alpha, gtaRwBool Textured, gtaRwReal Ambient, gtaRwReal Diffuse)
{
	memset(this, 0, sizeof(gtaRwMaterial));
	color.r = Red;
	color.g = Green;
	color.b = Blue;
	color.a = Alpha;
	textured = Textured;
	surfaceProps.ambient = Ambient;
	surfaceProps.diffuse = Diffuse;
	surfaceProps.specular = 1.0f;
}

void gtaRwMaterial::Destroy()
{
	texture.Destroy();
	Extension.uvAnim.Destroy();
	Extension.envMap.Destroy();
	Extension.matFx.Destroy();
	Extension.normalMap.Destroy();
	Extension.specMap.Destroy();
	memset(this, 0, sizeof(gtaRwMaterial));
}