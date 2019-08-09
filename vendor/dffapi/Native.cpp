#pragma once

#include "Native.h"
#include "StdInc.h"
#include "Memory.h"

gtaRwGeometryNative::gtaNativeOGl::gtaNativeOGl()
{
	gtaMemZero(this, sizeof(gtaNativeOGl));
}

void gtaRwGeometryNative::gtaNativeOGl::Initialise(gtaRwUInt32 NumVertexElements, gtaRwUInt32 NumVertices, gtaRwUInt32 VertexStride)
{
	memset(this, 0, sizeof(gtaRwGeometryNative::gtaNativeOGl));
	generated = true;
	numVertexElements = NumVertexElements;
	if(numVertexElements > 0)
		vertexElements = (gtaNativeOGlVertexElement *)gtaMemAlloc(sizeof(gtaNativeOGlVertexElement) * numVertexElements);
	if(NumVertices > 0 && VertexStride > 0)
		vertexBuffer = gtaMemAlloc(NumVertices * VertexStride);
}

void gtaRwGeometryNative::gtaNativeOGl::Destroy()
{
	if(vertexElements)
		gtaMemFree(vertexElements);
	if(vertexBuffer)
		gtaMemFree(vertexBuffer);
	gtaMemZero(this, sizeof(gtaNativeOGl));
}

gtaRwUInt32 gtaRwGeometryNative::gtaNativeOGl::GetStreamSize(gtaRwUInt32 NumVertices)
{
	if(generated)
	{
		if(numVertexElements > 0)
			return numVertexElements * 24 + vertexElements[numVertexElements - 1].stride * NumVertices + 16;
	}
	return 0;
}

gtaRwBool gtaRwGeometryNative::gtaNativeOGl::StreamWrite(gtaRwStream *Stream, gtaRwUInt32 NumVertices)
{
	if(generated)
	{
		gtaRwUInt32 stride = 0;
		if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_NATIVEDATA, GetStreamSize(NumVertices) - 12, gtaRwVersion, gtaRwBuild))
			return false;
		if(!gtaRwStreamWrite(Stream, &numVertexElements, 4))
			return false;
		for(gtaRwInt32 i = 0; i < numVertexElements; i++)
		{
			if(!gtaRwStreamWrite(Stream, &vertexElements[i].paramType, 24))
				return false;
			stride = vertexElements[i].stride;
		}
		if(vertexBuffer && stride > 0)
		{
			if(!gtaRwStreamWrite(Stream, vertexBuffer, NumVertices * stride))
				return false;
		}
	}
	return true;
}

gtaRwBool gtaRwGeometryNative::gtaNativeOGl::StreamRead(gtaRwStream *Stream, gtaRwUInt32 NumVertices)
{
	memset(this, 0, sizeof(gtaRwGeometryNative::gtaNativeOGl));
	generated = true;
	gtaRwUInt32 stride = 0;
	if(gtaRwStreamRead(Stream, &numVertexElements, 4) != 4)
		return false;
	if(numVertexElements > 0)
	{
		vertexElements = (gtaNativeOGlVertexElement *)gtaMemAlloc(sizeof(gtaNativeOGlVertexElement) * numVertexElements);
		for(gtaRwInt32 i = 0; i < numVertexElements; i++)
		{
			if(gtaRwStreamRead(Stream, &vertexElements[i].paramType, 24) != 24)
			{
				Destroy();
				return false;
			}
			stride = vertexElements[i].stride;
		}
		if(stride > 0)
		{
			vertexBuffer = gtaMemAlloc(NumVertices * stride);
			if(gtaRwStreamRead(Stream, vertexBuffer, NumVertices * stride) != NumVertices * stride)
			{
				Destroy();
				return false;
			}
		}
	}
	return true;
}

gtaRwGeometryNative::gtaRwGeometryNative()
{
	gtaMemZero(this, sizeof(gtaRwGeometryNative));
}

gtaRwBool gtaRwGeometryNative::StreamWrite(gtaRwStream *Stream, gtaRwUInt32 NumVertices)
{
	if(enabled)
	{
		switch(gtaPlatform)
		{
		case PLATFORM_OGL:
			if(!ogl.StreamWrite(Stream, NumVertices))
			{
				return false;
			}
		}
	}
	return true;
}

gtaRwBool gtaRwGeometryNative::StreamRead(gtaRwStream *Stream, gtaRwUInt32 NumVertices)
{
	memset(this, 0, sizeof(gtaRwGeometryNative));
	{
		if(gtaIsMobile)
		{
			enabled = true;
			if(!ogl.StreamRead(Stream, NumVertices))
			{
				Destroy();
				return false;
			}
		}
		else
		{
			gtaRwUInt32 platform;
			if(!gtaRwStreamFindChunk(Stream, rwID_STRUCT, NULL, NULL))
				return false;
			if(gtaRwStreamRead(Stream, &platform, 4) != 4)
				return false;
			// Now read specific native data
			// TODO
		}
	}
	return true;
}

gtaRwUInt32 gtaRwGeometryNative::GetStreamSize(gtaRwUInt32 NumVertices)
{
	if(enabled)
	{
		switch(gtaPlatform)
		{
		case PLATFORM_OGL:
			return ogl.GetStreamSize(NumVertices);
		}
	}
	return 0;
}

void gtaRwGeometryNative::Initialise()
{
	memset(this, 0, sizeof(gtaRwGeometryNative));
	enabled = true;
}

void gtaRwGeometryNative::Destroy()
{
	ogl.Destroy();
	memset(this, 0, sizeof(gtaRwGeometryNative));
}