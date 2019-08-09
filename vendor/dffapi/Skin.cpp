#pragma once

#include "Skin.h"
#include "StdInc.h"
#include "Memory.h"

gtaRwUInt8 gtaRwBoneIndices::operator[](gtaRwUInt32 i)
{
	return ai[i];
}
gtaRwReal gtaRwBoneWeights::operator[](gtaRwUInt32 i)
{
	return aw[i];
}
gtaRwUInt8 gtaCompressedBoneWeights::operator[](gtaRwUInt32 i)
{
	return aw[i];
}

gtaRwGeometrySkin::gtaRwGeometrySkin()
{
	memset(this, 0, sizeof(gtaRwGeometrySkin));
	maxNumWeightsPerVertex = 4;
}

void gtaRwGeometrySkin::Initialise(gtaRwUInt8 NumBones, gtaRwUInt8 NumBoneIds, gtaRwUInt32 NumVertices, gtaRwUInt8 MaxNumWeightsPerVertex)
{
	memset(this, 0, sizeof(gtaRwGeometrySkin));
	enabled = true;
	maxNumWeightsPerVertex = MaxNumWeightsPerVertex;
	if(NumBones > 0)
	{
		numBones = NumBones;
		skinToBoneMatrices = (gtaRwMatrix *)gtaMemAlloc(64 * NumBones);
	}
	if(NumVertices > 0)
	{
		vertexBoneIndices = (gtaRwBoneIndices *)gtaMemAlloc(4 * NumVertices);
		vertexBoneWeights = (gtaRwBoneWeights *)gtaMemAlloc(16 * NumVertices);
	}
	if(NumBoneIds > 0)
	{
		numBoneIds = NumBoneIds;
		boneIds = (gtaRwUInt8 *)gtaMemAlloc(NumBoneIds);
	}
}

void gtaRwGeometrySkin::Destroy()
{
	if(boneIds)
		gtaMemFree(boneIds);
	if(vertexBoneIndices)
		gtaMemFree(vertexBoneIndices);
	if(vertexBoneWeights)
		gtaMemFree(vertexBoneWeights);
	if(skinToBoneMatrices)
		gtaMemFree(skinToBoneMatrices);
	if(ogl.skinToBoneMatrices)
		gtaMemFree(ogl.skinToBoneMatrices);
	memset(this, 0, sizeof(gtaRwGeometrySkin));
	maxNumWeightsPerVertex = 4;
}

bool gtaRwGeometrySkin::StreamWrite(gtaRwStream *Stream, gtaRwUInt32 NumVertices, gtaRwBool IsNative)
{
	if(enabled)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_SKIN, GetStreamSize(IsNative, NumVertices) - 12, gtaRwVersion, gtaRwBuild))
			return false;
		if(IsNative)
		{
			// write native data...
			switch(gtaPlatform)
			{
			case PLATFORM_D3D9:
				if(d3d9.generated)
				{
					if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_STRUCT, 8, gtaRwVersion, gtaRwBuild))
						return false;
					gtaRwUInt32 platform = 9;
					if(!gtaRwStreamWrite(Stream, &platform, 4))
						return false;
					if(!gtaRwStreamWrite(Stream, &d3d9.numBones, 4))
						return false;
				}
				else
					return false;
				break;
			case PLATFORM_OGL:
				if(ogl.generated)
				{
					if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_STRUCT, 8 + numBones * 64, gtaRwVersion, gtaRwBuild))
						return false;
					gtaRwUInt32 platform = 2;
					if(!gtaRwStreamWrite(Stream, &platform, 4))
						return false;
					if(!gtaRwStreamWrite(Stream, &ogl.numBones, 4))
						return false;
					if(numBones > 0)
					{
						if(!gtaRwStreamWrite(Stream, ogl.skinToBoneMatrices, ogl.numBones * 64))
							return false;
					}
				}
				else
					return false;
				break;
			default:
				gtaRwErrorSet("Wiriting Native Skin data: this platform Id (%d) is not supported", gtaPlatform);
				return false;
			}
		}
		else
		{
			if(!gtaRwStreamWrite(Stream, &numBones, 4))
				return false;
			if(numBoneIds > 0 && maxNumWeightsPerVertex > 0)
			{
				if(!gtaRwStreamWrite(Stream, boneIds, numBoneIds))
					return false;
			}
			if(NumVertices > 0)
			{
				if(!gtaRwStreamWrite(Stream, vertexBoneIndices, NumVertices * 4))
					return false;
				if(!gtaRwStreamWrite(Stream, vertexBoneWeights, NumVertices * 16))
					return false;
			}
			if(numBones > 0)
			{
				if(maxNumWeightsPerVertex > 0)
				{
					if(!gtaRwStreamWrite(Stream, skinToBoneMatrices, numBones * 64))
						return false;
					if(!gtaRwStreamWrite(Stream, &boneLimit, 12))
						return false;
					if(numMeshes > 0)
					{
						if(!gtaRwStreamWrite(Stream, meshBoneRemapIndices, numBones + 2 * (numRLE + numMeshes)))
							return false;
					}
				}
				else
				{
					gtaRwUInt32 zero = 0;
					for(gtaRwInt32 i = 0; i < numBones; i++)
					{
						if(!gtaRwStreamWrite(Stream, &zero, 4))
							return false;
						if(!gtaRwStreamWrite(Stream, &skinToBoneMatrices[i], 64))
							return false;
					}
				}
			}
		}
	}
	return true;
}

bool gtaRwGeometrySkin::StreamRead(gtaRwStream *Stream, gtaRwUInt32 NumVertices, gtaRwBool IsNative)
{
	memset(this, 0, sizeof(gtaRwGeometrySkin));
	enabled = true;
	if(IsNative)
	{
		gtaRwUInt32 platform;
		if(!gtaRwStreamFindChunk(Stream, rwID_STRUCT, NULL, NULL))
			return false;
		if(gtaRwStreamRead(Stream, &platform, 4) != 4)
			return false;
		// read native data...
		switch(platform)
		{
		case rwID_PCD3D9:
			if(gtaRwStreamRead(Stream, &d3d9.numBones, 4) != 4)
				return false;
			d3d9.generated = true;
			break;
		case rwID_PCOGL:
			if(gtaRwStreamRead(Stream, &ogl.numBones, 4) != 4)
				return false;
			if(ogl.numBones > 0)
			{
				ogl.skinToBoneMatrices = (gtaRwMatrix *)gtaMemAlloc(64 * ogl.numBones);
				if(gtaRwStreamRead(Stream, ogl.skinToBoneMatrices, ogl.numBones * 64) != ogl.numBones * 64)
				{
					Destroy();
					return false;
				}
			}
			ogl.generated = true;
			break;
		default:
			gtaRwErrorSet("Reading Native Skin data: Unknown platform Id (%d)", platform);
			return false;
		}
	}
	else
	{
		if(gtaRwStreamRead(Stream, &numBones, 4) != 4)
			return false;
		if(numBoneIds > 0 && maxNumWeightsPerVertex > 0)
		{
			boneIds = (gtaRwUInt8 *)gtaMemAlloc(numBoneIds);
			if(gtaRwStreamRead(Stream, boneIds, numBoneIds) != numBoneIds)
			{
				Destroy();
				return false;
			}
		}
		if(NumVertices > 0)
		{
			vertexBoneIndices = (gtaRwBoneIndices *)gtaMemAlloc(4 * NumVertices);
			if(gtaRwStreamRead(Stream, vertexBoneIndices, NumVertices * 4) != NumVertices * 4)
			{
				Destroy();
				return false;
			}
			vertexBoneWeights = (gtaRwBoneWeights *)gtaMemAlloc(16 * NumVertices);
			if(gtaRwStreamRead(Stream, vertexBoneWeights, NumVertices * 16) != NumVertices * 16)
			{
				Destroy();
				return false;
			}
		}
		if(numBones > 0)
		{
			skinToBoneMatrices = (gtaRwMatrix *)gtaMemAlloc(64 * numBones);
			if(maxNumWeightsPerVertex > 0)
			{
				if(gtaRwStreamRead(Stream, skinToBoneMatrices, numBones * 64) != numBones * 64)
				{
					Destroy();
					return false;
				}
				if(gtaRwStreamRead(Stream, &boneLimit, 12) != 12)
				{
					Destroy();
					return false;
				}
				if(numMeshes > 0)
				{
					if(gtaRwStreamRead(Stream, meshBoneRemapIndices, numBones + 2 * (numRLE + numMeshes)) != 
						numBones + 2 * (numRLE + numMeshes))
					{
						Destroy();
						return false;
					}
				}
			}
			else
			{
				for(gtaRwInt32 i = 0; i < numBones; i++)
				{
					if(!gtaRwStreamSkip(Stream, 4))
					{
						Destroy();
						return false;
					}
					if(gtaRwStreamRead(Stream, &skinToBoneMatrices[i], 64) != 64)
					{
						Destroy();
						return false;
					}
				}
			}
		}
	}
	return true;
}

unsigned int gtaRwGeometrySkin::GetStreamSize(gtaRwBool IsNative, gtaRwUInt32 NumVertices)
{
	if(enabled)
	{
		if(!IsNative)
		{
			gtaRwUInt32 size = 16;
			if(maxNumWeightsPerVertex > 0)
				size += numBoneIds;
			size += NumVertices * 4;
			size += NumVertices * 16;
			if(numBones > 0)
			{
				if(maxNumWeightsPerVertex > 0)
				{
					size += 12 + numBones * 64;
					if(numMeshes > 0)
						size += numBones + 2 * (numRLE + numMeshes);
				}
				else
					size += numBones * 70;
			}
			return size;
		}
		else
		{
			// Calculate native size
			switch(gtaPlatform)
			{
			case PLATFORM_D3D9:
				if(d3d9.generated)
					return 30;
				break;
			case PLATFORM_OGL:
				if(ogl.generated)
					return 30 + ogl.numBones * 64;
				break;
			}
			return 0;
		}
		return 20;
	}
	return 0;
}

void gtaRwGeometrySkin::FindUsedBoneIds(gtaRwUInt32 NumVertices, gtaRwInt32 NumBones)
{
	if(boneIds)
		gtaMemFree(boneIds);
	numBoneIds = 0;
	numBones = NumBones;
	if(vertexBoneWeights && vertexBoneIndices && numBones > 0)
	{
		maxNumWeightsPerVertex = 0;
		// Find max number of weights in all vertices
		for(gtaRwInt32 i = 0; i < NumVertices; i++)
		{
			// for all weights
			for(gtaRwInt32 j = 0; j < 4; j++)
			{
				if(vertexBoneWeights[i].aw[j] == 0.0f)
					break;
				maxNumWeightsPerVertex++;
			}
			// if we already reached the limit
			if(maxNumWeightsPerVertex == 4)
				break;
		}
		// Find all unique boneIds
		gtaRwUInt8 *myBoneIds = (gtaRwUInt8 *)gtaMemAlloc(numBones);
		for(gtaRwInt32 i = 0; i < NumVertices; i++)
		{
			// for all indices
			for(gtaRwInt32 j = 0; j < maxNumWeightsPerVertex; j++)
			{
				if(vertexBoneWeights[i].aw[j] == 0.0f)
					break;
				// check if this index was not already found and if not - add it.
				gtaRwBool indexAlreadyFound = false;
				for(gtaRwInt32 k = 0; k < numBoneIds; k++)
				{
					if(myBoneIds[k] == vertexBoneIndices[i].ai[j])
					{
						indexAlreadyFound = true;
						break;
					}
				}
				if(!indexAlreadyFound)
				{
					myBoneIds[numBoneIds] = vertexBoneIndices[i].ai[j];
					numBoneIds++;
				}
			}
			// if we already reached the limit
			if(numBoneIds == numBones)
				break;
		}
		// now copy our data
		if(numBoneIds > 0)
		{
			boneIds = (gtaRwUInt8 *)gtaMemAlloc(4 * numBoneIds);
			gtaMemCopy(boneIds, myBoneIds, 4 * numBoneIds);
		}
		gtaMemFree(myBoneIds);
	}
}