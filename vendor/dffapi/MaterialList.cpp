#pragma once

#include "MaterialList.h"

gtaRwMaterialList::gtaRwMaterialList()
{
	memset(this, 0, sizeof(gtaRwMaterialList));
}

bool gtaRwMaterialList::StreamRead(gtaRwStream *stream)
{
	memset(this, 0, sizeof(gtaRwMaterialList));
	if(!gtaRwStreamFindChunk(stream, rwID_MATLIST, NULL, NULL))
		return false;
	if(!gtaRwStreamFindChunk(stream, rwID_STRUCT, NULL, NULL))
		return false;
	if(gtaRwStreamRead(stream, &materialCount, 4) != 4)
		return false;
	if(materialCount > 0)
	{
		materialValues = (gtaRwInt32 *)malloc(materialCount * 4);
		memset(materialValues, 0xFFFFFFFF, materialCount * 4);
		if(gtaRwStreamRead(stream, materialValues, materialCount * 4) != materialCount * 4)
		{
			Destroy();
			return false;
		}
		materials = (gtaRwMaterial *)malloc(materialCount * sizeof(gtaRwMaterial));
		memset(materials, 0, materialCount * sizeof(gtaRwMaterial));
		for(gtaRwInt32 i = 0; i < materialCount; i++)
		{
			if(!materials[i].StreamRead(stream))
			{
				Destroy();
				return false;
			}
		}
	}
	return true;
}

bool gtaRwMaterialList::StreamWrite(gtaRwStream *stream)
{
	if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_MATLIST, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
		return false;
	if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRUCT, 4 + materialCount * 4, gtaRwVersion, gtaRwBuild))
		return false;
	if(!gtaRwStreamWrite(stream, &materialCount, 4))
		return false;
	if(materialCount > 0)
	{
		if(!gtaRwStreamWrite(stream, materialValues, materialCount * 4))
			return false;
		for(gtaRwInt32 i = 0; i < materialCount; i++)
		{
			if(!materials[i].StreamWrite(stream))
				return false;
		}
	}
	return true;
}

unsigned int gtaRwMaterialList::GetStreamSize()
{
	unsigned int size = 28 + 4 * materialCount;
	for(gtaRwInt32 i = 0; i < materialCount; i++)
		size += materials[i].GetStreamSize();
	return size;
}

void gtaRwMaterialList::Initialise(gtaRwUInt32 MaterialCount)
{
	memset(this, 0, sizeof(gtaRwMaterialList));
	materialCount = MaterialCount;
	if(MaterialCount > 0)
	{
		materialValues = (gtaRwInt32 *)malloc(4 * MaterialCount);
		memset(materialValues, 0xFFFFFFFF, 4 * MaterialCount);
		materials = (gtaRwMaterial *)malloc(sizeof(gtaRwMaterial) * MaterialCount);
		memset(materials, 0, sizeof(gtaRwMaterial) * MaterialCount);
	}
}

void gtaRwMaterialList::Destroy()
{
	if(materialValues)
		free(materialValues);
	if(materials)
	{
		for(gtaRwInt32 i = 0; i < materialCount; i++)
			materials[i].Destroy();
		free(materials);
	}
	memset(this, 0, sizeof(gtaRwMaterialList));
}