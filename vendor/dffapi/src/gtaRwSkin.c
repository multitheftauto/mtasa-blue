#include "StdInc.h"

void gtaRwSkinInitialise(gtaRwGeometrySkin* skinObj, gtaRwUInt8 NumBones, gtaRwUInt8 NumBoneIds, gtaRwUInt32 NumVertices, gtaRwUInt8 MaxNumWeightsPerVertex)
{
    memset(skinObj, 0, sizeof(gtaRwGeometrySkin));
    skinObj->enabled = rwTRUE;
    skinObj->maxNumWeightsPerVertex = MaxNumWeightsPerVertex;
	if(NumBones > 0)
	{
        skinObj->numBones = NumBones;
        skinObj->skinToBoneMatrices = (gtaRwMatrix*)gtaRwMemAlloc(64 * NumBones);
	}
	if(NumVertices > 0)
	{
        skinObj->vertexBoneIndices = (gtaRwBoneIndices*)gtaRwMemAlloc(4 * NumVertices);
        skinObj->vertexBoneWeights = (gtaRwBoneWeights*)gtaRwMemAlloc(16 * NumVertices);
	}
	if(NumBoneIds > 0)
	{
        skinObj->numBoneIds = NumBoneIds;
        skinObj->boneIds = (gtaRwUInt8*)gtaRwMemAlloc(NumBoneIds);
	}
}

void gtaRwSkinDestroy(gtaRwGeometrySkin* skinObj)
{
    if (skinObj->boneIds)
        gtaRwMemFree(skinObj->boneIds);
    if (skinObj->vertexBoneIndices)
        gtaRwMemFree(skinObj->vertexBoneIndices);
    if (skinObj->vertexBoneWeights)
        gtaRwMemFree(skinObj->vertexBoneWeights);
    if (skinObj->skinToBoneMatrices)
        gtaRwMemFree(skinObj->skinToBoneMatrices);
    if (skinObj->ogl.skinToBoneMatrices)
        gtaRwMemFree(skinObj->ogl.skinToBoneMatrices);
	memset(skinObj, 0, sizeof(gtaRwGeometrySkin));
    skinObj->maxNumWeightsPerVertex = 4;
}

gtaRwBool gtaRwSkinStreamWrite(gtaRwGeometrySkin* skinObj, gtaRwStream* Stream, gtaRwUInt32 NumVertices, gtaRwBool IsNative)
{
    if (skinObj->enabled)
	{
        if (!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_SKIN, gtaRwSkinGetStreamSize(skinObj, IsNative, NumVertices) - 12, gtaRwVersion, gtaRwBuild))
			return rwTRUE;
		if(IsNative)
		{
			// write native data...
			switch(gtaPlatform)
			{
			case PLATFORM_D3D9:
                    if (skinObj->d3d9.generated)
				{
					if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_STRUCT, 8, gtaRwVersion, gtaRwBuild))
						return rwFALSE;
					gtaRwUInt32 platform = 9;
					if(!gtaRwStreamWrite(Stream, &platform, 4))
						return rwFALSE;
                    if (!gtaRwStreamWrite(Stream, &skinObj->d3d9.numBones, 4))
						return rwFALSE;
				}
				else
					return rwFALSE;
				break;
			case PLATFORM_OGL:
                if (skinObj->ogl.generated)
				{
                    if (!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_STRUCT, 8 + skinObj->numBones * 64, gtaRwVersion, gtaRwBuild))
						return rwFALSE;
					gtaRwUInt32 platform = 2;
					if(!gtaRwStreamWrite(Stream, &platform, 4))
						return rwFALSE;
                    if (!gtaRwStreamWrite(Stream, &skinObj->ogl.numBones, 4))
						return rwFALSE;
                    if (skinObj->numBones > 0)
					{
                        if (!gtaRwStreamWrite(Stream, skinObj->ogl.skinToBoneMatrices, skinObj->ogl.numBones * 64))
							return rwFALSE;
					}
				}
				else
					return rwFALSE;
				break;
			default:
				gtaRwErrorSet("Wiriting Native Skin data: this platform Id (%d) is not supported", gtaPlatform);
				return rwFALSE;
			}
		}
		else
		{
            if (!gtaRwStreamWrite(Stream, &skinObj->numBones, 4))
				return rwFALSE;
            if (skinObj->numBoneIds > 0 && skinObj->maxNumWeightsPerVertex > 0)
			{
                if (!gtaRwStreamWrite(Stream, skinObj->boneIds, skinObj->numBoneIds))
					return rwFALSE;
			}
			if(NumVertices > 0)
			{
                if (!gtaRwStreamWrite(Stream, skinObj->vertexBoneIndices, NumVertices * 4))
					return rwFALSE;
                if (!gtaRwStreamWrite(Stream, skinObj->vertexBoneWeights, NumVertices * 16))
					return rwFALSE;
			}
            if (skinObj->numBones > 0)
			{
                if (skinObj->maxNumWeightsPerVertex > 0)
				{
                    if (!gtaRwStreamWrite(Stream, skinObj->skinToBoneMatrices, skinObj->numBones * 64))
						return rwFALSE;
                    if (!gtaRwStreamWrite(Stream, &skinObj->boneLimit, 12))
						return rwFALSE;
                    if (skinObj->numMeshes > 0)
					{
                        if (!gtaRwStreamWrite(Stream, skinObj->meshBoneRemapIndices, skinObj->numBones + 2 * (skinObj->numRLE + skinObj->numMeshes)))
							return rwFALSE;
					}
				}
				else
				{
					gtaRwUInt32 zero = 0;
                    for (gtaRwInt32 i = 0; i < skinObj->numBones; i++)
					{
						if(!gtaRwStreamWrite(Stream, &zero, 4))
							return rwFALSE;
                        if (!gtaRwStreamWrite(Stream, &skinObj->skinToBoneMatrices[i], 64))
							return rwFALSE;
					}
				}
			}
		}
	}
	return rwTRUE;
}

gtaRwBool gtaRwSkinStreamRead(gtaRwGeometrySkin* skinObj, gtaRwStream* Stream, gtaRwUInt32 NumVertices, gtaRwBool IsNative)
{
	memset(skinObj, 0, sizeof(gtaRwGeometrySkin));
    skinObj->enabled = rwTRUE;
	if(IsNative)
	{
		gtaRwUInt32 platform;
		if(!gtaRwStreamFindChunk(Stream, rwID_STRUCT, NULL, NULL, NULL))
			return rwFALSE;
		if(gtaRwStreamRead(Stream, &platform, 4) != 4)
			return rwFALSE;
		// read native data...
		switch(platform)
		{
		case rwID_PCD3D9:
                if (gtaRwStreamRead(Stream, &skinObj->d3d9.numBones, 4) != 4)
				return rwFALSE;
                skinObj->d3d9.generated = rwTRUE;
			break;
		case rwID_PCOGL:
            if (gtaRwStreamRead(Stream, &skinObj->ogl.numBones, 4) != 4)
				return rwFALSE;
            if (skinObj->ogl.numBones > 0)
			{
                skinObj->ogl.skinToBoneMatrices = (gtaRwMatrix*)gtaRwMemAlloc(64 * skinObj->ogl.numBones);
                if (gtaRwStreamRead(Stream, skinObj->ogl.skinToBoneMatrices, skinObj->ogl.numBones * 64) != skinObj->ogl.numBones * 64)
				{
                    gtaRwSkinDestroy(skinObj);
					return rwFALSE;
				}
			}
            skinObj->ogl.generated = rwTRUE;
			break;
		default:
			gtaRwErrorSet("Reading Native Skin data: Unknown platform Id (%d)", platform);
			return rwFALSE;
		}
	}
	else
	{
        if (gtaRwStreamRead(Stream, &skinObj->numBones, 4) != 4)
			return rwFALSE;
        if (skinObj->numBoneIds > 0 && skinObj->maxNumWeightsPerVertex > 0)
		{
            skinObj->boneIds = (gtaRwUInt8*)gtaRwMemAlloc(skinObj->numBoneIds);
            if (gtaRwStreamRead(Stream, skinObj->boneIds, skinObj->numBoneIds) != skinObj->numBoneIds)
            {
                gtaRwSkinDestroy(skinObj);
				return rwFALSE;
			}
		}
		if(NumVertices > 0)
		{
            skinObj->vertexBoneIndices = (gtaRwBoneIndices*)gtaRwMemAlloc(4 * NumVertices);
            if (gtaRwStreamRead(Stream, skinObj->vertexBoneIndices, NumVertices * 4) != NumVertices * 4)
            {
                gtaRwSkinDestroy(skinObj);
				return rwFALSE;
			}
            skinObj->vertexBoneWeights = (gtaRwBoneWeights*)gtaRwMemAlloc(16 * NumVertices);
            if (gtaRwStreamRead(Stream, skinObj->vertexBoneWeights, NumVertices * 16) != NumVertices * 16)
            {
                gtaRwSkinDestroy(skinObj);
				return rwFALSE;
			}
		}
        if (skinObj->numBones > 0)
		{
            skinObj->skinToBoneMatrices = (gtaRwMatrix*)gtaRwMemAlloc(64 * skinObj->numBones);
            if (skinObj->maxNumWeightsPerVertex > 0)
			{
                if (gtaRwStreamRead(Stream, skinObj->skinToBoneMatrices, skinObj->numBones * 64) != skinObj->numBones * 64)
                {
                    gtaRwSkinDestroy(skinObj);
					return rwFALSE;
				}
                if (gtaRwStreamRead(Stream, &skinObj->boneLimit, 12) != 12)
                {
                    gtaRwSkinDestroy(skinObj);
					return rwFALSE;
				}
                if (skinObj->numMeshes > 0)
				{
                    if (gtaRwStreamRead(Stream, skinObj->meshBoneRemapIndices, skinObj->numBones + 2 * (skinObj->numRLE + skinObj->numMeshes)) != 
						skinObj->numBones + 2 * (skinObj->numRLE + skinObj->numMeshes))
                    {
                        gtaRwSkinDestroy(skinObj);
						return rwFALSE;
					}
				}
			}
			else
			{
                for (gtaRwInt32 i = 0; i < skinObj->numBones; i++)
				{
					if(!gtaRwStreamSkip(Stream, 4))
                    {
                        gtaRwSkinDestroy(skinObj);
						return rwFALSE;
					}
                    if (gtaRwStreamRead(Stream, &skinObj->skinToBoneMatrices[i], 64) != 64)
                    {
                        gtaRwSkinDestroy(skinObj);
						return rwFALSE;
					}
				}
			}
		}
	}
	return rwTRUE;
}

unsigned int gtaRwSkinGetStreamSize(gtaRwGeometrySkin* skinObj, gtaRwBool IsNative, gtaRwUInt32 NumVertices)
{
    if (skinObj->enabled)
	{
		if(!IsNative)
		{
			gtaRwUInt32 size = 16;
            if (skinObj->maxNumWeightsPerVertex > 0)
                size += skinObj->numBoneIds;
			size += NumVertices * 4;
			size += NumVertices * 16;
            if (skinObj->numBones > 0)
			{
                if (skinObj->maxNumWeightsPerVertex > 0)
				{
                    size += 12 + skinObj->numBones * 64;
                    if (skinObj->numMeshes > 0)
                        size += skinObj->numBones + 2 * (skinObj->numRLE + skinObj->numMeshes);
				}
				else
                    size += skinObj->numBones * 70;
			}
			return size;
		}
		else
		{
			// Calculate native size
			switch(gtaPlatform)
			{
			case PLATFORM_D3D9:
                    if (skinObj->d3d9.generated)
					return 30;
				break;
			case PLATFORM_OGL:
                if (skinObj->ogl.generated)
                    return 30 + skinObj->ogl.numBones * 64;
				break;
			}
			return 0;
		}
		return 20;
	}
	return 0;
}

void gtaRwSkinFindUsedBoneIds(gtaRwGeometrySkin* skinObj, gtaRwUInt32 NumVertices, gtaRwInt32 NumBones)
{
    if (skinObj->boneIds)
        gtaRwMemFree(skinObj->boneIds);
    skinObj->numBoneIds = 0;
    skinObj->numBones = NumBones;
    if (skinObj->vertexBoneWeights && skinObj->vertexBoneIndices && skinObj->numBones > 0)
	{
        skinObj->maxNumWeightsPerVertex = 0;
		// Find max number of weights in all vertices
		for(gtaRwInt32 i = 0; i < NumVertices; i++)
		{
			// for all weights
			for(gtaRwInt32 j = 0; j < 4; j++)
			{
                if (skinObj->vertexBoneWeights[i].aw[j] == 0.0f)
					break;
                skinObj->maxNumWeightsPerVertex++;
			}
			// if we already reached the limit
            if (skinObj->maxNumWeightsPerVertex == 4)
				break;
		}
		// Find all unique boneIds
        gtaRwUInt8* myBoneIds = (gtaRwUInt8*)gtaRwMemAlloc(skinObj->numBones);
		for(gtaRwInt32 i = 0; i < NumVertices; i++)
		{
			// for all indices
            for (gtaRwInt32 j = 0; j < skinObj->maxNumWeightsPerVertex; j++)
			{
                if (skinObj->vertexBoneWeights[i].aw[j] == 0.0f)
					break;
				// check if this index was not already found and if not - add it.
				gtaRwBool indexAlreadyFound = rwFALSE;
                for (gtaRwInt32 k = 0; k < skinObj->numBoneIds; k++)
				{
                    if (myBoneIds[k] == skinObj->vertexBoneIndices[i].ai[j])
					{
						indexAlreadyFound = rwTRUE;
						break;
					}
				}
				if(!indexAlreadyFound)
				{
                    myBoneIds[skinObj->numBoneIds] = skinObj->vertexBoneIndices[i].ai[j];
                    skinObj->numBoneIds++;
				}
			}
			// if we already reached the limit
            if (skinObj->numBoneIds == skinObj->numBones)
				break;
		}
		// now copy our data
        if (skinObj->numBoneIds > 0)
		{
            skinObj->boneIds = (gtaRwUInt8*)gtaRwMemAlloc(4 * skinObj->numBoneIds);
            gtaRwMemCopy(skinObj->boneIds, myBoneIds, 4 * skinObj->numBoneIds);
		}
        gtaRwMemFree(myBoneIds);
	}
}