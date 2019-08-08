typedef struct gtaCompressedBoneWeights gtaCompressedBoneWeights;
typedef struct gtaRwGeometrySkin gtaRwGeometrySkin;

struct gtaRwBoneIndices
{
	union{
		struct{
			gtaRwUInt8 i0;
			gtaRwUInt8 i1;
			gtaRwUInt8 i2;
			gtaRwUInt8 i3;
		};
		gtaRwUInt8 ai[4];
	};
};

struct gtaRwBoneWeights
{
	union{
		struct{
			gtaRwReal w0;
			gtaRwReal w1;
			gtaRwReal w2;
			gtaRwReal w3;
		};
		gtaRwReal aw[4];
	};
};

struct gtaCompressedBoneWeights
{
	union{
		struct{
			gtaRwUInt8 w0;
			gtaRwUInt8 w1;
			gtaRwUInt8 w2;
			gtaRwUInt8 w3;
		};
		gtaRwUInt8 aw[4];
	};
};

typedef struct gtaRwGeometrySkin
{
	gtaRwUInt8 numBones;
	gtaRwUInt8 numBoneIds;
	gtaRwUInt8 maxNumWeightsPerVertex;
	gtaRwUInt8 pad;
	gtaRwUInt8 *boneIds;
	gtaRwBoneIndices *vertexBoneIndices;
	gtaRwBoneWeights *vertexBoneWeights;
	gtaRwMatrix *skinToBoneMatrices;
	// Split Data
	gtaRwUInt32 boneLimit;
	gtaRwUInt32 numMeshes;
	gtaRwUInt32 numRLE ;
	// if numMeshes > 0
	gtaRwUInt8 *meshBoneRemapIndices;

	struct gtaNativeSkinD3D9
	{
		gtaRwUInt32 numBones;
		gtaRwBool generated;
	} d3d9;

	struct gtaNativeSkinOGL
	{
		gtaRwUInt32 numBones;
		gtaRwMatrix *skinToBoneMatrices;
		gtaRwBool generated;
	} ogl;

	gtaRwBool enabled;
};


void gtaRwSkinInitialise(gtaRwGeometrySkin* skinObj, gtaRwUInt8 NumBones, gtaRwUInt8 NumBoneIds, gtaRwUInt32 NumVertices, gtaRwUInt8 MaxNumWeightsPerVertex);
void gtaRwSkinDestroy(gtaRwGeometrySkin* skinObj);
gtaRwBool    gtaRwSkinStreamWrite(gtaRwGeometrySkin* skinObj, gtaRwStream* Stream, gtaRwUInt32 NumVertices, gtaRwBool IsNative);
gtaRwBool    gtaRwSkinStreamRead(gtaRwGeometrySkin* skinObj, gtaRwStream* Stream, gtaRwUInt32 NumVertices, gtaRwBool IsNative);
unsigned int gtaRwSkinGetStreamSize(gtaRwGeometrySkin* skinObj, gtaRwBool IsNative, gtaRwUInt32 NumVertices);
void         gtaRwSkinFindUsedBoneIds(gtaRwGeometrySkin* skinObj, gtaRwUInt32 NumVertices, gtaRwInt32 NumBones);