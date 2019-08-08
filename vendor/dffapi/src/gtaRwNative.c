#include "StdInc.h"

void gtaNativeOGlInitialise(gtaNativeOGl* native, gtaRwUInt32 NumVertexElements, gtaRwUInt32 NumVertices, gtaRwUInt32 VertexStride)
{
    memset(native, 0, sizeof(gtaNativeOGl));
    native->generated = rwTRUE;
    native->numVertexElements = NumVertexElements;
    if (native->numVertexElements > 0)
        native->vertexElements = (gtaNativeOGlVertexElement*)gtaMemAlloc(sizeof(gtaNativeOGlVertexElement) * native->numVertexElements);
	if(NumVertices > 0 && VertexStride > 0)
        native->vertexBuffer = gtaMemAlloc(NumVertices * VertexStride);
}

void gtaNativeOGlDestroy(gtaNativeOGl* native)
{
    if (native->vertexElements)
        gtaMemFree(native->vertexElements);
    if (native->vertexBuffer)
        gtaMemFree(native->vertexBuffer);
	gtaMemZero(native, sizeof(gtaNativeOGl));
}

gtaRwUInt32 gtaNativeOGlGetStreamSize(gtaNativeOGl* native, gtaRwUInt32 NumVertices)
{
    if (native->generated)
	{
        if (native->numVertexElements > 0)
            return native->numVertexElements * 24 + native->vertexElements[native->numVertexElements - 1].stride * NumVertices + 16;
	}
	return 0;
}

gtaRwBool gtaNativeOGlStreamWrite(gtaNativeOGl* native, gtaRwStream* Stream, gtaRwUInt32 NumVertices)
{
    if (native->generated)
	{
		gtaRwUInt32 stride = 0;
		if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_NATIVEDATA, GetStreamSize(NumVertices) - 12, gtaRwVersion, gtaRwBuild))
			return rwFALSE;
        if (!gtaRwStreamWrite(Stream, &native->numVertexElements, 4))
            return rwFALSE;
        for (gtaRwInt32 i = 0; i < native->numVertexElements; i++)
		{
            if (!gtaRwStreamWrite(Stream, &native->vertexElements[i].paramType, 24))
                return rwFALSE;
            stride = native->vertexElements[i].stride;
		}
        if (native->vertexBuffer && stride > 0)
		{
            if (!gtaRwStreamWrite(Stream, native->vertexBuffer, NumVertices * stride))
                return rwFALSE;
		}
	}
	return rwTRUE;
}

gtaRwBool gtaNativeOGlStreamRead(gtaNativeOGl* native, gtaRwStream* Stream, gtaRwUInt32 NumVertices)
{
	memset(native, 0, sizeof(gtaNativeOGl));
    native->generated = rwTRUE;
	gtaRwUInt32 stride = 0;
    if (gtaRwStreamRead(Stream, &native->numVertexElements, 4) != 4)
		return rwFALSE;
    if (native->numVertexElements > 0)
	{
        native->vertexElements = (gtaNativeOGlVertexElement*)gtaMemAlloc(sizeof(gtaNativeOGlVertexElement) * native->numVertexElements);
        for (gtaRwInt32 i = 0; i < native->numVertexElements; i++)
		{
            if (gtaRwStreamRead(Stream, &native->vertexElements[i].paramType, 24) != 24)
			{
				Destroy();
                return rwFALSE;
			}
            stride = native->vertexElements[i].stride;
		}
		if(stride > 0)
		{
            native->vertexBuffer = gtaMemAlloc(NumVertices * stride);
            if (gtaRwStreamRead(Stream, native->vertexBuffer, NumVertices * stride) != NumVertices * stride)
			{
				Destroy();
                return rwFALSE;
			}
		}
	}
    return rwTRUE;
}
gtaRwBool gtaRwGeometryNativeStreamWrite(gtaRwGeometryNative* geometryNativeObj, gtaRwStream* Stream, gtaRwUInt32 NumVertices)
{
    if (geometryNativeObj->enabled)
	{
		switch(gtaPlatform)
		{
            case PLATFORM_OGL:
                if (!gtaNativeOGlStreamWrite(&geometryNativeObj->ogl, Stream, NumVertices))
                {
                    return rwFALSE;
                }
        }
	}
	return rwFALSE;
}

gtaRwBool gtaRwGeometryNativeStreamRead(gtaRwGeometryNative* geometryNativeObj, gtaRwStream* Stream, gtaRwUInt32 NumVertices)
{
    memset(geometryNativeObj, 0, sizeof(gtaRwGeometryNative));
	{
		if(gtaIsMobile)
		{
            geometryNativeObj->enabled = rwTRUE;
            if (!gtaNativeOGlStreamRead(&geometryNativeObj, Stream, NumVertices))
			{
				Destroy();
                return rwFALSE;
			}
		}
		else
		{
			gtaRwUInt32 platform;
			if(!gtaRwStreamFindChunk(Stream, rwID_STRUCT, NULL, NULL, NULL))
                return rwFALSE;
			if(gtaRwStreamRead(Stream, &platform, 4) != 4)
                return rwFALSE;
			// Now read specific native data
			// TODO
		}
	}
	return rwTRUE;
}

gtaRwUInt32 gtaRwGeometryNativeGetStreamSize(gtaRwGeometryNative* geometryNativeObj, gtaRwUInt32 NumVertices)
{
    if (geometryNativeObj->enabled)
	{
		switch(gtaPlatform)
		{
		case PLATFORM_OGL:
                return gtaNativeOGlGetStreamSize(geometryNativeObj, NumVertices);
		}
	}
	return 0;
}

void gtaRwGeometryNativeInitialise(gtaRwGeometryNative* geometryNativeObj)
{
    memset(geometryNativeObj, 0, sizeof(gtaRwGeometryNative));
    geometryNativeObj->enabled = rwTRUE;
}

void gtaRwGeometryNativeDestroy(gtaRwGeometryNative* geometryNativeObj)
{
    gtaNativeOGlDestroy(&geometryNativeObj);
    memset(geometryNativeObj, 0, sizeof(gtaRwGeometryNative));
}