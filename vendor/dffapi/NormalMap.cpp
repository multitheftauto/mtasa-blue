#pragma once

#include "NormalMap.h"
#include "StdInc.h"

gtaRwMaterialNormalMap::gtaRwMaterialNormalMap()
{
	memset(this, 0, sizeof(gtaRwMaterialNormalMap));
}

bool gtaRwMaterialNormalMap::StreamWrite(gtaRwStream *stream)
{
	if(enabled)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_NORMALMAP, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
			return false;
		if(!gtaRwStreamWrite(stream, &flags, 4))
			return false;
		if(normalMapUsed)
		{
			if(!normalMapTexture.StreamWrite(stream))
				return false;
			if(envMapUsed)
			{
				if(!gtaRwStreamWrite(stream, &envMapCoefficient, 4))
					return false;
				if(!envMapTexture.StreamWrite(stream))
					return false;
			}
		}
	}
	return true;
}

bool gtaRwMaterialNormalMap::StreamRead(gtaRwStream *stream)
{
	memset(this, 0, sizeof(gtaRwMaterialNormalMap));
	enabled = true;
	if(gtaRwStreamRead(stream, &flags, 4) != 4)
		return false;
	if(normalMapUsed)
	{
		if(!normalMapTexture.StreamRead(stream))
			return false;
		if(envMapUsed)
		{
			if(gtaRwStreamRead(stream, &envMapCoefficient, 4) != 4)
				return false;
			if(!envMapTexture.StreamRead(stream))
				return false;
		}
	}
	return true;
}

unsigned int gtaRwMaterialNormalMap::GetStreamSize()
{
	if(enabled)
	{
		unsigned int size = 16;
		if(normalMapUsed)
		{
			size += normalMapTexture.GetStreamSize();
			if(envMapUsed)
				size += envMapTexture.GetStreamSize() + 4;
		}
		return size;
	}
	return 0;
}

void gtaRwMaterialNormalMap::Initialise(gtaRwBool NormalMapUsed, gtaRwBool EnvMapUsed, gtaRwReal EnvMapCoefficient, gtaRwBool EnvMapModulate)
{
	memset(this, 0, sizeof(gtaRwMaterialNormalMap));
	enabled = true;
	normalMapUsed = NormalMapUsed;
	envMapUsed = EnvMapUsed;
	envMapCoefficient = EnvMapCoefficient;
	envMapModulate = EnvMapModulate;
}

void gtaRwMaterialNormalMap::Destroy()
{
	memset(this, 0, sizeof(gtaRwMaterialNormalMap));
}