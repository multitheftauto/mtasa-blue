#include "StdInc.h"

__declspec(dllexport) gtaRwBool gtaRwClumpStreamRead(gtaRwClump* clumpObj, gtaRwStream* Stream)
{
	gtaRwUInt32 length, entryLength, type;
    memset(clumpObj, 0, sizeof(gtaRwClump));
    clumpObj->platform = PLATFORM_NOTDEFINED;
    if (!gtaRwStreamFindChunk(Stream, rwID_CLUMP, NULL, &clumpObj->version, NULL))
		return rwFALSE;
	if(!gtaRwStreamFindChunk(Stream, rwID_STRUCT, &length, NULL, NULL))
        return rwFALSE;
    if (gtaRwStreamRead(Stream, &clumpObj->numAtomics, length) != length)
        return rwFALSE;
    if (!gtaRwFrameListRead(&clumpObj->frameList,Stream))
        return rwFALSE;
    if (!gtaRwGeometryListStreamRead(&clumpObj->geometryList, Stream, clumpObj->version))
	{
        gtaRwClumpDestroy(clumpObj);
        return rwFALSE;
	}
    if (clumpObj->numAtomics > 0)
	{
        clumpObj->atomics = (gtaRwAtomic*)malloc(sizeof(gtaRwAtomic) * clumpObj->numAtomics);
        memset(&clumpObj->atomics, 0, sizeof(gtaRwAtomic) * clumpObj->numAtomics);
        for (gtaRwInt32 i = 0; i < clumpObj->numAtomics; i++)
		{
            if (!gtaRwAtomicRead(&clumpObj->atomics[i],Stream, clumpObj->geometryList.geometryCount == 0, clumpObj->version))
			{
                gtaRwClumpDestroy(clumpObj);
                return rwFALSE;
			}
		}
	}
	if(!gtaRwStreamFindChunk(Stream, rwID_EXTENSION, &length, NULL, NULL))
	{
        gtaRwClumpDestroy(clumpObj);
        return rwFALSE;
	}
	while(length && gtaRwStreamReadChunkHeader(Stream, &type, &entryLength, NULL, NULL))
	{
		switch(type)
		{
		default:
			if(!gtaRwStreamSkip(Stream, entryLength))
			{
                gtaRwClumpDestroy(clumpObj);
                return rwFALSE;
			}
		}
		length += -12 - entryLength;
	}
	return rwTRUE;
}

__declspec(dllexport) gtaRwBool gtaRwClumpStreamWrite(gtaRwClump* clumpObj, gtaRwStream* Stream)
{
    if (!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_CLUMP, gtaRwClumpGetStreamSize(clumpObj) - 12, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
	if(gtaRwVersion <= 0x32000)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_STRUCT, 4, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        if (!gtaRwStreamWrite(Stream, &clumpObj->numAtomics, 4))
            return rwFALSE;
	}
	else
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_STRUCT, 12, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        if (!gtaRwStreamWrite(Stream, &clumpObj->numAtomics, 12))
            return rwFALSE;
	}
    if (!gtaRwFrameListWrite(&clumpObj->frameList,Stream))
        return rwFALSE;
    if (!gtaRwGeometryListStreamWrite(&clumpObj->geometryList,Stream))
        return rwFALSE;
    for (gtaRwInt32 i = 0; i < clumpObj->numAtomics; i++)
	{
        if(!gtaRwAtomicWrite(&clumpObj->atomics[i], Stream))
            return rwFALSE;
	}
	if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_EXTENSION, 0, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
	//if(!Extension.collisionPlugin.StreamWrite(Stream))
	//	return rwFALSE;
	return rwTRUE;
}

__declspec(dllexport) gtaRwUInt32 gtaRwClumpGetStreamSize(gtaRwClump* clumpObj)
{
	gtaRwUInt32 size;
	if(gtaRwVersion <= 0x32000)
		size = 40;
	else
		size = 48;
    size += gtaRwGeometryListGetStreamSize(&clumpObj->geometryList) + gtaRwFrameListSize(&clumpObj->frameList);
    for (gtaRwInt32 i = 0; i < clumpObj->numAtomics; i++)

        size += gtaRwAtomicSize(&clumpObj->atomics[i]);
	return size;
}

__declspec(dllexport) void gtaRwClumpInitialise(gtaRwClump* clumpObj, gtaRwUInt32 NumAtomics, gtaRwUInt32 NumFrames, gtaRwUInt32 NumGeometries)
{
    memset(clumpObj, 0, sizeof(gtaRwClump));
    clumpObj->numAtomics = NumAtomics;
    clumpObj->numLights = 0;
    clumpObj->numCameras = 0;

	gtaRwFrameListInit(&clumpObj->frameList, NumFrames);
    gtaRwGeometryListInitialise(&clumpObj->geometryList,NumGeometries);
    clumpObj->atomics = (gtaRwAtomic*)gtaRwMemAlloc(sizeof(gtaRwAtomic) * NumAtomics);
}

__declspec(dllexport) void gtaRwClumpInitialiseIdentity(gtaRwClump* clumpObj, gtaRwUInt32 NumAtomics)
{
	memset(clumpObj, 0, sizeof(gtaRwClump));
    clumpObj->numAtomics = NumAtomics;
    clumpObj->numLights = 0;
    clumpObj->numCameras = 0;
    clumpObj->atomics = (gtaRwAtomic*)gtaRwMemAlloc(sizeof(gtaRwAtomic) * NumAtomics);
}

void gtaRwClumpDestroy(gtaRwClump* clumpObj)
{
    for (gtaRwInt32 i = 0; i < clumpObj->numAtomics; i++)
        gtaRwAtomicDestroy(&clumpObj->atomics[i]);
    gtaRwFrameListDestroy(&clumpObj->frameList);
    gtaRwGeometryListDestroy(&clumpObj->geometryList);
    memset(clumpObj, 0, sizeof(gtaRwClump));
}