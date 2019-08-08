struct gtaRwMesh
{
	gtaRwUInt32 numIndices; // number of vertices in mesh
	gtaRwUInt32 material;   // id of material in material list used to render the mesh.
	gtaRwUInt32 *indices;   // vertex indices defining the mesh

};

void gtaRwMeshInitialise(gtaRwMesh* meshObj, gtaRwUInt32 NumIndices, gtaRwUInt32 Material);
void gtaRwMeshDestroy(gtaRwMesh* meshObj);

struct gtaRwGeometryBinMesh
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
    gtaRwUInt32 numMeshes;                     // Number of meshes in object
    gtaRwUInt32 totalIndicesInMesh;            // Total triangle index count in all meshes
    gtaRwMesh*  meshes;
    gtaRwBool   enabled;
};


void        gtaRwGeometryBinMeshInitialise(gtaRwGeometryBinMesh* geometryBinMeshObj, gtaRwUInt32 Flags, gtaRwUInt32 NumMeshes, gtaRwUInt32 TotalIndicesInMesh);
void        gtaRwGeometryBinMeshDestroy(gtaRwGeometryBinMesh* geometryBinMeshObj);
gtaRwBool   gtaRwGeometryBinMeshStreamWrite(gtaRwGeometryBinMesh* geometryBinMeshObj, gtaRwStream* stream, gtaRwBool IsNative);
gtaRwBool   gtaRwGeometryBinMeshStreamRead(gtaRwGeometryBinMesh* geometryBinMeshObj, gtaRwStream* stream, gtaRwBool IsNative);
gtaRwUInt32 gtaRwGeometryBinMeshGetStreamSize(gtaRwGeometryBinMesh* geometryBinMeshObj, gtaRwBool IsNative);