#pragma once

#include "EnvMap.h"
#include "StdInc.h"

gtaMaterialEnvMap::gtaMaterialEnvMap()
{
	memset(this, 0, sizeof(gtaMaterialEnvMap));
}

bool gtaMaterialEnvMap::StreamWrite(gtaRwStream *stream)
{
	if(enabled)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, gtaID_ENVMAP, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
			return false;
		if(!gtaRwStreamWrite(stream, &scaleX, 24))
			return false;
	}
	return true;
}

bool gtaMaterialEnvMap::StreamRead(gtaRwStream *stream)
{
	memset(this, 0, sizeof(gtaMaterialEnvMap));
	enabled = true;
	if(gtaRwStreamRead(stream, &scaleX, 24) != 24)
		return false;
	return true;
}

unsigned int gtaMaterialEnvMap::GetStreamSize()
{
	if(enabled)
		return 36;
	return 0;
}

void gtaMaterialEnvMap::Initialise(gtaRwReal ScaleX, gtaRwReal ScaleY, gtaRwReal TransSclX, gtaRwReal TransSclY, gtaRwReal Shininess)
{
	memset(this, 0, sizeof(gtaMaterialEnvMap));
	enabled = true;
	scaleX = ScaleX;
	scaleY = ScaleY;
	transSclX = TransSclX;
	transSclY = TransSclY;
	shininess = Shininess;
}

void gtaMaterialEnvMap::Destroy()
{
	memset(this, 0, sizeof(gtaMaterialEnvMap));
}