#include "StdInc.h"

void gtaClumpCollisionPluginInitialise(gtaClumpCollisionPlugin* collisionObj, gtaCollisionVersion Version, gtaRwChar* Name, gtaRwInt32 ModelId,
                                       gtaRwUInt32 NumSpheres, gtaRwUInt32 NumBoxes, gtaRwUInt32 NumLines, gtaRwUInt32 NumVertices, gtaRwUInt32 NumFaces,
                                       gtaRwUInt32 NumShadowVertices, gtaRwUInt32 NumShadowFaces)
{
    memset(collisionObj, 0, sizeof(gtaClumpCollisionPlugin));
	switch(Version)
	{
	case GTA_COL3:
        collisionObj->fourcc = '3LOC';
		break;
	default:
        gtaRwErrorSet("Not supported version (%d) for Collision plugin", collisionObj->fourcc);
		return;
	}
    collisionObj->enabled = rwTRUE;
    collisionObj->modelId = ModelId;
    strcpy(collisionObj->modelName, Name);
    collisionObj->numSpheres = NumSpheres;
    collisionObj->numBoxes = NumBoxes;
    collisionObj->numLines = NumLines;
    collisionObj->numVertices = NumVertices;
    collisionObj->numFaces = NumFaces;
    collisionObj->numShadowFaces = NumShadowFaces;
    collisionObj->numShadowVertices = NumShadowVertices;
	if(NumSpheres > 0)
        collisionObj->spheres = (gtaCollisionSphere*)gtaRwMemAlloc(NumSpheres * sizeof(gtaCollisionSphere));
	if(NumBoxes > 0)
        collisionObj->boxes = (gtaCollisionBox*)gtaRwMemAlloc(NumBoxes * sizeof(gtaCollisionBox));
	if(NumLines > 0)
        collisionObj->lines = (gtaCollisionLine*)gtaRwMemAlloc(NumLines * sizeof(gtaCollisionLine));
	if(NumVertices > 0)
        collisionObj->vertices = (gtaCollisionVertex*)gtaRwMemAlloc(NumVertices * sizeof(gtaCollisionVertex));
	if(NumFaces > 0)
        collisionObj->faces = (gtaCollisionTriangle*)gtaRwMemAlloc(NumFaces * sizeof(gtaCollisionTriangle));
	if(NumShadowFaces > 0)
        collisionObj->shadowFaces = (gtaCollisionTriangle*)gtaRwMemAlloc(NumShadowFaces * sizeof(gtaCollisionTriangle));
	if(NumShadowVertices > 0)
        collisionObj->shadowVertices = (gtaCollisionVertex*)gtaRwMemAlloc(NumSpheres * sizeof(gtaCollisionVertex));
}

void gtaClumpCollisionPluginDestroy(gtaClumpCollisionPlugin* collisionObj)
{
    if (collisionObj->spheres)
        free(collisionObj->spheres);
    if (collisionObj->boxes)
        free(collisionObj->boxes);
    if (collisionObj->lines)
        free(collisionObj->lines);
    if (collisionObj->vertices)
        free(collisionObj->vertices);
    if (collisionObj->faces)
        free(collisionObj->faces);
    if (collisionObj->faceGroups)
        free(collisionObj->faceGroups);
    if (collisionObj->shadowVertices)
        free(collisionObj->shadowVertices);
    if (collisionObj->shadowFaces)
        free(collisionObj->shadowFaces);
    memset(collisionObj, 0, sizeof(gtaClumpCollisionPlugin));
}

gtaRwBool gtaClumpCollisionPluginStreamWrite(gtaClumpCollisionPlugin* collisionObj, gtaRwStream* stream)
{
    if (collisionObj->enabled)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, gtaID_COLLISIONPLUGIN, 0, gtaRwVersion, gtaRwBuild))
			return rwFALSE;
	}
	return rwTRUE;
}

gtaRwBool gtaClumpCollisionPluginStreamRead(gtaClumpCollisionPlugin* collisionObj, gtaRwStream* stream)
{
    memset(collisionObj, 0, sizeof(gtaClumpCollisionPlugin));
    if (gtaRwStreamRead(stream, &collisionObj->fourcc, 8) != 8)
        return rwFALSE;
    gtaCollisionFile* data = (gtaCollisionFile*)gtaRwMemAlloc(collisionObj->size);
    if (gtaRwStreamRead(stream, data, collisionObj->size) == collisionObj->size)
	{
        strcpy(collisionObj->modelName, data->modelName);
        collisionObj->modelId = data->modelId;
        switch (collisionObj->fourcc)
		{
		case '3LOC':
                collisionObj->enabled = rwTRUE;
                collisionObj->min = data->col3.min;
                collisionObj->max = data->col3.max;
                collisionObj->center = data->col3.center;
                collisionObj->radius = data->col3.radius;
                collisionObj->numSpheres = data->col3.numSpheres;
                collisionObj->numBoxes = data->col3.numBoxes;
                collisionObj->numLines = data->col3.numLines;
                collisionObj->numFaces = data->col3.numFaces;

			// read spheres
                if (collisionObj->numSpheres > 0 && data->col3.spheres)
			{
				data->col3.spheres = (gtaCollisionSphere_COL2 *)((gtaRwUInt32)data->col3.spheres + (gtaRwUInt32)data - 4);
                collisionObj->spheres = (gtaCollisionSphere*)gtaRwMemAlloc(sizeof(gtaCollisionSphere) * collisionObj->numSpheres);
                for (int i = 0; i < collisionObj->numSpheres; i++)
				{
                    collisionObj->spheres[i].center = data->col3.spheres[i].center;
                    collisionObj->spheres[i].radius = data->col3.spheres[i].radius;
                    collisionObj->spheres[i].material = data->col3.spheres[i].material;
                    collisionObj->spheres[i].piece = data->col3.spheres[i].piece;
                    collisionObj->spheres[i].lighting = data->col3.spheres[i].lighting;
				}
			}
			// read boxes
            if (collisionObj->numBoxes > 0 && data->col3.boxes)
			{
				data->col3.boxes = (gtaCollisionBox_COL2 *)((gtaRwUInt32)data->col3.boxes + (gtaRwUInt32)data - 4);
                collisionObj->boxes = (gtaCollisionBox*)gtaRwMemAlloc(sizeof(gtaCollisionBox) * collisionObj->numBoxes);
                for (int i = 0; i < collisionObj->numBoxes; i++)
				{
                    collisionObj->boxes[i].min = data->col3.boxes[i].min;
                    collisionObj->boxes[i].max = data->col3.boxes[i].max;
                    collisionObj->boxes[i].material = data->col3.boxes[i].material;
                    collisionObj->boxes[i].piece = data->col3.boxes[i].piece;
                    collisionObj->boxes[i].lighting = data->col3.boxes[i].lighting;
				}
			}
			// read lines or disks
			if(data->col3.usesDisks)
			{
                if (collisionObj->numDisks > 0 && data->col3.disks)
				{
					data->col3.disks = (gtaCollisionDisk *)((gtaRwUInt32)data->col3.disks + (gtaRwUInt32)data - 4);
                    collisionObj->disks = (gtaCollisionDisk*)gtaRwMemAlloc(sizeof(gtaCollisionBox) * collisionObj->numDisks);
                    for (int i = 0; i < collisionObj->numDisks; i++)
					{
                        collisionObj->disks[i].start = data->col3.disks[i].start;
                        collisionObj->disks[i].end = data->col3.disks[i].end;
                        collisionObj->disks[i].startRadius = data->col3.disks[i].startRadius;
                        collisionObj->disks[i].endRadius = data->col3.disks[i].endRadius;
                        collisionObj->disks[i].material = data->col3.disks[i].material;
                        collisionObj->disks[i].piece = data->col3.disks[i].piece;
                        collisionObj->disks[i].lighting = data->col3.disks[i].lighting;
					}
				}
			}
			else
			{
                if (collisionObj->numLines > 0 && data->col3.lines)
				{
					data->col3.lines = (gtaCollisionLine_COL2 *)((gtaRwUInt32)data->col3.lines + (gtaRwUInt32)data - 4);
                    collisionObj->lines = (gtaCollisionLine*)gtaRwMemAlloc(sizeof(gtaCollisionLine) * collisionObj->numLines);
                    for (int i = 0; i < collisionObj->numLines; i++)
					{
                        collisionObj->lines[i].start = data->col3.lines[i].start;
                        collisionObj->lines[i].end = data->col3.lines[i].end;
                        collisionObj->lines[i].startRadius = data->col3.lines[i].startRadius;
                        collisionObj->lines[i].endRadius = data->col3.lines[i].endRadius;
					}
				}
			}
			

			/*if(numFaces > 0)
			{
				faces = (gtaCollisionTriangle *)gtaRwMemAlloc(sizeof(gtaCollisionTriangle) * numFaces);
				for(int i = 0; i < numFaces; i++)
				{
					faces[i].a = data->col3.
				}
			}
			if(data->col3.hasFaceGroups)
			{
				numFaceGroups;
			}
			if(data->col3.hasShadowMesh)
			{
				numShadowFaces = data->col3.numShadowFaces;
			}*/
			return rwTRUE;
		default:
            gtaRwErrorSet("Not supported version (%d) for Collision plugin", collisionObj->fourcc);
			break;
		}
	}
    gtaRwMemFree(data);
	Destroy();
	return rwFALSE;
}

unsigned int gtaClumpCollisionPluginGetStreamSize(gtaClumpCollisionPlugin* collisionObj)
{
    if (collisionObj->enabled)
	{
		unsigned int size = 12;
        switch (collisionObj->fourcc)
		{
		case '3LOC':
			size += 120;
            if (collisionObj->vertices && collisionObj->numVertices > 0)
			{
                size += collisionObj->numVertices * 6;
                if (!((collisionObj->numVertices * 6) % 4))
					size += 2;
			}
            if (collisionObj->faces && collisionObj->numFaces > 0)
                size += collisionObj->numFaces * 8;
            if (collisionObj->numFaceGroups > 0 && collisionObj->faceGroups)
                size += 4 + collisionObj->numFaceGroups * 28;
            if (collisionObj->numSpheres > 0 && collisionObj->spheres)
                size += collisionObj->numSpheres * 20;
            if (collisionObj->numBoxes > 0 && collisionObj->boxes)
                size += collisionObj->numBoxes * 28;
            if (collisionObj->usesDisks)
			{
                if (collisionObj->numDisks && collisionObj->disks)
                    size += collisionObj->numDisks * 36;
			}
			else
			{
                if (collisionObj->numLines && collisionObj->lines)
                    size += collisionObj->numLines * 32;
			}
            if (collisionObj->numShadowVertices > 0 && collisionObj->shadowVertices)
			{
                size += collisionObj->numShadowVertices * 6;
                if (!((collisionObj->numShadowVertices * 6) % 4))
					size += 2;
			}
            if (collisionObj->shadowFaces && collisionObj->numShadowFaces > 0)
                size += collisionObj->numShadowFaces * 8;
			return size;
		default:
            gtaRwErrorSet("Not supported version (%d) for Collision plugin", collisionObj->fourcc);
			return 0;
		}
	}
	return 0;
}