#pragma once

#include "SpecMap.h"
#include "StdInc.h"

gtaMaterialSpecMap::gtaMaterialSpecMap()
{
	memset(this, 0, sizeof(gtaMaterialSpecMap));
}

bool gtaMaterialSpecMap::StreamWrite(gtaRwStream *stream)
{
	if(enabled)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, gtaID_SPECMAP, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
			return false;
		if(!gtaRwStreamWrite(stream, &specularity, 28))
			return false;
	}
	return true;
}

bool gtaMaterialSpecMap::StreamRead(gtaRwStream *stream)
{
	memset(this, 0, sizeof(gtaMaterialSpecMap));
	enabled = true;
	if(gtaRwStreamRead(stream, &specularity, 28) != 28)
		return false;
	return true;
}

unsigned int gtaMaterialSpecMap::GetStreamSize()
{
	if(enabled)
		return 40;
	return 0;
}

void gtaMaterialSpecMap::Initialise(gtaRwReal Specularity, gtaRwChar *TextureName)
{
	memset(this, 0, sizeof(gtaMaterialSpecMap));
	enabled = true;
	specularity = Specularity;
	memset(textureName, 0, 24);
	if(TextureName)
		strncpy(textureName, TextureName, 24);
}

void gtaMaterialSpecMap::Destroy()
{
	memset(this, 0, sizeof(gtaMaterialSpecMap));
}