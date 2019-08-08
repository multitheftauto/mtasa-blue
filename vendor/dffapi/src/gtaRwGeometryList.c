#include "StdInc.h"

void gtaRwGeometryListInitialise(gtaRwGeometryList* geometryList, gtaRwInt32 GeometryCount)
{
    memset(geometryList, 0, sizeof(gtaRwGeometryList));
    geometryList->geometryCount = GeometryCount;
    geometryList->geometries = (gtaRwGeometry*)malloc(sizeof(gtaRwGeometry) * GeometryCount);
    memset(geometryList->geometries, 0, sizeof(gtaRwGeometry) * GeometryCount);
}

void gtaRwGeometryListDestroy(gtaRwGeometryList* geometryList)
{
    if (geometryList->geometries)
    {
        for (int i = 0; i < geometryList->geometryCount; i++)
            gtaRwGeometryDestroy(&geometryList->geometries[i]);
        free(geometryList->geometries);
    }
}

gtaRwBool gtaRwGeometryListStreamRead(gtaRwGeometryList* geometryList, gtaRwStream* stream, gtaRwUInt32 ClumpVersion)
{
    memset(geometryList, 0, sizeof(gtaRwGeometryList));
    if (!gtaRwStreamFindChunk(stream, rwID_GEOMETRYLIST, rwNULL, rwNULL, rwNULL))
        return rwFALSE;
    if (!gtaRwStreamFindChunk(stream, rwID_STRUCT, rwNULL, rwNULL, rwNULL))
        return rwFALSE;
    if (gtaRwStreamRead(stream, &(geometryList->geometryCount), 4) != 4)
        return rwFALSE;
    if (geometryList->geometryCount > 0)
    {
        geometryList->geometries = (gtaRwGeometry*)malloc(sizeof(gtaRwGeometry) * geometryList->geometryCount);
        memset(geometryList->geometries, 0, sizeof(gtaRwGeometry) * geometryList->geometryCount);
        for (gtaRwInt8 i = 0; i < geometryList->geometryCount; i++)
        {
            if (!gtaRwGeometryStreamRead(&geometryList->geometries[i], stream, ClumpVersion))
            {
                gtaRwGeometryListDestroy(geometryList);
                return rwFALSE;
            }
        }
    }
    return rwTRUE;
}

gtaRwBool gtaRwGeometryListStreamWrite(gtaRwGeometryList* geometryList, gtaRwStream* stream)
{
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_GEOMETRYLIST, gtaRwGeometryListGetStreamSize(geometryList) - 12, gtaRwVersion, gtaRwBuild))
    {
        return rwFALSE;
	}
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRUCT, 4, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (!gtaRwStreamWrite(stream, &geometryList->geometryCount, 4))
        return rwFALSE;
    for (gtaRwInt8 i = 0; i < geometryList->geometryCount; i++)
    {
        if (!gtaRwGeometryStreamWrite(&geometryList->geometries[i], stream))
            return rwFALSE;
    }
    return rwTRUE;
}

gtaRwUInt32 gtaRwGeometryListGetStreamSize(gtaRwGeometryList* geometryList)
{
    gtaRwUInt32 size = 28;
    for (gtaRwInt8 i = 0; i < geometryList->geometryCount; i++)
        size += gtaRwGeometryGetStreamSize(&geometryList->geometries[i]);
    return size;
}