#pragma once

#include "ExtraVertColour.h"
#include "StdInc.h"

gtaGeometryExtraVertColour::gtaGeometryExtraVertColour()
{
	memset(this, 0, sizeof(gtaGeometryExtraVertColour));
}

void gtaGeometryExtraVertColour::Initialise(gtaRwUInt32 VertexCount)
{
	memset(this, 0, sizeof(gtaGeometryExtraVertColour));
	enabled = true;
	nightColors = (gtaRwRGBA *)malloc(4 * VertexCount);
	memset(nightColors, 0, 4 * VertexCount);
}

void gtaGeometryExtraVertColour::Destroy()
{
	if(nightColors)
		free(nightColors);
	memset(this, 0, sizeof(gtaGeometryExtraVertColour));
}

bool gtaGeometryExtraVertColour::StreamWrite(gtaRwStream *stream, gtaRwUInt32 vertexCount)
{
	if(enabled)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, gtaID_EXTRAVERTCOLOUR, GetStreamSize(vertexCount) - 12, gtaRwVersion, gtaRwBuild))
			return false;
		if(!gtaRwStreamWrite(stream, &nightColors, 4))
			return false;
		if(nightColors && vertexCount > 0)
		{
			if(!gtaRwStreamWrite(stream, nightColors, 4 * vertexCount))
				return false;
		}
	}
	return true;
}

bool gtaGeometryExtraVertColour::StreamRead(gtaRwStream *stream, gtaRwUInt32 vertexCount)
{
	memset(this, 0, sizeof(gtaGeometryExtraVertColour));
	enabled = true;
	if(gtaRwStreamRead(stream, &nightColors, 4) != 4)
		return false;
	if(nightColors && vertexCount > 0)
	{
		nightColors = (gtaRwRGBA *)malloc(4 * vertexCount);
		memset(nightColors, 0, 4 * vertexCount);
		if(gtaRwStreamRead(stream, nightColors, 4 * vertexCount) != 4 * vertexCount)
		{
			Destroy();
			return false;
		}
	}
	return true;
}

unsigned int gtaGeometryExtraVertColour::GetStreamSize(gtaRwUInt32 vertexCount)
{
	if(enabled)
	{
		if(nightColors)
			return 16 + vertexCount * 4;
		return 16;
	}
	return 0;
}