#pragma once

#include "Breakable.h"
#include "StdInc.h"
#include "Memory.h"

gtaGeometryBreakable::gtaGeometryBreakable()
{
	memset(this, 0, sizeof(gtaGeometryBreakable));
}

void gtaGeometryBreakable::Initialise(gtaRwUInt32 VertexCount, gtaRwUInt32 FaceCount, gtaRwUInt32 MeshCount, gtaRwBool PosnRelativeToCollision)
{
	memset(this, 0, sizeof(gtaGeometryBreakable));
	enabled = true;
	if(VertexCount == 0 && FaceCount == 0 && MeshCount == 0)
		thisData = NULL;
	else
	{
		thisData = this;
		posnRelativeToCollision = PosnRelativeToCollision;
		vertexCount = VertexCount;
		faceCount = FaceCount;
		meshCount = MeshCount;
		vertices = (gtaRwV3d *)gtaMemAlloc(VertexCount * 12);
		texCoors = (gtaRwTexCoords *)gtaMemAlloc(VertexCount * 8);
		colors = (gtaRwRGBA *)gtaMemAlloc(VertexCount * 4, 0xFFFFFFFF);
		faces = (gtaBreakableFace *)gtaMemAlloc(FaceCount * 6);
		faceMaterials = (gtaRwUInt16 *)gtaMemAlloc(FaceCount * 2);
		texNames = (gtaRwChar (*)[32])gtaMemAlloc(MeshCount * 32);
		texMaskNames = (gtaRwChar (*)[32])gtaMemAlloc(MeshCount * 32);
		matColors = (gtaBreakableMaterialColor *)gtaMemAlloc(MeshCount * 12);
	}
}

void gtaGeometryBreakable::Destroy()
{
	if(vertices)
		free(vertices);
	if(texCoors)
		free(texCoors);
	if(colors)
		free(colors);
	if(faces)
		free(faces);
	if(faceMaterials)
		free(faceMaterials);
	if(texNames)
		free(texNames);
	if(texMaskNames)
		free(texMaskNames);
	if(matColors)
		free(matColors);
	memset(this, 0, sizeof(gtaGeometryBreakable));
}

bool gtaGeometryBreakable::StreamWrite(gtaRwStream *stream)
{
	if(enabled)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, gtaID_BREAKABLE, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
			return false;
		if(!thisData)
		{
			if(!gtaRwStreamWrite(stream, &thisData, 4))
				return false;
		}
		else
		{
			if(!gtaRwStreamWrite(stream, &thisData, 56))
				return false;
			if(!gtaRwStreamWrite(stream, vertices, vertexCount * 12))
				return false;
			if(!gtaRwStreamWrite(stream, texCoors, vertexCount * 8))
				return false;
			if(!gtaRwStreamWrite(stream, colors, vertexCount * 4))
				return false;
			if(!gtaRwStreamWrite(stream, faces, faceCount * 6))
				return false;
			if(!gtaRwStreamWrite(stream, faceMaterials, faceCount * 2))
				return false;
			if(!gtaRwStreamWrite(stream, texNames, meshCount * 32))
				return false;
			if(!gtaRwStreamWrite(stream, texMaskNames, meshCount * 32))
				return false;
			if(!gtaRwStreamWrite(stream, matColors, meshCount * 12))
				return false;
		}
	}
	return true;
}

bool gtaGeometryBreakable::StreamRead(gtaRwStream *stream)
{
	memset(this, 0, sizeof(gtaGeometryBreakable));
	if(gtaRwStreamRead(stream, &thisData, 4) != 4)
		return false;
	if(thisData)
	{
		enabled = true;
		thisData = this;
		if(gtaRwStreamRead(stream, &posnRelativeToCollision, 52) != 52)
		{
			Destroy();
			return false;
		}
		vertices = (gtaRwV3d *)gtaMemAlloc(vertexCount * 12);
		if(gtaRwStreamRead(stream, vertices, vertexCount * 12) != vertexCount * 12)
		{
			Destroy();
			return false;
		}
		texCoors = (gtaRwTexCoords *)gtaMemAlloc(vertexCount * 8);
		if(gtaRwStreamRead(stream, texCoors, vertexCount * 8) != vertexCount * 8)
		{
			Destroy();
			return false;
		}
		colors = (gtaRwRGBA *)gtaMemAlloc(vertexCount * 4, 0xFFFFFFFF);
		if(gtaRwStreamRead(stream, colors, vertexCount * 4) != vertexCount * 4)
		{
			Destroy();
			return false;
		}
		faces = (gtaBreakableFace *)gtaMemAlloc(faceCount * 6);
		if(gtaRwStreamRead(stream, faces, faceCount * 6) != faceCount * 6)
		{
			Destroy();
			return false;
		}
		faceMaterials = (gtaRwUInt16 *)gtaMemAlloc(faceCount * 2);
		if(gtaRwStreamRead(stream, faceMaterials, faceCount * 2) != faceCount * 2)
		{
			Destroy();
			return false;
		}
		texNames = (gtaRwChar (*)[32])gtaMemAlloc(meshCount * 32);
		if(gtaRwStreamRead(stream, texNames, meshCount * 32) != meshCount * 32)
		{
			Destroy();
			return false;
		}
		texMaskNames = (gtaRwChar (*)[32])gtaMemAlloc(meshCount * 32);
		if(gtaRwStreamRead(stream, texMaskNames, meshCount * 32) != meshCount * 32)
		{
			Destroy();
			return false;
		}
		matColors = (gtaBreakableMaterialColor *)gtaMemAlloc(meshCount * 12);
		if(gtaRwStreamRead(stream, matColors, meshCount * 12) != meshCount * 12)
		{
			Destroy();
			return false;
		}
	}
	return true;
}

unsigned int gtaGeometryBreakable::GetStreamSize()
{
	if(enabled)
	{
		if(thisData)
			return 68 + vertexCount * 24 + faceCount * 8 + meshCount * 76;
		return 16;
	}
	return 0;
}