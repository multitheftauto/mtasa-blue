//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

gtaRwUInt8 gtaRwGeometryGetTexCoordsCount(gtaRwGeometry* geometryObj)
{
    gtaRwUInt8 texCoordsCount = 0;
    if (geometryObj->numTexCoordSets > 0)
        texCoordsCount = geometryObj->numTexCoordSets;
    else
    {
        if (geometryObj->textured2)
            texCoordsCount = 2;
        else if (geometryObj->textured)
            texCoordsCount = 1;
    }
    return texCoordsCount;
}

void gtaRwGeometryInitialise(gtaRwGeometry* geometryObj, gtaRwInt32 NumTriangles, gtaRwInt32 NumVertices, gtaRwInt32 NumMorphTargets, gtaRwInt32 Format,
                             gtaRwUInt8 NumTexCoordsCustom, gtaRwReal CenterX, gtaRwReal CenterY, gtaRwReal CenterZ, gtaRwReal Radius)

{
    memset(geometryObj, 0, sizeof(gtaRwGeometry));
    geometryObj->format = Format;
    geometryObj->numTriangles = NumTriangles;
    geometryObj->numVertices = NumVertices;
    geometryObj->numMorphTargets = NumMorphTargets;
    geometryObj->numTexCoordSets = NumTexCoordsCustom;
    if (!geometryObj->native)
    {
        if (NumTriangles > 0)
        {
            geometryObj->triangles = (gtaRwTriangle*)gtaMemAlloc(sizeof(gtaRwTriangle) * NumTriangles);
            for (gtaRwInt32 i = 0; i < NumTriangles; i++)
                geometryObj->triangles[i].mtlId = -1;
        }
        if (NumVertices > 0)
        {
            if (geometryObj->prelit)
                geometryObj->preLitLum = (gtaRwRGBA*)gtaMemAlloc(sizeof(gtaRwRGBA) * NumVertices);
            gtaRwUInt8 texCoordsCount = GetTexCoordsCount();
            for (gtaRwInt8 i = 0; i < texCoordsCount; i++)
                geometryObj->texCoords[i] = (gtaRwTexCoords*)gtaMemAlloc(sizeof(gtaRwTexCoords) * NumVertices);
        }
    }
    if (NumMorphTargets > 0)
    {
        geometryObj->morphTarget = (gtaRwMorphTarget*)gtaMemAlloc(sizeof(gtaRwMorphTarget) * NumMorphTargets);
        for (gtaRwInt8 i = 0; i < NumMorphTargets; i++)
        {
            geometryObj->morphTarget[i].boundingSphere.center.x = CenterX;
            geometryObj->morphTarget[i].boundingSphere.center.y = CenterY;
            geometryObj->morphTarget[i].boundingSphere.center.z = CenterZ;
            geometryObj->morphTarget[i].boundingSphere.radius = Radius;
        }
        if (!geometryObj->native && NumVertices > 0)
        {
            if (geometryObj->positions)
            {
                for (gtaRwInt8 i = 0; i < NumMorphTargets; i++)
                {
                    geometryObj->morphTarget[i].hasVerts = TRUE;
                    geometryObj->morphTarget[i].verts = (gtaRwV3d*)gtaMemAlloc(sizeof(gtaRwV3d) * NumVertices);
                }
            }
            if (geometryObj->normals)
            {
                for (gtaRwInt8 i = 0; i < NumMorphTargets; i++)
                {
                    geometryObj->morphTarget[i].hasNormals = TRUE;
                    geometryObj->morphTarget[i].normals = (gtaRwV3d*)gtaMemAlloc(sizeof(gtaRwV3d) * NumVertices);
                }
            }
        }
    }
    geometryObj->geometryLighting.ambient = geometryObj->geometryLighting.diffuse = geometryObj->geometryLighting.specular = 1.0f;
}

void gtaRwGeometryDestroy(gtaRwGeometry* geometryObj)
{
    if (geometryObj->triangles)
        gtaMemFree(geometryObj->triangles);
    if (geometryObj->preLitLum)
        gtaMemFree(geometryObj->preLitLum);
    for (int i = 0; i < 8; i++)
    {
        if (geometryObj->texCoords[i])
            gtaMemFree(geometryObj->texCoords[i]);
    }
    if (geometryObj->morphTarget)
    {
        for (gtaRwInt8 i = 0; i < geometryObj->numMorphTargets; i++)
        {
            if (geometryObj->morphTarget[i].verts)
                gtaMemFree(geometryObj->morphTarget[i].verts);
            if (geometryObj->morphTarget[i].normals)
                gtaMemFree(geometryObj->morphTarget[i].normals);
        }
        free(geometryObj->morphTarget);
    }
    gtaRwMaterialListDestroy(&geometryObj->matList);
    gtaRwGeometryExtraVertColourDestroy(&geometryObj->Extension.extraColour);
    gtaRwGeometryBinMeshDestroy(&geometryObj->Extension.mesh);
    // geometryObj->Extension.effect2d.Destroy();
    gtaRWSkinDestroy(&geometryObj->Extension.skin);
    gtaRwGeometryBreakableDestroy(&geometryObj->Extension.breakable);
    gtaMemZero(geometryObj, sizeof(gtaRwGeometry));
}

gtaRwBool gtaRwGeometryStreamRead(gtaRwGeometry* geometryObj, gtaRwStream* stream, gtaRwUInt32 ClumpVersion)
{
    gtaRwUInt32 length, entryLength, type;
    memset(geometryObj, 0, sizeof(gtaRwGeometry));
    if (!gtaRwStreamFindChunk(stream, rwID_GEOMETRY, NULL, NULL, NULL))
        return rwFALSE;
    if (!gtaRwStreamFindChunk(stream, rwID_STRUCT, NULL, NULL, NULL))
        return rwFALSE;
    if (gtaRwStreamRead(stream, &geometryObj->format, 16) != 16)
        return rwFALSE;
    if (ClumpVersion < 0x34003)
    {
        if (gtaRwStreamRead(stream, &geometryObj->geometryLighting, 12) != 12)
            return rwFALSE;
    }
    if (geometryObj->numVertices > 65536)
        gtaRwErrorSet("Warning: geometry has more than 65536 vertices (%d vertices)", geometryObj->numVertices);
    if (!geometryObj->native)
    {
        if (geometryObj->numVertices > 0)
        {
            if (geometryObj->prelit)
            {
                geometryObj->preLitLum = (gtaRwRGBA*)gtaMemAlloc(4 * geometryObj->numVertices);
                if (gtaRwStreamRead(stream, geometryObj->preLitLum, 4 * geometryObj->numVertices) != 4 * geometryObj->numVertices)
                {
                    Destroy();
                    return rwFALSE;
                }
            }
            gtaRwUInt8 texCoordsCount = GetTexCoordsCount();
            for (gtaRwInt8 i = 0; i < texCoordsCount; i++)
            {
                geometryObj->texCoords[i] = (gtaRwTexCoords*)gtaMemAlloc(8 * geometryObj->numVertices);
                if (gtaRwStreamRead(stream, geometryObj->texCoords[i], 8 * geometryObj->numVertices) != 8 * geometryObj->numVertices)
                {
                    Destroy();
                    return rwFALSE;
                }
            }
        }
        if (geometryObj->numTriangles > 0)
        {
            geometryObj->triangles = (gtaRwTriangle*)gtaMemAlloc(8 * geometryObj->numTriangles);
            if (gtaRwStreamRead(stream, geometryObj->triangles, 8 * geometryObj->numTriangles) != 8 * geometryObj->numTriangles)
            {
                Destroy();
                return rwFALSE;
            }
        }
    }
    if (geometryObj->numMorphTargets > 0)
    {
        geometryObj->morphTarget = (gtaRwMorphTarget*)gtaMemAlloc(sizeof(gtaRwMorphTarget) * geometryObj->numMorphTargets);
        for (gtaRwInt8 i = 0; i < geometryObj->numMorphTargets; i++)
        {
            if (gtaRwStreamRead(stream, &geometryObj->morphTarget[i], 24) != 24)
            {
                Destroy();
                return rwFALSE;
            }
            if (geometryObj->morphTarget[i].hasVerts)
            {
                geometryObj->morphTarget[i].verts = (gtaRwV3d*)gtaMemAlloc(12 * geometryObj->numVertices);
                if (gtaRwStreamRead(stream, geometryObj->morphTarget[i].verts, 12 * geometryObj->numVertices) != 12 * geometryObj->numVertices)
                {
                    Destroy();
                    return rwFALSE;
                }
            }
            else
                geometryObj->morphTarget[i].verts = NULL;
            if (geometryObj->morphTarget[i].hasNormals)
            {
                geometryObj->morphTarget[i].normals = (gtaRwV3d*)gtaMemAlloc(12 * geometryObj->numVertices);
                if (gtaRwStreamRead(stream, geometryObj->morphTarget[i].normals, 12 * geometryObj->numVertices) != 12 * geometryObj->numVertices)
                {
                    Destroy();
                    return rwFALSE;
                }
            }
            else
                geometryObj->morphTarget[i].normals = NULL;
        }
    }
    if (!gtaRwMaterialListRead(&geometryObj->matList, stream))
    {
        Destroy();
        return rwFALSE;
    }
    if (!gtaRwStreamFindChunk(stream, rwID_EXTENSION, &length, NULL, NULL))
    {
        Destroy();
        return rwFALSE;
    }
    while (length && gtaRwStreamReadChunkHeader(stream, &type, &entryLength, NULL, NULL))
    {
        gtaRwUInt32 dummy_el, dummy_sz;
        switch (type)
        {
            case rwID_BINMESH:
                
                if (!gtaRwGeometryBinMeshStreamRead(&geometryObj->Extension.mesh, stream, geometryObj->native))
                {
                    Destroy();
                    return rwFALSE;
                }
                break;
            case rwID_NATIVEDATA:

                if (!gtaNativeOGlStreamRead(&geometryObj->Extension.native, stream, geometryObj->numVertices))
                {
                    Destroy();
                    return rwFALSE;
                }
                break;
            case rwID_SKIN:
                if (!gtaRWSkinStreamRead(&geometryObj->Extension.skin, stream, geometryObj->numVertices, geometryObj->native))
                {
                    Destroy();
                    return rwFALSE;
                }
                break;
            case gtaID_EXTRAVERTCOLOUR:
                if (!gtaRwGeometryExtraVertColourRead(&geometryObj->Extension.extraColour, stream, geometryObj->numVertices))
                {
                    Destroy();
                    return rwFALSE;
                }
                break;
            case gtaID_BREAKABLE:
                if (!gtaRwGeometryBreakableDestroy(&geometryObj->Extension.breakable, stream))
                {
                    Destroy();
                    return rwFALSE;
                }
                break;
            case gtaID_2DEFFECT:
                /*if (!geometryObj->Extension.effect2d.StreamRead(stream))
                {
                    Destroy();
                    return rwFALSE;
                }*/
                break;
            default:
                if (!gtaRwStreamSkip(stream, entryLength))
                {
                    Destroy();
                    return rwFALSE;
                }
        }
        length += -12 - entryLength;
    }
    return rwTRUE;
}

gtaRwBool gtaRwGeometryStreamWrite(gtaRwGeometry* geometryObj, gtaRwStream* stream)
{
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_GEOMETRY, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRUCT, GetStreamActualSize(), gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (!gtaRwStreamWrite(stream, &geometryObj->format, 16))
        return rwFALSE;
    if (!geometryObj->native && geometryObj->numVertices > 0)
    {
        if (geometryObj->prelit)
        {
            if (!gtaRwStreamWrite(stream, geometryObj->preLitLum, 4 * geometryObj->numVertices))
                return rwFALSE;
        }
        gtaRwUInt8 texCoordsCount = GetTexCoordsCount();
        for (gtaRwInt8 i = 0; i < texCoordsCount; i++)
        {
            if (geometryObj->texCoords[i])
            {
                if (!gtaRwStreamWrite(stream, geometryObj->texCoords[i], 8 * geometryObj->numVertices))
                    return rwFALSE;
            }
            else
            {
                if (geometryObj->texCoords[0])
                {
                    if (!gtaRwStreamWrite(stream, geometryObj->texCoords[0], 8 * geometryObj->numVertices))
                        return rwFALSE;
                }
                else
                {
                    gtaRwChar* temporaryTexCoords = (gtaRwChar*)gtaMemAlloc(8 * geometryObj->numVertices);
                    if (!gtaRwStreamWrite(stream, temporaryTexCoords, 8 * geometryObj->numVertices))
                    {
                        gtaMemFree(temporaryTexCoords);
                        return rwFALSE;
                    }
                    gtaMemFree(temporaryTexCoords);
                }
            }
        }
        if (geometryObj->numTriangles > 0)
        {
            if (!gtaRwStreamWrite(stream, geometryObj->triangles, 8 * geometryObj->numTriangles))
                return rwFALSE;
        }
    }
    for (gtaRwInt8 i = 0; i < geometryObj->numMorphTargets; i++)
    {
        if (geometryObj->native)
        {
            geometryObj->morphTarget[i].hasVerts = FALSE;
            geometryObj->morphTarget[i].hasNormals = FALSE;
            if (!gtaRwStreamWrite(stream, &geometryObj->morphTarget[i].boundingSphere, 24))
                return rwFALSE;
        }
        else
        {
            if (geometryObj->morphTarget[i].verts)
                geometryObj->morphTarget[i].hasVerts = TRUE;
            else
                geometryObj->morphTarget[i].hasVerts = FALSE;
            if (geometryObj->morphTarget[i].normals)
                geometryObj->morphTarget[i].hasNormals = TRUE;
            else
                geometryObj->morphTarget[i].hasNormals = FALSE;
            if (!gtaRwStreamWrite(stream, &geometryObj->morphTarget[i].boundingSphere, 24))
                return rwFALSE;
            if (geometryObj->morphTarget[i].verts)
            {
                if (!gtaRwStreamWrite(stream, geometryObj->morphTarget[i].verts, 12 * geometryObj->numVertices))
                    return rwFALSE;
            }
            if (geometryObj->morphTarget[i].normals)
            {
                if (!gtaRwStreamWrite(stream, geometryObj->morphTarget[i].normals, 12 * geometryObj->numVertices))
                    return rwFALSE;
            }
        }
    }

    if (!gtaRwMaterialWrite(&geometryObj->matList, stream))
        return rwFALSE;
    if (!gtaRwStreamWriteVersionedChunkHeader(
            stream, rwID_EXTENSION,
                                              gtaRwGeometryGetStreamSize(&geometryObj->Extension.extraColour, geometryObj->numVertices) +
                                                  gtaRwGeometryBinMeshGetStreamSize(&geometryObj->Extension.mesh, geometryObj->native) +
                                                  /*geometryObj->Extension.effect2d.GetStreamSize() +*/
                                                  gtaRwGeometryNativeGetStreamSize(&geometryObj->Extension.native, geometryObj->numVertices) +
                                                  gtaRWSkinGetStreamSize(&geometryObj->Extension.skin, geometryObj->native, geometryObj->numVertices) +
                                                  gtaRwGeometryBreakableSize(&geometryObj->Extension.breakable),
                                              gtaRwVersion, gtaRwBuild))
    {
        return rwFALSE;
    }
    if (!gtaRwGeometryBinMeshStreamWrite(&geometryObj->Extension.mesh,stream, geometryObj->native))
        return rwFALSE;
    if (geometryObj->native)
    {
        if (!gtaRwGeometryNativeStreamWrite(&geometryObj->Extension.native, stream, geometryObj->numVertices))
            return rwFALSE;
    }
    if (!gtaRWSkinStreamWrite(&geometryObj->Extension.skin,stream, geometryObj->numVertices, geometryObj->native))
        return rwFALSE;
    if (!gtaRwGeometryExtraVertColourWrite(&geometryObj->Extension.extraColour,stream, geometryObj->numVertices))
        return rwFALSE;
    if (!gtaRwGeometryBreakableWrite(&geometryObj->Extension.breakable,stream))
        return rwFALSE;
    //if (!geometryObj->Extension.effect2d.StreamWrite(stream))
    //    return rwFALSE;
    return rwTRUE;
}

gtaRwUInt32 gtaRwGeometryGetStreamActualSize(gtaRwGeometry* geometryObj)
{
    gtaRwUInt32 size = 16;
    if (gtaRwVersion < 0x34001)
        size += 12;
    if (!geometryObj->native)
    {
        if (geometryObj->numVertices > 0)
        {
            gtaRwUInt8 texCoordsCount = GetTexCoordsCount();
            if (geometryObj->prelit)
                size += 4 * geometryObj->numVertices;
            size += 8 * (geometryObj->numTriangles + geometryObj->numVertices * texCoordsCount);
        }
    }
    for (gtaRwInt8 i = 0; i < geometryObj->numMorphTargets; i++)
    {
        size += 24;
        if (!geometryObj->native)
        {
            if (geometryObj->morphTarget[i].verts)
                size += 12 * geometryObj->numVertices;
            if (geometryObj->morphTarget[i].normals)
                size += 12 * geometryObj->numVertices;
        }
    }
    return size;
}

gtaRwUInt32 gtaRwGeometryGetStreamSize(gtaRwGeometry* geometryObj)
{
    return 36 + GetStreamActualSize() + gtaRwMaterialListSize(&geometryObj->matList) +
           gtaRwGeometryBinMeshGetStreamSize(&geometryObj->Extension.mesh,geometryObj->native) +
           gtaRwGeometryExtraVertColourSize(&geometryObj->Extension.extraColour,geometryObj->numVertices) +
           /*geometryObj->Extension.effect2d.GetStreamSize() +*/
           gtaRwGeometryNativeGetStreamSize(&geometryObj->Extension.native,geometryObj->numVertices) +
           gtaRWSkinStreamWrite(&geometryObj->Extension.skin, geometryObj->native, geometryObj->numVertices, geometryObj->native) +
           gtaRwGeometryBreakableSize(&geometryObj->Extension.breakable);
}

gtaRwBool gtaRwGeometryConvertFromOGlNative(gtaRwGeometry* geometryObj)
{
    if (!geometryObj->native || !geometryObj->Extension.native.ogl.generated || !geometryObj->Extension.native.ogl.numVertexElements ||
        !geometryObj->Extension.native.ogl.vertexBuffer)
        return rwFALSE;
    gtaRwUInt32* vertexElementSizes = (gtaRwUInt32*)gtaMemAlloc(4 * geometryObj->Extension.native.ogl.numVertexElements);
    gtaRwBool    hasPositions = rwFALSE, hasNormals = rwFALSE, hasColors = rwFALSE, hasExtraColors = rwFALSE, hasTexCoords = rwFALSE, hasBoneWeights = rwFALSE,
              hasBoneIndices = rwFALSE;
    for (gtaRwInt32 i = geometryObj->Extension.native.ogl.numVertexElements - 1; i >= 0; i--)
    {
        gtaRwUInt32 previousOffset;
        if (i == geometryObj->Extension.native.ogl.numVertexElements - 1)
            previousOffset = geometryObj->Extension.native.ogl.vertexElements[i].stride;
        else
            previousOffset = geometryObj->Extension.native.ogl.vertexElements[i + 1].offset;
        vertexElementSizes[i] = previousOffset - geometryObj->Extension.native.ogl.vertexElements[i].offset;
    }

    geometryObj->positions = rwFALSE;
    geometryObj->normals = rwFALSE;
    geometryObj->numTexCoordSets = 0;
    geometryObj->textured = rwFALSE;
    geometryObj->textured2 = rwFALSE;
    geometryObj->prelit = rwFALSE;
    for (gtaRwInt32 i = 0; i < geometryObj->numMorphTargets; i++)
    {
        geometryObj->morphTarget[i].hasVerts = rwFALSE;
        geometryObj->morphTarget[i].hasNormals = rwFALSE;
        if (geometryObj->morphTarget[i].verts)
            gtaMemFree(geometryObj->morphTarget[i].verts);
        if (geometryObj->morphTarget[i].normals)
            gtaMemFree(geometryObj->morphTarget[i].normals);
    }
    for (gtaRwInt32 i = 0; i < 8; i++)
    {
        if (geometryObj->texCoords[i])
            gtaMemFree(geometryObj->texCoords[i]);
    }
    if (geometryObj->preLitLum)
        gtaMemFree(geometryObj->preLitLum);
    if (geometryObj->Extension.skin.enabled)
    {
        if (geometryObj->Extension.skin.vertexBoneIndices)
            gtaMemFree(geometryObj->Extension.skin.vertexBoneIndices);
        if (geometryObj->Extension.skin.vertexBoneWeights)
            gtaMemFree(geometryObj->Extension.skin.vertexBoneWeights);
        geometryObj->Extension.skin.vertexBoneIndices = (gtaRwBoneIndices*)gtaMemAlloc(geometryObj->numVertices * 4);
        geometryObj->Extension.skin.vertexBoneWeights = (gtaRwBoneWeights*)gtaMemAlloc(geometryObj->numVertices * 16);
        if (geometryObj->Extension.skin.skinToBoneMatrices)
            gtaMemFree(geometryObj->Extension.skin.skinToBoneMatrices);
    }

    for (gtaRwInt32 i = 0; i < geometryObj->Extension.native.ogl.numVertexElements; i++)
    {
        switch (geometryObj->Extension.native.ogl.vertexElements[i].paramType)
        {
            case OGL_VERTEX_POSITION:
                geometryObj->positions = rwTRUE;
                for (gtaRwInt32 mt = 0; mt < geometryObj->numMorphTargets; mt++)
                {
                    geometryObj->morphTarget[mt].hasVerts = rwTRUE;
                    geometryObj->morphTarget[mt].verts = (gtaRwV3d*)gtaMemAlloc(geometryObj->numVertices * 12);
                    for (gtaRwInt32 vertex = 0; vertex < geometryObj->numVertices; vertex++)
                    {
                        gtaRwV3d* position = (gtaRwV3d*)((gtaRwUInt32)geometryObj->Extension.native.ogl.vertexBuffer +
                                                         geometryObj->Extension.native.ogl.vertexElements[i].stride * vertex +
                                                         geometryObj->Extension.native.ogl.vertexElements[i].offset);
                        geometryObj->morphTarget[mt].verts[vertex] = *position;
                    }
                }
                break;
            case OGL_VERTEX_NORMAL:
                geometryObj->normals = rwTRUE;
                for (gtaRwInt32 mt = 0; mt < geometryObj->numMorphTargets; mt++)
                {
                    geometryObj->morphTarget[mt].hasNormals = rwTRUE;
                    geometryObj->morphTarget[mt].normals = (gtaRwV3d*)gtaMemAlloc(geometryObj->numVertices * 12);
                    for (gtaRwInt32 vertex = 0; vertex < geometryObj->numVertices; vertex++)
                    {
                        gtaNativeOGlCompressedNormal* normal =
                            (gtaNativeOGlCompressedNormal*)((gtaRwUInt32)geometryObj->Extension.native.ogl.vertexBuffer +
                                                            geometryObj->Extension.native.ogl.vertexElements[i].stride * vertex +
                                                            geometryObj->Extension.native.ogl.vertexElements[i].offset);
                        if (normal->x > 0)
                            geometryObj->morphTarget[mt].normals[vertex].x = (float)normal->x / 127.0f;
                        else
                            geometryObj->morphTarget[mt].normals[vertex].x = (float)normal->x / 128.0f;
                        if (normal->x > 0)
                            geometryObj->morphTarget[mt].normals[vertex].y = (float)normal->y / 127.0f;
                        else
                            geometryObj->morphTarget[mt].normals[vertex].y = (float)normal->y / 128.0f;
                        if (normal->x > 0)
                            geometryObj->morphTarget[mt].normals[vertex].z = (float)normal->z / 127.0f;
                        else
                            geometryObj->morphTarget[mt].normals[vertex].z = (float)normal->z / 128.0f;
                    }
                }
                break;
            case OGL_VERTEX_TEXCOORD0:
                geometryObj->numTexCoordSets = 1;
                geometryObj->textured = rwTRUE;
                geometryObj->texCoords[0] = (gtaRwTexCoords*)gtaMemAlloc(geometryObj->numVertices * 8);
                for (gtaRwInt32 vertex = 0; vertex < geometryObj->numVertices; vertex++)
                {
                    gtaNativeOGlCompressedTexCoords* texCoordsCompressed =
                        (gtaNativeOGlCompressedTexCoords*)((gtaRwUInt32)geometryObj->Extension.native.ogl.vertexBuffer +
                                                           geometryObj->Extension.native.ogl.vertexElements[i].stride * vertex +
                                                           geometryObj->Extension.native.ogl.vertexElements[i].offset);
                    geometryObj->texCoords[0][vertex].u = (float)texCoordsCompressed->u / 512.0f;
                    geometryObj->texCoords[0][vertex].v = (float)texCoordsCompressed->v / 512.0f;
                }
                break;
            case OGL_VERTEX_GLOBALCOLOR:
                geometryObj->prelit = rwTRUE;
                geometryObj->preLitLum = (gtaRwRGBA*)gtaMemAlloc(geometryObj->numVertices * 4);
                for (gtaRwInt32 vertex = 0; vertex < geometryObj->numVertices; vertex++)
                {
                    gtaRwRGBA* color = (gtaRwRGBA*)((gtaRwUInt32)geometryObj->Extension.native.ogl.vertexBuffer +
                                                    geometryObj->Extension.native.ogl.vertexElements[i].stride * vertex +
                                                    geometryObj->Extension.native.ogl.vertexElements[i].offset);
                    geometryObj->preLitLum[vertex] = *color;
                }
                break;
            case OGL_VERTEX_COLOR2:
                gtaRwGeometryExtraVertColourInit(&geometryObj->Extension.extraColour, geometryObj->numVertices);
                for (gtaRwInt32 vertex = 0; vertex < geometryObj->numVertices; vertex++)
                {
                    gtaRwRGBA* color = (gtaRwRGBA*)((gtaRwUInt32)geometryObj->Extension.native.ogl.vertexBuffer +
                                                    geometryObj->Extension.native.ogl.vertexElements[i].stride * vertex +
                                                    geometryObj->Extension.native.ogl.vertexElements[i].offset);
                    geometryObj->Extension.extraColour.nightColors[vertex] = *color;
                }
                break;
            case OGL_VERTEX_BLENDINDICES:
                if (geometryObj->Extension.skin.enabled)
                {
                    for (gtaRwInt32 vertex = 0; vertex < geometryObj->numVertices; vertex++)
                    {
                        gtaRwBoneIndices* boneIndices = (gtaRwBoneIndices*)((gtaRwUInt32)geometryObj->Extension.native.ogl.vertexBuffer +
                                                                            geometryObj->Extension.native.ogl.vertexElements[i].stride * vertex +
                                                                            geometryObj->Extension.native.ogl.vertexElements[i].offset);
                        geometryObj->Extension.skin.vertexBoneIndices[vertex] = *boneIndices;
                    }
                }
                break;
            case OGL_VERTEX_BLENDWEIGHT:
                if (geometryObj->Extension.skin.enabled)
                {
                    for (gtaRwInt32 vertex = 0; vertex < geometryObj->numVertices; vertex++)
                    {
                        gtaCompressedBoneWeights* boneWeights =
                            (gtaCompressedBoneWeights*)((gtaRwUInt32)geometryObj->Extension.native.ogl.vertexBuffer +
                                                        geometryObj->Extension.native.ogl.vertexElements[i].stride * vertex +
                                                        geometryObj->Extension.native.ogl.vertexElements[i].offset);
                        geometryObj->Extension.skin.vertexBoneWeights[vertex].w0 = (float)boneWeights->w0 / 255.0f;
                        geometryObj->Extension.skin.vertexBoneWeights[vertex].w1 = (float)boneWeights->w1 / 255.0f;
                        geometryObj->Extension.skin.vertexBoneWeights[vertex].w2 = (float)boneWeights->w2 / 255.0f;
                        geometryObj->Extension.skin.vertexBoneWeights[vertex].w3 = (float)boneWeights->w3 / 255.0f;
                    }
                }
                break;
        }
    }
    if (!geometryObj->triangles && geometryObj->Extension.mesh.enabled)
    {
        if (geometryObj->Extension.mesh.tristrip)
        {
            geometryObj->tristrip = rwTRUE;
            geometryObj->numTriangles = 0;
            for (gtaRwInt32 i = 0; i < geometryObj->Extension.mesh.numMeshes; i++)
                geometryObj->numTriangles += geometryObj->Extension.mesh.meshes[i].numIndices - 2;
            geometryObj->triangles = (gtaRwTriangle*)gtaMemAlloc(geometryObj->numTriangles * 8);
            gtaRwUInt32 triangleId = 0;
            for (gtaRwInt32 i = 0; i < geometryObj->Extension.mesh.numMeshes; i++)
            {
                geometryObj->triangles[triangleId].mtlId = geometryObj->Extension.mesh.meshes[i].material;
                geometryObj->triangles[triangleId].vertA = geometryObj->Extension.mesh.meshes[i].indices[0];
                geometryObj->triangles[triangleId].vertB = geometryObj->Extension.mesh.meshes[i].indices[1];
                geometryObj->triangles[triangleId].vertC = geometryObj->Extension.mesh.meshes[i].indices[2];
                triangleId++;
                gtaRwBool iteration = rwFALSE;
                for (gtaRwInt32 j = 3; j < geometryObj->Extension.mesh.meshes[i].numIndices; j++)
                {
                    if (iteration)
                    {
                        iteration = rwFALSE;
                        geometryObj->triangles[triangleId].mtlId = geometryObj->Extension.mesh.meshes[i].material;
                        geometryObj->triangles[triangleId].vertA = geometryObj->Extension.mesh.meshes[i].indices[j - 2];
                        geometryObj->triangles[triangleId].vertB = geometryObj->Extension.mesh.meshes[i].indices[j - 1];
                        geometryObj->triangles[triangleId].vertC = geometryObj->Extension.mesh.meshes[i].indices[j];
                        triangleId++;
                    }
                    else
                    {
                        iteration = rwTRUE;
                        geometryObj->triangles[triangleId].mtlId = geometryObj->Extension.mesh.meshes[i].material;
                        geometryObj->triangles[triangleId].vertA = geometryObj->Extension.mesh.meshes[i].indices[j];
                        geometryObj->triangles[triangleId].vertB = geometryObj->Extension.mesh.meshes[i].indices[j - 1];
                        geometryObj->triangles[triangleId].vertC = geometryObj->Extension.mesh.meshes[i].indices[j - 2];
                        triangleId++;
                    }
                }
            }
        }
        else
        {
            geometryObj->numTriangles = geometryObj->Extension.mesh.totalIndicesInMesh / 3;
            geometryObj->triangles = (gtaRwTriangle*)gtaMemAlloc(geometryObj->numTriangles * 8);
            gtaRwUInt32 triangleId = 0;
            for (gtaRwInt32 i = 0; i < geometryObj->Extension.mesh.numMeshes; i++)
            {
                for (gtaRwInt32 j = 0; j < geometryObj->Extension.mesh.meshes[i].numIndices / 3; j++)
                {
                    geometryObj->triangles[triangleId].mtlId = geometryObj->Extension.mesh.meshes[i].material;
                    geometryObj->triangles[triangleId].vertA = geometryObj->Extension.mesh.meshes[i].indices[j];
                    geometryObj->triangles[triangleId].vertB = geometryObj->Extension.mesh.meshes[i].indices[j + 1];
                    geometryObj->triangles[triangleId].vertC = geometryObj->Extension.mesh.meshes[i].indices[j + 2];
                    triangleId++;
                }
            }
        }
    }
    if (geometryObj->Extension.skin.enabled)
    {
        gtaRWSkinFindUsedBoneIds(&geometryObj->Extension.skin,geometryObj->numVertices, geometryObj->Extension.skin.ogl.numBones);
        geometryObj->Extension.skin.skinToBoneMatrices = (gtaRwMatrix*)gtaMemAlloc(geometryObj->Extension.skin.numBones * 64);
        gtaMemCopy(geometryObj->Extension.skin.skinToBoneMatrices, geometryObj->Extension.skin.ogl.skinToBoneMatrices,
                   geometryObj->Extension.skin.numBones * 64);
    }
    geometryObj->native = rwFALSE;
    gtaMemFree(vertexElementSizes);
    return rwTRUE;
}

void gtaRwGeometryRecalculateFaces(gtaRwGeometry* geometryObj)
{
    if (geometryObj->Extension.mesh.enabled)
    {
        if (geometryObj->triangles)
            gtaMemFree(geometryObj->triangles);
        gtaRwUInt32    tempNumTriangles;
        gtaRwTriangle* tempTriangles;
        if (geometryObj->Extension.mesh.tristrip)
        {
            geometryObj->tristrip = rwTRUE;
            tempNumTriangles = 0;
            for (gtaRwInt32 i = 0; i < geometryObj->Extension.mesh.numMeshes; i++)
                tempNumTriangles += geometryObj->Extension.mesh.meshes[i].numIndices - 2;
            tempTriangles = (gtaRwTriangle*)gtaMemAlloc(tempNumTriangles * 8);
            gtaRwUInt32 triangleId = 0;
            for (gtaRwInt32 i = 0; i < geometryObj->Extension.mesh.numMeshes; i++)
            {
                tempTriangles[triangleId].mtlId = geometryObj->Extension.mesh.meshes[i].material;
                tempTriangles[triangleId].vertA = geometryObj->Extension.mesh.meshes[i].indices[0];
                tempTriangles[triangleId].vertB = geometryObj->Extension.mesh.meshes[i].indices[1];
                tempTriangles[triangleId].vertC = geometryObj->Extension.mesh.meshes[i].indices[2];
                triangleId++;
                gtaRwBool iteration = rwFALSE;
                for (gtaRwInt32 j = 3; j < geometryObj->Extension.mesh.meshes[i].numIndices; j++)
                {
                    if (iteration)
                    {
                        iteration = rwFALSE;
                        tempTriangles[triangleId].mtlId = geometryObj->Extension.mesh.meshes[i].material;
                        tempTriangles[triangleId].vertA = geometryObj->Extension.mesh.meshes[i].indices[j - 2];
                        tempTriangles[triangleId].vertB = geometryObj->Extension.mesh.meshes[i].indices[j - 1];
                        tempTriangles[triangleId].vertC = geometryObj->Extension.mesh.meshes[i].indices[j];
                        triangleId++;
                    }
                    else
                    {
                        iteration = rwTRUE;
                        tempTriangles[triangleId].mtlId = geometryObj->Extension.mesh.meshes[i].material;
                        tempTriangles[triangleId].vertA = geometryObj->Extension.mesh.meshes[i].indices[j];
                        tempTriangles[triangleId].vertB = geometryObj->Extension.mesh.meshes[i].indices[j - 1];
                        tempTriangles[triangleId].vertC = geometryObj->Extension.mesh.meshes[i].indices[j - 2];
                        triangleId++;
                    }
                }
            }
        }
        else
        {
            tempNumTriangles = geometryObj->Extension.mesh.totalIndicesInMesh / 3;
            tempTriangles = (gtaRwTriangle*)gtaMemAlloc(tempNumTriangles * 8);
            gtaRwUInt32 triangleId = 0;
            for (gtaRwInt32 i = 0; i < geometryObj->Extension.mesh.numMeshes; i++)
            {
                for (gtaRwInt32 j = 0; j < geometryObj->Extension.mesh.meshes[i].numIndices; j += 3)
                {
                    tempTriangles[triangleId].mtlId = geometryObj->Extension.mesh.meshes[i].material;
                    tempTriangles[triangleId].vertA = geometryObj->Extension.mesh.meshes[i].indices[j];
                    tempTriangles[triangleId].vertB = geometryObj->Extension.mesh.meshes[i].indices[j + 1];
                    tempTriangles[triangleId].vertC = geometryObj->Extension.mesh.meshes[i].indices[j + 2];
                    triangleId++;
                }
            }
        }
        // delete degenerated faces
        geometryObj->numTriangles = 0;
        for (gtaRwInt32 i = 0; i < tempNumTriangles; i++)
        {
            if (tempTriangles[i].vertA == tempTriangles[i].vertB || tempTriangles[i].vertA == tempTriangles[i].vertC ||
                tempTriangles[i].vertB == tempTriangles[i].vertC)
                continue;
            tempTriangles[geometryObj->numTriangles].mtlId = tempTriangles[i].mtlId;
            tempTriangles[geometryObj->numTriangles].vertA = tempTriangles[i].vertA;
            tempTriangles[geometryObj->numTriangles].vertB = tempTriangles[i].vertB;
            tempTriangles[geometryObj->numTriangles].vertC = tempTriangles[i].vertC;
            geometryObj->numTriangles++;
        }
        geometryObj->triangles = (gtaRwTriangle*)gtaMemAlloc(geometryObj->numTriangles * 8);
        gtaMemCopy(geometryObj->triangles, tempTriangles, geometryObj->numTriangles * 8);
        gtaMemFree(tempTriangles);
    }
}

void gtaRwGeometryConvertFromNative(gtaRwGeometry* geometryObj, gtaPlatformId Platform)
{
    switch (Platform)
    {
        case PLATFORM_OGL:
            ConvertFromOGlNative();
            break;
        default:
            return;
    }
}