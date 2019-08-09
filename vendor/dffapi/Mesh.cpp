#pragma once

#include "Mesh.h"
#include "StdInc.h"

gtaRwMesh::gtaRwMesh()
{
	memset(this, 0, sizeof(gtaRwMesh));
}

void gtaRwMesh::Initialise(gtaRwUInt32 NumIndices, gtaRwUInt32 Material)
{
	memset(this, 0, sizeof(gtaRwMesh));
	numIndices = NumIndices;
	material = Material;
	if(NumIndices > 0)
	{
		indices = (gtaRwUInt32 *)malloc(4 * NumIndices);
		memset(indices, 0, 4 * NumIndices);
	}
}

void gtaRwMesh::Destroy()
{
	if(indices)
		free(indices);
	memset(this, 0, sizeof(gtaRwMesh));
}

gtaRwGeometryBinMesh::gtaRwGeometryBinMesh()
{
	memset(this, 0, sizeof(gtaRwGeometryBinMesh));
}

void gtaRwGeometryBinMesh::Initialise(gtaRwUInt32 Flags, gtaRwUInt32 NumMeshes, gtaRwUInt32 TotalIndicesInMesh)
{
	memset(this, 0, sizeof(gtaRwGeometryBinMesh));
	enabled = true;
	flags = Flags;
	numMeshes = NumMeshes;
	totalIndicesInMesh = TotalIndicesInMesh;
	if(numMeshes > 0)
	{
		meshes = (gtaRwMesh *)malloc(NumMeshes * sizeof(gtaRwMesh));
		memset(meshes, 0, NumMeshes * sizeof(gtaRwMesh));
	}
}

void gtaRwGeometryBinMesh::Destroy()
{
	if(meshes)
	{
		for(int i = 0; i < numMeshes; i++)
			meshes[i].Destroy();
		free(meshes);
	}
	memset(this, 0, sizeof(gtaRwGeometryBinMesh));
}

gtaRwBool gtaRwGeometryBinMesh::StreamWrite(gtaRwStream *stream, gtaRwBool IsNative)
{
	if(enabled)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_BINMESH, GetStreamSize(IsNative) - 12, gtaRwVersion, gtaRwBuild))
			return false;
		if(!gtaRwStreamWrite(stream, &flags, 12))
			return false;
		if(!IsNative || gtaPlatform == PLATFORM_OGL)
		{
			for(int i = 0; i < numMeshes; i++)
			{
				if(!gtaRwStreamWrite(stream, &meshes[i].numIndices, 8))
					return false;
				if(meshes[i].numIndices > 0)
				{
					if(IsNative)
					{
						gtaRwUInt16 *indices = (gtaRwUInt16 *)malloc(2 * meshes[i].numIndices);
						memset(indices, 0, 2 * meshes[i].numIndices);
						for(gtaRwInt8 j = 0; j < meshes[i].numIndices; j++)
							indices[j] = meshes[i].indices[j];
						if(!gtaRwStreamWrite(stream, indices, 2 * meshes[i].numIndices))
						{
							free(indices);
							return false;
						}
						free(indices);
					}
					else
					{
						if(!gtaRwStreamWrite(stream, meshes[i].indices, 4 * meshes[i].numIndices))
							return false;
					}
				}
			}
		}
	}
	return true;
}

gtaRwBool gtaRwGeometryBinMesh::StreamRead(gtaRwStream *stream, gtaRwBool IsNative)
{
	memset(this, 0, sizeof(gtaRwGeometryBinMesh));
	enabled = true;
	if(gtaRwStreamRead(stream, &flags, 12) != 12)
		return false;
	if(numMeshes > 0)
	{
		meshes = (gtaRwMesh *)malloc(numMeshes * sizeof(gtaRwMesh));
		memset(meshes, 0, numMeshes * sizeof(gtaRwMesh));
		for(int i = 0; i < numMeshes; i++)
		{
			if(gtaRwStreamRead(stream, &meshes[i].numIndices, 8) != 8)
			{
				Destroy();
				return false;
			}
			if(!IsNative || gtaIsMobile)
			{
				if(meshes[i].numIndices > 0)
				{
					meshes[i].indices = (gtaRwUInt32 *)malloc(4 * meshes[i].numIndices);
					memset(meshes[i].indices, 0, 4 * meshes[i].numIndices);
					if(IsNative)
					{
						gtaRwUInt16 *indices = (gtaRwUInt16 *)malloc(2 * meshes[i].numIndices);
						memset(indices, 0, 2 * meshes[i].numIndices);
						if(gtaRwStreamRead(stream, indices, 2 * meshes[i].numIndices) != 2 * meshes[i].numIndices)
						{
							free(indices);
							return false;
						}
						for(gtaRwInt32 j = 0; j < meshes[i].numIndices; j++)
							meshes[i].indices[j] = indices[j];
						free(indices);
					}
					else
					{
						if(gtaRwStreamRead(stream, meshes[i].indices, 4 * meshes[i].numIndices) != 4 * meshes[i].numIndices)
						{
							Destroy();
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}

gtaRwUInt32 gtaRwGeometryBinMesh::GetStreamSize(gtaRwBool IsNative)
{
	if(enabled)
	{
		gtaRwUInt32 size = 24;
		if(!IsNative || gtaPlatform == PLATFORM_OGL)
		{
			for(int i = 0; i < numMeshes; i++)
			{
				size += 8;
				if(IsNative)
					size += meshes[i].numIndices * 2;
				else
					size += meshes[i].numIndices * 4;
			}
		}
		return size;
	}
	return 0;
}