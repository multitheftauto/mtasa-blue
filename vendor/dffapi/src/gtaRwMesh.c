#include "StdInc.h"

void gtaRwMeshInitialise(gtaRwMesh* meshObj, gtaRwUInt32 NumIndices, gtaRwUInt32 Material)
{
    memset(meshObj, 0, sizeof(gtaRwMesh));
    meshObj->numIndices = NumIndices;
    meshObj->material = Material;
	if(NumIndices > 0)
	{
        meshObj->indices = (gtaRwUInt32*)malloc(4 * NumIndices);
        memset(meshObj->indices, 0, 4 * NumIndices);
	}
}

void gtaRwMeshDestroy(gtaRwMesh* meshObj)
{
    if (meshObj->indices)
        free(meshObj->indices);
    memset(meshObj, 0, sizeof(gtaRwMesh));
}

void gtaRwGeometryBinMeshInitialise(gtaRwGeometryBinMesh* geometryBinMeshObj, gtaRwUInt32 Flags, gtaRwUInt32 NumMeshes, gtaRwUInt32 TotalIndicesInMesh)
{
    memset(geometryBinMeshObj, 0, sizeof(gtaRwGeometryBinMesh));
    geometryBinMeshObj->enabled = rwTRUE;
    geometryBinMeshObj->flags = Flags;
    geometryBinMeshObj->numMeshes = NumMeshes;
    geometryBinMeshObj->totalIndicesInMesh = TotalIndicesInMesh;
    if (geometryBinMeshObj->numMeshes > 0)
	{
        geometryBinMeshObj->meshes = (gtaRwMesh*)malloc(NumMeshes * sizeof(gtaRwMesh));
        memset(geometryBinMeshObj->meshes, 0, NumMeshes * sizeof(gtaRwMesh));
	}
}

void gtaRwGeometryBinMeshDestroy(gtaRwGeometryBinMesh* geometryBinMeshObj)
{
    if (geometryBinMeshObj->meshes)
	{
        for (int i = 0; i < geometryBinMeshObj->numMeshes; i++)
            gtaRwGeometryBinMeshDestroy(&geometryBinMeshObj->meshes[i]);
        free(geometryBinMeshObj->meshes);
	}
    memset(geometryBinMeshObj, 0, sizeof(gtaRwGeometryBinMesh));
}

gtaRwBool gtaRwGeometryBinMeshStreamWrite(gtaRwGeometryBinMesh* geometryBinMeshObj, gtaRwStream* stream, gtaRwBool IsNative)
{
    if (geometryBinMeshObj->enabled)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_BINMESH, GetStreamSize(IsNative) - 12, gtaRwVersion, gtaRwBuild))
			return rwFALSE;
        if (!gtaRwStreamWrite(stream, &geometryBinMeshObj->flags, 12))
			return rwFALSE;
		if(!IsNative || gtaPlatform == PLATFORM_OGL)
		{
            for (int i = 0; i < geometryBinMeshObj->numMeshes; i++)
			{
                if (!gtaRwStreamWrite(stream, &geometryBinMeshObj->meshes[i].numIndices, 8))
					return rwFALSE;
                if (geometryBinMeshObj->meshes[i].numIndices > 0)
				{
					if(IsNative)
					{
                        gtaRwUInt16* indices = (gtaRwUInt16*)malloc(2 * geometryBinMeshObj->meshes[i].numIndices);
                        memset(indices, 0, 2 * geometryBinMeshObj->meshes[i].numIndices);
                        for (gtaRwInt8 j = 0; j < geometryBinMeshObj->meshes[i].numIndices; j++)
                            indices[j] = geometryBinMeshObj->meshes[i].indices[j];
                        if (!gtaRwStreamWrite(stream, indices, 2 * geometryBinMeshObj->meshes[i].numIndices))
						{
							free(indices);
							return rwFALSE;
						}
						free(indices);
					}
					else
					{
                        if (!gtaRwStreamWrite(stream, geometryBinMeshObj->meshes[i].indices, 4 * geometryBinMeshObj->meshes[i].numIndices))
							return rwFALSE;
					}
				}
			}
		}
	}
	return rwTRUE;
}

gtaRwBool gtaRwGeometryBinMeshStreamRead(gtaRwGeometryBinMesh* geometryBinMeshObj, gtaRwStream* stream, gtaRwBool IsNative)
{
    memset(geometryBinMeshObj, 0, sizeof(gtaRwGeometryBinMesh));
    geometryBinMeshObj->enabled = rwTRUE;
    if (gtaRwStreamRead(stream, &geometryBinMeshObj->flags, 12) != 12)
		return rwFALSE;
    if (geometryBinMeshObj->numMeshes > 0)
	{
        geometryBinMeshObj->meshes = (gtaRwMesh*)malloc(geometryBinMeshObj->numMeshes * sizeof(gtaRwMesh));
        memset(geometryBinMeshObj->meshes, 0, geometryBinMeshObj->numMeshes * sizeof(gtaRwMesh));
        for (int i = 0; i < geometryBinMeshObj->numMeshes; i++)
		{
            if (gtaRwStreamRead(stream, &geometryBinMeshObj->meshes[i].numIndices, 8) != 8)
			{
                gtaRwGeometryBinMeshDestroy(geometryBinMeshObj);
				return rwFALSE;
			}
            if (!IsNative || gtaIsMobile)
			{
                if (geometryBinMeshObj->meshes[i].numIndices > 0)
				{
                    geometryBinMeshObj->meshes[i].indices = (gtaRwUInt32*)malloc(4 * geometryBinMeshObj->meshes[i].numIndices);
                    memset(geometryBinMeshObj->meshes[i].indices, 0, 4 * geometryBinMeshObj->meshes[i].numIndices);
					if(IsNative)
					{
                        gtaRwUInt16* indices = (gtaRwUInt16*)malloc(2 * geometryBinMeshObj->meshes[i].numIndices);
                        memset(indices, 0, 2 * geometryBinMeshObj->meshes[i].numIndices);
                        if (gtaRwStreamRead(stream, indices, 2 * geometryBinMeshObj->meshes[i].numIndices) != 2 * geometryBinMeshObj->meshes[i].numIndices)
						{
							free(indices);
							return rwFALSE;
						}
                        for (gtaRwInt32 j = 0; j < geometryBinMeshObj->meshes[i].numIndices; j++)
                            geometryBinMeshObj->meshes[i].indices[j] = indices[j];
						free(indices);
					}
					else
					{
                        if (gtaRwStreamRead(stream, geometryBinMeshObj->meshes[i].indices, 4 * geometryBinMeshObj->meshes[i].numIndices) !=
                            4 * geometryBinMeshObj->meshes[i].numIndices)
						{
                            gtaRwGeometryBinMeshDestroy(geometryBinMeshObj);
							return rwFALSE;
						}
					}
				}
			}
		}
	}
	return rwTRUE;
}

gtaRwUInt32 gtaRwGeometryBinMeshGetStreamSize(gtaRwGeometryBinMesh* geometryBinMeshObj, gtaRwBool IsNative)
{
    if (geometryBinMeshObj->enabled)
	{
		gtaRwUInt32 size = 24;
		if(!IsNative || gtaPlatform == PLATFORM_OGL)
		{
            for (int i = 0; i < geometryBinMeshObj->numMeshes; i++)
			{
				size += 8;
				if(IsNative)
                    size += geometryBinMeshObj->meshes[i].numIndices * 2;
				else
                    size += geometryBinMeshObj->meshes[i].numIndices * 4;
			}
		}
		return size;
	}
	return 0;
}