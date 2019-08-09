#pragma once

#include "GeometryList.h"

void gtaRwGeometryList::Initialise(gtaRwInt32 GeometryCount)
{
	memset(this, 0, sizeof(gtaRwGeometryList));
	geometryCount = GeometryCount;
	geometries = (gtaRwGeometry *)malloc(sizeof(gtaRwGeometry) * GeometryCount);
	memset(geometries, 0, sizeof(gtaRwGeometry) * GeometryCount);
}

void gtaRwGeometryList::Destroy()
{
	if(geometries)
	{
		for(int i = 0; i < geometryCount; i++)
			geometries[i].Destroy();
		free(geometries);
	}
}

gtaRwGeometryList::gtaRwGeometryList()
{
	memset(this, 0, sizeof(gtaRwGeometryList));
}

gtaRwBool gtaRwGeometryList::StreamRead(gtaRwStream *stream, gtaRwUInt32 ClumpVersion)
{
	memset(this, 0, sizeof(gtaRwGeometryList));
	if(!gtaRwStreamFindChunk(stream, rwID_GEOMETRYLIST, NULL, NULL))
		return false;
	if(!gtaRwStreamFindChunk(stream, rwID_STRUCT, NULL, NULL))
		return false;
	if(gtaRwStreamRead(stream, &geometryCount, 4) != 4)
		return false;
	if(geometryCount > 0)
	{
		geometries = (gtaRwGeometry *)malloc(sizeof(gtaRwGeometry) * geometryCount);
		memset(geometries, 0, sizeof(gtaRwGeometry) * geometryCount);
		for(gtaRwInt8 i = 0; i < geometryCount; i++)
		{
			if(!geometries[i].StreamRead(stream, ClumpVersion))
			{
				Destroy();
				return false;
			}
		}
	}
	return true;
}

gtaRwBool gtaRwGeometryList::StreamWrite(gtaRwStream *stream)
{
	if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_GEOMETRYLIST, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
		return false;
	if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRUCT, 4, gtaRwVersion, gtaRwBuild))
		return false;
	if(!gtaRwStreamWrite(stream, &geometryCount, 4))
		return false;
	for(gtaRwInt8 i = 0; i < geometryCount; i++)
	{
		if(!geometries[i].StreamWrite(stream))
			return false;
	}
	return true;
}

gtaRwUInt32 gtaRwGeometryList::GetStreamSize()
{
	gtaRwUInt32 size = 28;
	for(gtaRwInt8 i = 0; i < geometryCount; i++)
		size += geometries[i].GetStreamSize();
	return size;
}