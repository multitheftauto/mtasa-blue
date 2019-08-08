#include "StdInc.h"

gtaRwBool gtaRwAtomicRead(gtaRwAtomic* atomicObj, gtaRwStream* stream, gtaRwBool geometryListIsEmpty, gtaRwUInt32 clumpVersion)
{
    gtaRwUInt32 length, entryLength, type;
    memset(atomicObj, 0, sizeof(gtaRwAtomic));
    if (!gtaRwStreamFindChunk(stream, rwID_ATOMIC, NULL, NULL, NULL))
        return rwFALSE;
    if (!gtaRwStreamFindChunk(stream, rwID_STRUCT, &length, NULL, NULL))
        return rwFALSE;
    if (gtaRwStreamRead(stream, (void*)(&atomicObj->frameIndex), length) != length)
        return rwFALSE;

    if (geometryListIsEmpty)
    {
        atomicObj->internalGeometry = (gtaRwGeometry*)malloc(sizeof(gtaRwGeometry));
        memset(atomicObj->internalGeometry, 0, sizeof(gtaRwGeometry));
        if (!gtaRwGeometryStreamRead(atomicObj->internalGeometry, stream, clumpVersion))
        {
            gtaRwAtomicDestroy(atomicObj);
            return rwFALSE;
        }
    }
    if (!gtaRwStreamFindChunk(stream, rwID_EXTENSION, &length, NULL, NULL))
    {
        gtaRwAtomicDestroy(atomicObj);
        return rwFALSE;
    }
    while (length && gtaRwStreamReadChunkHeader(stream, &type, &entryLength, NULL, NULL))
    {
        switch (type)
        {
            case rwID_RIGHTTORENDER:
                if (atomicObj->rights[0].enabled)
                {
                    if (!atomicObj->rights[1].enabled)
                    {
                        if (gtaRwRightsRead(&atomicObj->rights[1], stream))
                        {
                            gtaRwAtomicDestroy(atomicObj);
                            return rwFALSE;
                        }
                    }
                    else
                    {
                        if (!gtaRwStreamSkip(stream, entryLength))
                        {
                            gtaRwAtomicDestroy(atomicObj);
                            return rwFALSE;
                        }
                    }
                }
                else
                {
                    if (!gtaRwRightsRead(&atomicObj->rights[0], stream))
                    {
                        gtaRwAtomicDestroy(atomicObj);
                        return rwFALSE;
                    }
                }
                break;
            case rwID_MATFX:
                if (!gtaRwAtomicMatFXRead(&atomicObj->matFx, stream))
                {
                    gtaRwAtomicDestroy(atomicObj);
                    return rwFALSE;
                }
                break;
            case gtaID_PIPELINE:
                if (!gtaRwAtomicPipelineRead(&atomicObj->pipeline, stream))
                {
                    gtaRwAtomicDestroy(atomicObj);
                    return rwFALSE;
                }
                break;
            default:
                if (!gtaRwStreamSkip(stream, entryLength))
                {
                    gtaRwAtomicDestroy(atomicObj);
                    return rwFALSE;
                }
        }
        length += -12 - entryLength;
    }
    return rwTRUE;
}

gtaRwBool gtaRwAtomicWrite(gtaRwAtomic* atomicObj, gtaRwStream* stream)
{
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_ATOMIC, gtaRwAtomicSize(atomicObj) - 12, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRUCT, 16, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (!gtaRwStreamWrite(stream, &atomicObj->frameIndex, 16))
        return rwFALSE;
    if (atomicObj->internalGeometry)
    {
        gtaRwUInt32 clumpVersion = 0;
        if (!gtaRwGeometryStreamRead(atomicObj->internalGeometry, stream, clumpVersion))
            return rwFALSE;
    }

    gtaRwUInt32 rightsSize = 0;
    if (atomicObj->rights != NULL)
    {
        rightsSize = gtaRwRightsSize(&atomicObj->rights[0]) + gtaRwRightsSize(&atomicObj->rights[1]);
    }
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_EXTENSION,
                                              rightsSize + gtaRwAtomicPipelineSize(&atomicObj->pipeline) + gtaRwAtomicMatFXSize(&atomicObj->matFx),
                                              gtaRwVersion, gtaRwBuild))
    {
        return rwFALSE;
    }

    if (atomicObj->rights != NULL)
    {
        if (!gtaRwRightsWrite(&atomicObj->rights[0], stream))
            return rwFALSE;
        if (!gtaRwRightsWrite(&atomicObj->rights[1], stream))
            return rwFALSE;
    }
    if (!gtaRwAtomicMatFXWrite(&atomicObj->matFx, stream))
        return rwFALSE;
    if (!gtaRwAtomicPipelineWrite(&atomicObj->pipeline, stream))
        return rwFALSE;
    return rwTRUE;
}

gtaRwUInt32 gtaRwAtomicSize(gtaRwAtomic* atomicObj)
{
    gtaRwUInt32 rightsSize = 0;
    if (atomicObj->rights != NULL)
    {
        rightsSize = gtaRwRightsSize(&atomicObj->rights[0]) + gtaRwRightsSize(&atomicObj->rights[1]);
    }
    gtaRwUInt32 size = 52 + gtaRwAtomicPipelineSize(&atomicObj->pipeline) + gtaRwAtomicMatFXSize(&atomicObj->matFx) + rightsSize;
    if (atomicObj->internalGeometry)
        size += gtaRwGeometryGetStreamSize(atomicObj->internalGeometry);

    return size;
}

void gtaRwAtomicInit(gtaRwAtomic* atomicObj, gtaRwInt32 frameIndex, gtaRwInt32 geometryIndex, gtaRwUInt32 flags, gtaRwBool usesInternalGeometry)
{
    memset(atomicObj, 0, sizeof(gtaRwAtomic));
    frameIndex = frameIndex;
    geometryIndex = geometryIndex;
    flags = flags;
    if (usesInternalGeometry)
    {
        atomicObj->internalGeometry = (gtaRwGeometry*)malloc(sizeof(gtaRwGeometry));
        memset(atomicObj->internalGeometry, 0, sizeof(gtaRwGeometry));
    }
}

void gtaRwAtomicDestroy(gtaRwAtomic* atomicObj)
{
    if (atomicObj->internalGeometry)
    {
        gtaRwGeometryDestroy(atomicObj->internalGeometry);
        free(atomicObj->internalGeometry);
    }
    gtaRwAtomicMatFXDestroy(&atomicObj->matFx);
    if (atomicObj->rights != NULL)
    {
        gtaRwRightsDestroy(&atomicObj->rights[0]);
        gtaRwRightsDestroy(&atomicObj->rights[1]);
    }
    gtaRwAtomicPipelineDestroy(&atomicObj->pipeline);
    memset(atomicObj, 0, sizeof(gtaRwAtomic));
}