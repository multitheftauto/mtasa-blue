#pragma once

#include "UVAnim.h"
#include "StdInc.h"

gtaRwMaterialUVAnim::gtaRwMaterialUVAnim()
{
	memset(this, 0, sizeof(gtaRwMaterialUVAnim));
}

void gtaRwMaterialUVAnim::Initialise(gtaRwUInt32 NumSlots)
{
	memset(this, 0, sizeof(gtaRwMaterialUVAnim));
	enabled = true;
	if(animNames)
		free(animNames);
	if(NumSlots > 8)
		NumSlots = 8;
	numAnimNames = NumSlots;
	if(NumSlots > 0)
	{
		animNames = (gtaRwChar *)malloc(NumSlots * 32);
		memset(animNames, 0, NumSlots * 32);
	}
}

void gtaRwMaterialUVAnim::SetupAnim(gtaRwUInt32 Slot, gtaRwChar *AnimName)
{
	animSlotsMap |= (1 << Slot);
	strcpy(&animNames[32 * currentSlot],  AnimName);
	currentSlot++;
}

void gtaRwMaterialUVAnim::Destroy()
{
	if(animNames)
		free(animNames);
	memset(this, 0, sizeof(gtaRwMaterialUVAnim));
}

bool gtaRwMaterialUVAnim::StreamWrite(gtaRwStream *stream)
{
	if(enabled)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_UVANIM, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
			return false;
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRUCT, GetStreamSize() - 24, gtaRwVersion, gtaRwBuild))
			return false;
		if(!gtaRwStreamWrite(stream, &animSlotsMap, 4))
			return false;
		if(animNames)
		{
			if(!gtaRwStreamWrite(stream, animNames, numAnimNames * 32))
				return false;
		}
	}
	return true;
}

bool gtaRwMaterialUVAnim::StreamRead(gtaRwStream *stream)
{
	memset(this, 0, sizeof(gtaRwMaterialUVAnim));
	if(!gtaRwStreamFindChunk(stream, rwID_STRUCT, NULL, NULL))
		return false;
	if(gtaRwStreamRead(stream, &animSlotsMap, 4) != 4)
		return false;
	for(gtaRwInt32 i = 0; i < 8; i++)
	{
		if(animSlotsMap & (1 << i))
			numAnimNames++;
	}
	if(numAnimNames > 0)
	{
		animNames = (gtaRwChar *)malloc(numAnimNames * 32);
		memset(animNames, 0, numAnimNames * 32);
		if(gtaRwStreamRead(stream, animNames, numAnimNames * 32) != numAnimNames * 32)
		{
			Destroy();
			return false;
		}
	}
	enabled = true;
	return true;
}

unsigned int gtaRwMaterialUVAnim::GetStreamSize()
{
	if(enabled)
		return 28 + numAnimNames * 32;
	return 0;
}