#pragma once

#include "Stream.h"
#include "Extension.h"
#include "StdInc.h"

struct gtaRwMesh
{
	gtaRwUInt32 numIndices; // number of vertices in mesh
	gtaRwUInt32 material;   // id of material in material list used to render the mesh.
	gtaRwUInt32 *indices;   // vertex indices defining the mesh

	gtaRwMesh();

	void Initialise(gtaRwUInt32 NumIndices, gtaRwUInt32 Material);

	void Destroy();
};

struct gtaRwGeometryBinMesh : public gtaRwExtension
{
	union{
		gtaRwUInt32 flags;    // see gtaRwMeshHeaderFlags
		struct{
			gtaRwUInt32 tristrip : 1;
			gtaRwUInt32 trifan : 1;
			gtaRwUInt32 linelist : 1;
			gtaRwUInt32 polyline : 1;
			gtaRwUInt32 pointlist : 1;
			gtaRwUInt32 unused : 3;
			gtaRwUInt32 unindexed : 1;
		};
	};
	gtaRwUInt32 numMeshes; // Number of meshes in object
	gtaRwUInt32 totalIndicesInMesh; // Total triangle index count in all meshes
	gtaRwMesh *meshes;

	gtaRwGeometryBinMesh();

	void Initialise(gtaRwUInt32 Flags, gtaRwUInt32 NumMeshes, gtaRwUInt32 TotalIndicesInMesh);

	void Destroy();

	gtaRwBool StreamWrite(gtaRwStream *stream, gtaRwBool IsNative);

	gtaRwBool StreamRead(gtaRwStream *stream, gtaRwBool IsNative);
	
	gtaRwUInt32 GetStreamSize(gtaRwBool IsNative);
};