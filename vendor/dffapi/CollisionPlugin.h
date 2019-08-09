#pragma once

#include "Extension.h"
#include "Stream.h"
#include "RwTypes.h"

struct gtaCollisionSphere_COLL
{
	gtaRwReal radius;
	gtaRwV3d center;
	gtaRwUInt8 material;
	gtaRwUInt8 piece;
	gtaRwUInt8 _pad[2];
};

struct gtaCollisionSphere_COL2
{
	gtaRwV3d center;
	gtaRwReal radius;
	gtaRwUInt8 material;
	gtaRwUInt8 piece;
	gtaRwUInt8 lighting;
	gtaRwUInt8 _pad;
};

struct gtaCollisionSphere
{
	gtaRwV3d center;
	gtaRwReal radius;
	gtaRwUInt8 material;
	gtaRwUInt8 piece;
	gtaRwUInt8 lighting;
};

struct gtaCollisionBox_COLL
{
	gtaRwV3d min;
	gtaRwV3d max;
	gtaRwUInt8 material;
	gtaRwUInt8 piece;
	gtaRwUInt8 _pad[2];
};

struct gtaCollisionBox_COL2
{
	gtaRwV3d min;
	gtaRwV3d max;
	gtaRwUInt8 material;
	gtaRwUInt8 piece;
	gtaRwUInt8 lighting;
	gtaRwUInt8 _pad;
};

struct gtaCollisionBox
{
	gtaRwV3d min;
	gtaRwV3d max;
	gtaRwUInt8 material;
	gtaRwUInt8 piece;
	gtaRwUInt8 lighting;
};

struct gtaCollisionFaceGroup
{
	gtaRwV3d min;
	gtaRwV3d max;
	gtaRwUInt16 startFace;
	gtaRwUInt16 endFace;
};

struct gtaCollisionLine_COLL
{
	gtaRwV3d start;
	gtaRwV3d end;
};

struct gtaCollisionLine_COL2
{
	gtaRwV3d start;
	gtaRwReal startRadius; // unused
	gtaRwV3d end;
	gtaRwReal endRadius; // unused
};

struct gtaCollisionLine
{
	gtaRwV3d start;
	gtaRwReal startRadius; // unused
	gtaRwV3d end;
	gtaRwReal endRadius; // unused
};

struct gtaCollisionDisk
{
	gtaRwV3d start;
	gtaRwReal startRadius;
	gtaRwUInt8 material;
	gtaRwUInt8 piece;
	gtaRwUInt8 lighting;
	gtaRwUInt8 _pad;
	gtaRwV3d end;
	gtaRwReal endRadius;
};

struct gtaCollisionTriangle_COLL
{
	gtaRwUInt32 a, b, c;
	gtaRwUInt8 material;
	gtaRwUInt8 unused;
	gtaRwUInt8 _pad[2];
};

struct gtaCollisionTriangle_COL2
{
	gtaRwUInt16 a, b, c;
	gtaRwUInt8 material;
	gtaRwUInt8 light;
};

struct gtaCollisionTriangle
{
	gtaRwUInt16 a, b, c;
	gtaRwUInt8 material;
	gtaRwUInt8 light;
};

struct gtaCollisionVertex_COLL
{
	gtaRwReal x, y, z;
};

struct gtaCollisionVertex_COL2
{
	gtaRwInt16 x, y, z;
};

struct gtaCollisionVertex
{
	gtaRwReal x, y, z;
};

enum gtaCollisionVersion
{
	GTA_COLL = 0,
	GTA_COL2 = 1,
	GTA_COL3 = 2,
	GTA_COL4 = 3
};

struct gtaCollisionFile_COLL
{
	gtaRwReal radius;
	gtaRwV3d center;
	gtaRwV3d min;
	gtaRwV3d max;
	gtaRwUInt32 numSpheres;
};

struct gtaCollisionFile_COL2
{
	gtaRwV3d min;
	gtaRwV3d max;
	gtaRwV3d center;
	gtaRwReal radius;
	gtaRwUInt16 numSpheres;
	gtaRwUInt16 numBoxes;
	gtaRwUInt16 numFaces;
	gtaRwUInt8 numLines;
	gtaRwUInt8 _pad;
	union{
		gtaRwUInt32 flags;
		struct{
			gtaRwUInt32 usesDisks : 1;
			gtaRwUInt32 isNotEmpty : 1;
			gtaRwUInt32 flags_b3 : 1;
			gtaRwUInt32 hasFaceGroups : 1;
			gtaRwUInt32 hasShadowMesh : 1;
		};
	};
	gtaCollisionSphere_COL2 *spheres;
	gtaCollisionBox_COL2 *boxes;
	union
	{
		gtaCollisionLine_COL2 *lines;
		gtaCollisionDisk *disks;
	};
	gtaCollisionVertex_COL2 *vertices;
	gtaRwUInt32 numFaceGroups;
	gtaCollisionFaceGroup *faceGroups;
	gtaCollisionTriangle_COL2 *faces;
	void *facePlanes; // not read
};

struct gtaCollisionFile_COL3 : public gtaCollisionFile_COL2
{
	gtaRwUInt32 numShadowFaces;
	gtaCollisionVertex_COL2 *shadowVertices;
	gtaCollisionTriangle_COL2 *shadowFaces;
};

struct gtaCollisionFile_COL4 : public gtaCollisionFile_COL3
{
	gtaRwUInt32 unknown; // not used
};

struct gtaCollisionFile
{
	gtaRwChar modelName[20];
	gtaRwInt8 _pad[2];
	gtaRwInt16 modelId;
	union
	{
		gtaCollisionFile_COLL coll;
		gtaCollisionFile_COL2 col2;
		gtaCollisionFile_COL3 col3;
		gtaCollisionFile_COL4 col4;
	};
};

struct gtaClumpCollisionPlugin : public gtaRwExtension
{
	gtaRwUInt32 fourcc;
	gtaRwUInt32 size;
	gtaRwChar modelName[20];
	gtaRwInt16 modelId;
	gtaRwV3d min;
	gtaRwV3d max;
	gtaRwV3d center;
	gtaRwReal radius;
	gtaRwUInt16 numSpheres;
	gtaRwUInt16 numBoxes;
	gtaRwUInt16 numFaces;
	gtaRwUInt32 numVertices;
	union
	{
		gtaRwUInt8 numLines;
		gtaRwUInt8 numDisks;
	};
	gtaRwBool usesDisks;
	gtaCollisionSphere *spheres;
	gtaCollisionBox *boxes;
	union
	{
		gtaCollisionLine *lines;
		gtaCollisionDisk *disks;
	};
	gtaCollisionVertex *vertices;
	gtaCollisionTriangle *faces;
	gtaRwUInt32 numFaceGroups;
	gtaCollisionFaceGroup *faceGroups;
	gtaRwUInt32 numShadowFaces;
	gtaRwUInt32 numShadowVertices;
	gtaCollisionVertex *shadowVertices;
	gtaCollisionTriangle *shadowFaces;

	gtaClumpCollisionPlugin();

	void Initialise(gtaCollisionVersion Version, gtaRwChar *Name, gtaRwInt32 ModelId, gtaRwUInt32 NumSpheres, gtaRwUInt32 NumBoxes, 
		gtaRwUInt32 NumLines, gtaRwUInt32 NumVertices, gtaRwUInt32 NumFaces, gtaRwUInt32 NumShadowVertices, gtaRwUInt32 NumShadowFaces);

	void Destroy();

	bool StreamWrite(gtaRwStream *stream);

	bool StreamRead(gtaRwStream *stream);
	
	unsigned int GetStreamSize();
};