#pragma once

#include "Anisot.h"
#include "StdInc.h"

gtaRwTextureAnisot::gtaRwTextureAnisot()
{
	memset(this, 0, sizeof(gtaRwTextureAnisot));
}

gtaRwBool gtaRwTextureAnisot::StreamWrite(gtaRwStream *Stream)
{
	if(enabled)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_ANISOT, 4, gtaRwVersion, gtaRwBuild))
			return false;
		if(!gtaRwStreamWrite(Stream, &anisotropyLevel, 4))
			return false;
	}
	return true;
}

gtaRwBool gtaRwTextureAnisot::StreamRead(gtaRwStream *Stream)
{
	memset(this, 0, sizeof(gtaRwTextureAnisot));
	enabled = true;
	if(gtaRwStreamRead(Stream, &anisotropyLevel, 4) != 4)
		return false;
	return true;
}

gtaRwUInt32 gtaRwTextureAnisot::GetStreamSize()
{
	if(enabled)
		return 16;
	return 0;
}

void gtaRwTextureAnisot::Initialise(gtaRwUInt32 AnisotropyLevel)
{
	memset(this, 0, sizeof(gtaRwTextureAnisot));
	enabled = true;
	anisotropyLevel = AnisotropyLevel;
}

void gtaRwTextureAnisot::Destroy()
{
	memset(this, 0, sizeof(gtaRwTextureAnisot));
}