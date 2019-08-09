#pragma once

#include "Geometry.h"
#include "Memory.h"

gtaRwUInt8 gtaRwGeometry::GetTexCoordsCount()
{
	gtaRwUInt8 texCoordsCount = 0;
	if(numTexCoordSets > 0)
		texCoordsCount = numTexCoordSets;
	else
	{
		if(textured2)
			texCoordsCount = 2;
		else if(textured)
			texCoordsCount = 1;
	}
	return texCoordsCount;
}

void gtaRwGeometry::Initialise(gtaRwInt32 NumTriangles, gtaRwInt32 NumVertices, gtaRwInt32 NumMorphTargets, gtaRwInt32 Format, gtaRwUInt8 NumTexCoordsCustom,
	gtaRwReal CenterX, gtaRwReal CenterY, gtaRwReal CenterZ, gtaRwReal Radius)
{
	memset(this, 0, sizeof(gtaRwGeometry));
	format = Format;
	numTriangles = NumTriangles;
	numVertices = NumVertices;
	numMorphTargets = NumMorphTargets;
	numTexCoordSets = NumTexCoordsCustom;
	if(!native)
	{
		if(NumTriangles > 0)
		{
			triangles = (gtaRwTriangle *)gtaMemAlloc(sizeof(gtaRwTriangle) * NumTriangles);
			for(gtaRwInt32 i = 0; i < NumTriangles; i++)
				triangles[i].mtlId = -1;
		}
		if(NumVertices > 0)
		{
			if(prelit)
				preLitLum = (gtaRwRGBA *)gtaMemAlloc(sizeof(gtaRwRGBA) * NumVertices);
			gtaRwUInt8 texCoordsCount = GetTexCoordsCount();
			for(gtaRwInt8 i = 0; i < texCoordsCount; i++)
				texCoords[i] = (gtaRwTexCoords *)gtaMemAlloc(sizeof(gtaRwTexCoords) * NumVertices);
		}
	}
	if(NumMorphTargets > 0)
	{
		morphTarget = (gtaRwMorphTarget *)gtaMemAlloc(sizeof(gtaRwMorphTarget) * NumMorphTargets);
		for(gtaRwInt8 i = 0; i < NumMorphTargets; i++)
		{
			morphTarget[i].boundingSphere.center.x = CenterX;
			morphTarget[i].boundingSphere.center.y = CenterY;
			morphTarget[i].boundingSphere.center.z = CenterZ;
			morphTarget[i].boundingSphere.radius = Radius;
		}
		if(!native && NumVertices > 0)
		{
			if(positions)
			{
				for(gtaRwInt8 i = 0; i < NumMorphTargets; i++)
				{
					morphTarget[i].hasVerts = TRUE;
					morphTarget[i].verts = (gtaRwV3d *)gtaMemAlloc(sizeof(gtaRwV3d) * NumVertices);
				}
			}
			if(normals)
			{
				for(gtaRwInt8 i = 0; i < NumMorphTargets; i++)
				{
					morphTarget[i].hasNormals = TRUE;
					morphTarget[i].normals = (gtaRwV3d *)gtaMemAlloc(sizeof(gtaRwV3d) * NumVertices);
				}
			}
		}
	}
	geometryLighting.ambient = geometryLighting.diffuse = geometryLighting.specular = 1.0f;
}

void gtaRwGeometry::Destroy()
{
	if(triangles)
		gtaMemFree(triangles);
	if(preLitLum)
		gtaMemFree(preLitLum);
	for(int i = 0; i < 8; i++)
	{
		if(texCoords[i])
			gtaMemFree(texCoords[i]);
	}
	if(morphTarget)
	{
		for(gtaRwInt8 i = 0; i < numMorphTargets; i++)
		{
			if(morphTarget[i].verts)
				gtaMemFree(morphTarget[i].verts);
			if(morphTarget[i].normals)
				gtaMemFree(morphTarget[i].normals);
		}
		free(morphTarget);
	}
	matList.Destroy();
	Extension.extraColour.Destroy();
	Extension.mesh.Destroy();
	Extension.effect2d.Destroy();
	Extension.skin.Destroy();
	Extension.breakable.Destroy();
	gtaMemZero(this, sizeof(gtaRwGeometry));
}

gtaRwGeometry::gtaRwGeometry()
{
	gtaMemZero(this, sizeof(gtaRwGeometry));
}

gtaRwBool gtaRwGeometry::StreamRead(gtaRwStream *stream, gtaRwUInt32 ClumpVersion)
{
	gtaRwUInt32 length, entryLength, type;
	memset(this, 0, sizeof(gtaRwGeometry));
	if(!gtaRwStreamFindChunk(stream, rwID_GEOMETRY, NULL, NULL))
		return false;
	if(!gtaRwStreamFindChunk(stream, rwID_STRUCT, NULL, NULL))
		return false;
	if(gtaRwStreamRead(stream, &format, 16) != 16)
		return false;
	if(ClumpVersion < 0x34003)
	{
		if(gtaRwStreamRead(stream, &geometryLighting, 12) != 12)
			return false;
	}
	if(numVertices > 65536)
		gtaRwErrorSet("Warning: geometry has more than 65536 vertices (%d vertices)", numVertices);
	if(!native)
	{
		if(numVertices > 0)
		{
			if(prelit)
			{
				preLitLum = (gtaRwRGBA *)gtaMemAlloc(4 * numVertices);
				if(gtaRwStreamRead(stream, preLitLum, 4 * numVertices) != 4 * numVertices)
				{
					Destroy();
					return false;
				}
			}
			gtaRwUInt8 texCoordsCount = GetTexCoordsCount();
			for(gtaRwInt8 i = 0; i < texCoordsCount; i++)
			{
				texCoords[i] = (gtaRwTexCoords *)gtaMemAlloc(8 * numVertices);
				if(gtaRwStreamRead(stream, texCoords[i], 8 * numVertices) != 8 * numVertices)
				{
					Destroy();
					return false;
				}
			}
		}
		if(numTriangles > 0)
		{
			triangles = (gtaRwTriangle *)gtaMemAlloc(8 * numTriangles);
			if(gtaRwStreamRead(stream, triangles, 8 * numTriangles) != 8 * numTriangles)
			{
				Destroy();
				return false;
			}
		}
	}
	if(numMorphTargets > 0)
	{
		morphTarget = (gtaRwMorphTarget *)gtaMemAlloc(sizeof(gtaRwMorphTarget) * numMorphTargets);
		for(gtaRwInt8 i = 0; i < numMorphTargets; i++)
		{
			if(gtaRwStreamRead(stream, &morphTarget[i], 24) != 24)
			{
				Destroy();
				return false;
			}
			if(morphTarget[i].hasVerts)
			{
				morphTarget[i].verts = (gtaRwV3d *)gtaMemAlloc(12 * numVertices);
				if(gtaRwStreamRead(stream, morphTarget[i].verts, 12 * numVertices) != 12 * numVertices)
				{
					Destroy();
					return false;
				}
			}
			else
				morphTarget[i].verts = NULL;
			if(morphTarget[i].hasNormals)
			{
				morphTarget[i].normals = (gtaRwV3d *)gtaMemAlloc(12 * numVertices);
				if(gtaRwStreamRead(stream, morphTarget[i].normals, 12 * numVertices) != 12 * numVertices)
				{
					Destroy();
					return false;
				}
			}
			else
				morphTarget[i].normals = NULL;
		}
	}
	if(!matList.StreamRead(stream))
	{
		Destroy();
		return false;
	}
	if(!gtaRwStreamFindChunk(stream, rwID_EXTENSION, &length, NULL))
	{
		Destroy();
		return false;
	}
	while(length && gtaRwStreamReadChunkHeader(stream, &type, &entryLength, NULL, NULL))
	{
		gtaRwUInt32 dummy_el, dummy_sz;
		switch(type)
		{
		case rwID_BINMESH:
			if(!Extension.mesh.StreamRead(stream, native))
			{
				Destroy();
				return false;
			}
			break;
		case rwID_NATIVEDATA:
			if(!Extension.native.StreamRead(stream, numVertices))
			{
				Destroy();
				return false;
			}
			break;
		case rwID_SKIN:
			if(!Extension.skin.StreamRead(stream, numVertices, native))
			{
				Destroy();
				return false;
			}
			break;
		case gtaID_EXTRAVERTCOLOUR:
			if(!Extension.extraColour.StreamRead(stream, numVertices))
			{
				Destroy();
				return false;
			}
			break;
		case gtaID_BREAKABLE:
			if(!Extension.breakable.StreamRead(stream))
			{
				Destroy();
				return false;
			}
			break;
		case gtaID_2DEFFECT:
			if(!Extension.effect2d.StreamRead(stream))
			{
				Destroy();
				return false;
			}
			break;
		default:
			if(!gtaRwStreamSkip(stream, entryLength))
			{
				Destroy();
				return false;
			}
		}
		length += -12 - entryLength;
	}
	return true;
}

gtaRwBool gtaRwGeometry::StreamWrite(gtaRwStream *stream)
{
	if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_GEOMETRY, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
		return false;
	if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRUCT, GetStreamActualSize(), gtaRwVersion, gtaRwBuild))
		return false;
	if(!gtaRwStreamWrite(stream, &format, 16))
		return false;
	if(!native && numVertices > 0)
	{
		if(prelit)
		{
			if(!gtaRwStreamWrite(stream, preLitLum, 4 * numVertices))
				return false;
		}
		gtaRwUInt8 texCoordsCount = GetTexCoordsCount();
		for(gtaRwInt8 i = 0; i < texCoordsCount; i++)
		{
			if(texCoords[i])
			{
				if(!gtaRwStreamWrite(stream, texCoords[i], 8 * numVertices))
					return false;
			}
			else
			{
				if(texCoords[0])
				{
					if(!gtaRwStreamWrite(stream, texCoords[0], 8 * numVertices))
						return false;
				}
				else
				{
					gtaRwChar *temporaryTexCoords = (gtaRwChar *)gtaMemAlloc(8 * numVertices);
					if(!gtaRwStreamWrite(stream, temporaryTexCoords, 8 * numVertices))
					{
						gtaMemFree(temporaryTexCoords);
						return false;
					}
					gtaMemFree(temporaryTexCoords);
				}
			}
		}
		if(numTriangles > 0)
		{
			if(!gtaRwStreamWrite(stream, triangles, 8 * numTriangles))
				return false;
		}
	}
	for(gtaRwInt8 i = 0; i < numMorphTargets; i++)
	{
		if(native)
		{
			morphTarget[i].hasVerts = FALSE;
			morphTarget[i].hasNormals = FALSE;
			if(!gtaRwStreamWrite(stream, &morphTarget[i].boundingSphere, 24))
				return false;
		}
		else
		{
			if(morphTarget[i].verts)
				morphTarget[i].hasVerts = TRUE;
			else
				morphTarget[i].hasVerts = FALSE;
			if(morphTarget[i].normals)
				morphTarget[i].hasNormals = TRUE;
			else
				morphTarget[i].hasNormals = FALSE;
			if(!gtaRwStreamWrite(stream, &morphTarget[i].boundingSphere, 24))
				return false;
			if(morphTarget[i].verts)
			{
				if(!gtaRwStreamWrite(stream, morphTarget[i].verts, 12 * numVertices))
					return false;
			}
			if(morphTarget[i].normals)
			{
				if(!gtaRwStreamWrite(stream, morphTarget[i].normals, 12 * numVertices))
					return false;
			}
		}
	}
	if(!matList.StreamWrite(stream))
		return false;
	if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_EXTENSION, Extension.extraColour.GetStreamSize(numVertices) +
		Extension.mesh.GetStreamSize(native) + Extension.effect2d.GetStreamSize() + Extension.native.GetStreamSize(numVertices) +
		Extension.skin.GetStreamSize(native, numVertices) + Extension.breakable.GetStreamSize(), 
		gtaRwVersion, gtaRwBuild))
	{
		return false;
	}
	if(!Extension.mesh.StreamWrite(stream, native))
		return false;
	if(native)
	{
		if(!Extension.native.StreamWrite(stream, numVertices))
			return false;
	}
	if(!Extension.skin.StreamWrite(stream, numVertices, native))
		return false;
	if(!Extension.extraColour.StreamWrite(stream, numVertices))
		return false;
	if(!Extension.breakable.StreamWrite(stream))
		return false;
	if(!Extension.effect2d.StreamWrite(stream))
		return false;
	return true;
}

gtaRwUInt32 gtaRwGeometry::GetStreamActualSize()
{
	gtaRwUInt32 size = 16;
	if(gtaRwVersion < 0x34001)
		size += 12;
	if(!native)
	{
		if(numVertices > 0)
		{
			gtaRwUInt8 texCoordsCount = GetTexCoordsCount();
			if(prelit)
				size += 4 * numVertices;
			size += 8 * (numTriangles + numVertices * texCoordsCount);
		}
	}
	for(gtaRwInt8 i = 0; i < numMorphTargets; i++)
	{
		size += 24;
		if(!native)
		{
			if(morphTarget[i].verts)
				size += 12 * numVertices;
			if(morphTarget[i].normals)
				size += 12 * numVertices;
		}
	}
	return size;
}

gtaRwUInt32 gtaRwGeometry::GetStreamSize()
{
	return 36 + GetStreamActualSize() + matList.GetStreamSize() + Extension.mesh.GetStreamSize(native) + 
		Extension.extraColour.GetStreamSize(numVertices) + Extension.effect2d.GetStreamSize() +
		Extension.native.GetStreamSize(numVertices) + Extension.skin.GetStreamSize(native, numVertices) +
		Extension.breakable.GetStreamSize();
}

gtaRwBool gtaRwGeometry::ConvertFromOGlNative()
{
	if(!native || !Extension.native.ogl.generated || !Extension.native.ogl.numVertexElements || 
		!Extension.native.ogl.vertexBuffer)
		return false;
	gtaRwUInt32 *vertexElementSizes = (gtaRwUInt32 *)gtaMemAlloc(4 * Extension.native.ogl.numVertexElements);
	gtaRwBool hasPositions = false, hasNormals = false, hasColors = false, hasExtraColors = false, hasTexCoords = false, 
		hasBoneWeights = false, hasBoneIndices = false;
	for(gtaRwInt32 i = Extension.native.ogl.numVertexElements - 1; i >= 0; i--)
	{
		gtaRwUInt32 previousOffset;
		if(i == Extension.native.ogl.numVertexElements - 1)
			previousOffset = Extension.native.ogl.vertexElements[i].stride;
		else
			previousOffset = Extension.native.ogl.vertexElements[i + 1].offset;
		vertexElementSizes[i] = previousOffset - Extension.native.ogl.vertexElements[i].offset;
	}

	positions = false;
	normals = false;
	numTexCoordSets = 0;
	textured = false;
	textured2 = false;
	prelit = false;
	for(gtaRwInt32 i = 0; i < numMorphTargets; i++)
	{
		morphTarget[i].hasVerts = false;
		morphTarget[i].hasNormals = false;
		if(morphTarget[i].verts)
			gtaMemFree(morphTarget[i].verts);
		if(morphTarget[i].normals)
			gtaMemFree(morphTarget[i].normals);
	}
	for(gtaRwInt32 i = 0; i < 8; i++)
	{
		if(texCoords[i])
			gtaMemFree(texCoords[i]);
	}
	if(preLitLum)
		gtaMemFree(preLitLum);
	if(Extension.skin.enabled)
	{
		if(Extension.skin.vertexBoneIndices)
			gtaMemFree(Extension.skin.vertexBoneIndices);
		if(Extension.skin.vertexBoneWeights)
			gtaMemFree(Extension.skin.vertexBoneWeights);
		Extension.skin.vertexBoneIndices = (gtaRwBoneIndices *)gtaMemAlloc(numVertices * 4);
		Extension.skin.vertexBoneWeights = (gtaRwBoneWeights *)gtaMemAlloc(numVertices * 16);
		if(Extension.skin.skinToBoneMatrices)
			gtaMemFree(Extension.skin.skinToBoneMatrices);
	}

	for(gtaRwInt32 i = 0; i < Extension.native.ogl.numVertexElements; i++)
	{
		switch(Extension.native.ogl.vertexElements[i].paramType)
		{
		case OGL_VERTEX_POSITION:
			positions = true;
			for(gtaRwInt32 mt = 0; mt < numMorphTargets; mt++)
			{
				morphTarget[mt].hasVerts = true;
				morphTarget[mt].verts = (gtaRwV3d *)gtaMemAlloc(numVertices * 12);
				for(gtaRwInt32 vertex = 0; vertex < numVertices; vertex++)
				{
					gtaRwV3d *position = (gtaRwV3d *)((gtaRwUInt32)Extension.native.ogl.vertexBuffer +
						Extension.native.ogl.vertexElements[i].stride * vertex + Extension.native.ogl.vertexElements[i].offset);
					morphTarget[mt].verts[vertex] = *position;
				}
			}
			break;
		case OGL_VERTEX_NORMAL:
			normals = true;
			for(gtaRwInt32 mt = 0; mt < numMorphTargets; mt++)
			{
				morphTarget[mt].hasNormals = true;
				morphTarget[mt].normals = (gtaRwV3d *)gtaMemAlloc(numVertices * 12);
				for(gtaRwInt32 vertex = 0; vertex < numVertices; vertex++)
				{
					gtaNativeOGlCompressedNormal *normal = (gtaNativeOGlCompressedNormal *)((gtaRwUInt32)Extension.native.ogl.vertexBuffer +
						Extension.native.ogl.vertexElements[i].stride * vertex + Extension.native.ogl.vertexElements[i].offset);
					if(normal->x > 0)
						morphTarget[mt].normals[vertex].x = (float)normal->x / 127.0f;
					else
						morphTarget[mt].normals[vertex].x = (float)normal->x / 128.0f;
					if(normal->x > 0)
						morphTarget[mt].normals[vertex].y = (float)normal->y / 127.0f;
					else
						morphTarget[mt].normals[vertex].y = (float)normal->y / 128.0f;
					if(normal->x > 0)
						morphTarget[mt].normals[vertex].z = (float)normal->z / 127.0f;
					else
						morphTarget[mt].normals[vertex].z = (float)normal->z / 128.0f;
				}
			}
			break;
		case OGL_VERTEX_TEXCOORD0:
			numTexCoordSets = 1;
			textured = true;
			texCoords[0] = (gtaRwTexCoords *)gtaMemAlloc(numVertices * 8);
			for(gtaRwInt32 vertex = 0; vertex < numVertices; vertex++)
			{
				gtaNativeOGlCompressedTexCoords *texCoordsCompressed = 
					(gtaNativeOGlCompressedTexCoords *)((gtaRwUInt32)Extension.native.ogl.vertexBuffer +
					Extension.native.ogl.vertexElements[i].stride * vertex + Extension.native.ogl.vertexElements[i].offset);
				texCoords[0][vertex].u = (float)texCoordsCompressed->u / 512.0f;
				texCoords[0][vertex].v = (float)texCoordsCompressed->v / 512.0f;
			}
			break;
		case OGL_VERTEX_GLOBALCOLOR:
			prelit = true;
			preLitLum = (gtaRwRGBA *)gtaMemAlloc(numVertices * 4);
			for(gtaRwInt32 vertex = 0; vertex < numVertices; vertex++)
			{
				gtaRwRGBA *color = (gtaRwRGBA *)((gtaRwUInt32)Extension.native.ogl.vertexBuffer +
					Extension.native.ogl.vertexElements[i].stride * vertex + Extension.native.ogl.vertexElements[i].offset);
				preLitLum[vertex] = *color;
			}
			break;
		case OGL_VERTEX_COLOR2:
			Extension.extraColour.Initialise(numVertices);
			for(gtaRwInt32 vertex = 0; vertex < numVertices; vertex++)
			{
				gtaRwRGBA *color = (gtaRwRGBA *)((gtaRwUInt32)Extension.native.ogl.vertexBuffer +
					Extension.native.ogl.vertexElements[i].stride * vertex + Extension.native.ogl.vertexElements[i].offset);
				Extension.extraColour.nightColors[vertex] = *color;
			}
			break;
		case OGL_VERTEX_BLENDINDICES:
			if(Extension.skin.enabled)
			{
				for(gtaRwInt32 vertex = 0; vertex < numVertices; vertex++)
				{
					gtaRwBoneIndices *boneIndices = (gtaRwBoneIndices *)((gtaRwUInt32)Extension.native.ogl.vertexBuffer +
						Extension.native.ogl.vertexElements[i].stride * vertex + Extension.native.ogl.vertexElements[i].offset);
					Extension.skin.vertexBoneIndices[vertex] = *boneIndices;
				}
			}
			break;
		case OGL_VERTEX_BLENDWEIGHT:
			if(Extension.skin.enabled)
			{
				for(gtaRwInt32 vertex = 0; vertex < numVertices; vertex++)
				{
					gtaCompressedBoneWeights *boneWeights = (gtaCompressedBoneWeights *)((gtaRwUInt32)Extension.native.ogl.vertexBuffer +
						Extension.native.ogl.vertexElements[i].stride * vertex + Extension.native.ogl.vertexElements[i].offset);
					Extension.skin.vertexBoneWeights[vertex].w0 = (float)boneWeights->w0 / 255.0f;
					Extension.skin.vertexBoneWeights[vertex].w1 = (float)boneWeights->w1 / 255.0f;
					Extension.skin.vertexBoneWeights[vertex].w2 = (float)boneWeights->w2 / 255.0f;
					Extension.skin.vertexBoneWeights[vertex].w3 = (float)boneWeights->w3 / 255.0f;
				}
			}
			break;
		}
	}
	if(!triangles && Extension.mesh.enabled)
	{
		if(Extension.mesh.tristrip)
		{
			tristrip = true;
			numTriangles = 0;
			for(gtaRwInt32 i = 0; i < Extension.mesh.numMeshes; i++)
				numTriangles += Extension.mesh.meshes[i].numIndices - 2;
			triangles = (gtaRwTriangle *)gtaMemAlloc(numTriangles * 8);
			gtaRwUInt32 triangleId = 0;
			for(gtaRwInt32 i = 0; i < Extension.mesh.numMeshes; i++)
			{
				triangles[triangleId].mtlId = Extension.mesh.meshes[i].material;
				triangles[triangleId].vertA = Extension.mesh.meshes[i].indices[0];
				triangles[triangleId].vertB = Extension.mesh.meshes[i].indices[1];
				triangles[triangleId].vertC = Extension.mesh.meshes[i].indices[2];
				triangleId++;
				gtaRwBool iteration = false;
				for(gtaRwInt32 j = 3; j < Extension.mesh.meshes[i].numIndices; j++)
				{
					if(iteration)
					{
						iteration = false;
						triangles[triangleId].mtlId = Extension.mesh.meshes[i].material;
						triangles[triangleId].vertA = Extension.mesh.meshes[i].indices[j - 2];
						triangles[triangleId].vertB = Extension.mesh.meshes[i].indices[j - 1];
						triangles[triangleId].vertC = Extension.mesh.meshes[i].indices[j];
						triangleId++;
					}
					else
					{
						iteration = true;
						triangles[triangleId].mtlId = Extension.mesh.meshes[i].material;
						triangles[triangleId].vertA = Extension.mesh.meshes[i].indices[j];
						triangles[triangleId].vertB = Extension.mesh.meshes[i].indices[j - 1];
						triangles[triangleId].vertC = Extension.mesh.meshes[i].indices[j - 2];
						triangleId++;
					}
				}
			}
		}
		else
		{
			numTriangles = Extension.mesh.totalIndicesInMesh / 3;
			triangles = (gtaRwTriangle *)gtaMemAlloc(numTriangles * 8);
			gtaRwUInt32 triangleId = 0;
			for(gtaRwInt32 i = 0; i < Extension.mesh.numMeshes; i++)
			{
				for(gtaRwInt32 j = 0; j < Extension.mesh.meshes[i].numIndices / 3; j++)
				{
					triangles[triangleId].mtlId = Extension.mesh.meshes[i].material;
					triangles[triangleId].vertA = Extension.mesh.meshes[i].indices[j];
					triangles[triangleId].vertB = Extension.mesh.meshes[i].indices[j + 1];
					triangles[triangleId].vertC = Extension.mesh.meshes[i].indices[j + 2];
					triangleId++;
				}
			}
		}
	}
	if(Extension.skin.enabled)
	{
		Extension.skin.FindUsedBoneIds(numVertices, Extension.skin.ogl.numBones);
		Extension.skin.skinToBoneMatrices = (gtaRwMatrix *)gtaMemAlloc(Extension.skin.numBones * 64);
		gtaMemCopy(Extension.skin.skinToBoneMatrices, Extension.skin.ogl.skinToBoneMatrices, Extension.skin.numBones * 64);
	}
	native = false;
	gtaMemFree(vertexElementSizes);
	return true;
}

void gtaRwGeometry::RecalculateFaces()
{
	if(Extension.mesh.enabled)
	{
		if(triangles)
			gtaMemFree(triangles);
		gtaRwUInt32 tempNumTriangles;
		gtaRwTriangle *tempTriangles;
		if(Extension.mesh.tristrip)
		{
			tristrip = true;
			tempNumTriangles = 0;
			for(gtaRwInt32 i = 0; i < Extension.mesh.numMeshes; i++)
				tempNumTriangles += Extension.mesh.meshes[i].numIndices - 2;
			tempTriangles = (gtaRwTriangle *)gtaMemAlloc(tempNumTriangles * 8);
			gtaRwUInt32 triangleId = 0;
			for(gtaRwInt32 i = 0; i < Extension.mesh.numMeshes; i++)
			{
				tempTriangles[triangleId].mtlId = Extension.mesh.meshes[i].material;
				tempTriangles[triangleId].vertA = Extension.mesh.meshes[i].indices[0];
				tempTriangles[triangleId].vertB = Extension.mesh.meshes[i].indices[1];
				tempTriangles[triangleId].vertC = Extension.mesh.meshes[i].indices[2];
				triangleId++;
				gtaRwBool iteration = false;
				for(gtaRwInt32 j = 3; j < Extension.mesh.meshes[i].numIndices; j++)
				{
					if(iteration)
					{
						iteration = false;
						tempTriangles[triangleId].mtlId = Extension.mesh.meshes[i].material;
						tempTriangles[triangleId].vertA = Extension.mesh.meshes[i].indices[j - 2];
						tempTriangles[triangleId].vertB = Extension.mesh.meshes[i].indices[j - 1];
						tempTriangles[triangleId].vertC = Extension.mesh.meshes[i].indices[j];
						triangleId++;
					}
					else
					{
						iteration = true;
						tempTriangles[triangleId].mtlId = Extension.mesh.meshes[i].material;
						tempTriangles[triangleId].vertA = Extension.mesh.meshes[i].indices[j];
						tempTriangles[triangleId].vertB = Extension.mesh.meshes[i].indices[j - 1];
						tempTriangles[triangleId].vertC = Extension.mesh.meshes[i].indices[j - 2];
						triangleId++;
					}
				}
			}
		}
		else
		{
			tempNumTriangles = Extension.mesh.totalIndicesInMesh / 3;
			tempTriangles = (gtaRwTriangle *)gtaMemAlloc(tempNumTriangles * 8);
			gtaRwUInt32 triangleId = 0;
			for(gtaRwInt32 i = 0; i < Extension.mesh.numMeshes; i++)
			{
				for(gtaRwInt32 j = 0; j < Extension.mesh.meshes[i].numIndices; j += 3)
				{
					tempTriangles[triangleId].mtlId = Extension.mesh.meshes[i].material;
					tempTriangles[triangleId].vertA = Extension.mesh.meshes[i].indices[j];
					tempTriangles[triangleId].vertB = Extension.mesh.meshes[i].indices[j + 1];
					tempTriangles[triangleId].vertC = Extension.mesh.meshes[i].indices[j + 2];
					triangleId++;
				}
			}
		}
		// delete degenerated faces
		numTriangles = 0;
		for(gtaRwInt32 i = 0; i < tempNumTriangles; i++)
		{
			if(tempTriangles[i].vertA == tempTriangles[i].vertB || tempTriangles[i].vertA == tempTriangles[i].vertC
				|| tempTriangles[i].vertB == tempTriangles[i].vertC)
				continue;
			tempTriangles[numTriangles].mtlId = tempTriangles[i].mtlId;
			tempTriangles[numTriangles].vertA = tempTriangles[i].vertA;
			tempTriangles[numTriangles].vertB = tempTriangles[i].vertB;
			tempTriangles[numTriangles].vertC = tempTriangles[i].vertC;
			numTriangles++;
		}
		triangles = (gtaRwTriangle *)gtaMemAlloc(numTriangles * 8);
		gtaMemCopy(triangles, tempTriangles, numTriangles * 8);
		gtaMemFree(tempTriangles);
	}
}

void gtaRwGeometry::ConvertFromNative(gtaPlatformId Platform)
{
	switch(Platform)
	{
	case PLATFORM_OGL:
		ConvertFromOGlNative();
		break;
	default:
		return;
	}
}