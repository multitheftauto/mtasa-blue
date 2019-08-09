#pragma once

#include "Clump.h"
#include "StdInc.h"
#include "Memory.h"

gtaRwClump::gtaRwClump()
{
	memset(this, 0, sizeof(gtaRwClump));
}

gtaRwBool gtaRwClump::StreamRead(gtaRwStream *Stream)
{
	gtaRwUInt32 length, entryLength, type;
	memset(this, 0, sizeof(gtaRwClump));
	platform = PLATFORM_NOTDEFINED;
	if(!gtaRwStreamFindChunk(Stream, rwID_CLUMP, NULL, &version))
		return false;
	if(!gtaRwStreamFindChunk(Stream, rwID_STRUCT, &length, NULL))
		return false;
	if(gtaRwStreamRead(Stream, &numAtomics, length) != length)
		return false;
	if(!frameList.StreamRead(Stream))
		return false;
	if(!geometryList.StreamRead(Stream, version))
	{
		Destroy();
		return false;
	}
	if(numAtomics > 0)
	{
		atomics = (gtaRwAtomic *)malloc(sizeof(gtaRwAtomic) * numAtomics);
		memset(atomics, 0, sizeof(gtaRwAtomic) * numAtomics);
		for(gtaRwInt32 i = 0; i < numAtomics; i++)
		{
			if(!atomics[i].StreamRead(Stream, geometryList.geometryCount == 0, version))
			{
				Destroy();
				return false;
			}
		}
	}
	if(!gtaRwStreamFindChunk(Stream, rwID_EXTENSION, &length, NULL))
	{
		Destroy();
		return false;
	}
	while(length && gtaRwStreamReadChunkHeader(Stream, &type, &entryLength, NULL, NULL))
	{
		switch(type)
		{
		//case gtaID_COLLISIONPLUGIN:
		//	if(!Extension.collisionPlugin.StreamRead(Stream))
		//	{
		//		Destroy();
		//		return false;
		//	}
		//	break;
		default:
			if(!gtaRwStreamSkip(Stream, entryLength))
			{
				Destroy();
				return false;
			}
		}
		length += -12 - entryLength;
	}
	return true;
}

gtaRwBool gtaRwClump::StreamWrite(gtaRwStream *Stream)
{
	if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_CLUMP, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
		return false;
	if(gtaRwVersion <= 0x32000)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_STRUCT, 4, gtaRwVersion, gtaRwBuild))
			return false;
		if(!gtaRwStreamWrite(Stream, &numAtomics, 4))
			return false;
	}
	else
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_STRUCT, 12, gtaRwVersion, gtaRwBuild))
			return false;
		if(!gtaRwStreamWrite(Stream, &numAtomics, 12))
			return false;
	}
	if(!frameList.StreamWrite(Stream))
		return false;
	if(!geometryList.StreamWrite(Stream))
		return false;
	for(gtaRwInt32 i = 0; i < numAtomics; i++)
	{
		if(!atomics[i].StreamWrite(Stream))
			return false;
	}
	if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_EXTENSION, 0, gtaRwVersion, gtaRwBuild))
		return false;
	//if(!Extension.collisionPlugin.StreamWrite(Stream))
	//	return false;
	return true;
}

gtaRwUInt32 gtaRwClump::GetStreamSize()
{
	gtaRwUInt32 size;
	if(gtaRwVersion <= 0x32000)
		size = 40;
	else
		size = 48;
	size += geometryList.GetStreamSize() + frameList.GetStreamSize();
	for(gtaRwInt32 i = 0; i < numAtomics; i++)
		size += atomics[i].GetStreamSize();
	return size;
}

void gtaRwClump::Initialise(gtaRwUInt32 NumAtomics, gtaRwUInt32 NumFrames, gtaRwUInt32 NumGeometries)
{
	memset(this, 0, sizeof(gtaRwClump));
	numAtomics = NumAtomics;
	numLights = 0;
	numCameras = 0;
	frameList.Initialise(NumFrames);
	geometryList.Initialise(NumGeometries);
	atomics = (gtaRwAtomic *)gtaMemAlloc(sizeof(gtaRwAtomic) * NumAtomics);
}

void gtaRwClump::Initialise(gtaRwUInt32 NumAtomics)
{
	memset(this, 0, sizeof(gtaRwClump));
	numAtomics = NumAtomics;
	numLights = 0;
	numCameras = 0;
	atomics = (gtaRwAtomic *)gtaMemAlloc(sizeof(gtaRwAtomic) * NumAtomics);
}

void gtaRwClump::Destroy()
{
	for(gtaRwInt32 i = 0; i < numAtomics; i++)
		atomics[i].Destroy();
	frameList.Destroy();
	geometryList.Destroy();
	memset(this, 0, sizeof(gtaRwClump));
}