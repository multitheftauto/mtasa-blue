#pragma once

#include "CollisionPlugin.h"
#include "StdInc.h"
#include "Memory.h"

gtaClumpCollisionPlugin::gtaClumpCollisionPlugin()
{
	memset(this, 0, sizeof(gtaClumpCollisionPlugin));
}

void gtaClumpCollisionPlugin::Initialise(gtaCollisionVersion Version, gtaRwChar *Name, gtaRwInt32 ModelId, gtaRwUInt32 NumSpheres, gtaRwUInt32 NumBoxes, 
	gtaRwUInt32 NumLines, gtaRwUInt32 NumVertices, gtaRwUInt32 NumFaces, gtaRwUInt32 NumShadowVertices, gtaRwUInt32 NumShadowFaces)
{
	memset(this, 0, sizeof(gtaClumpCollisionPlugin));
	switch(Version)
	{
	case GTA_COL3:
		fourcc = '3LOC';
		break;
	default:
		gtaRwErrorSet("Not supported version (%d) for Collision plugin", fourcc);
		return;
	}
	enabled = true;
	modelId = ModelId;
	strcpy(modelName, Name);
	numSpheres = NumSpheres;
	numBoxes = NumBoxes;
	numLines = NumLines;
	numVertices = NumVertices;
	numFaces = NumFaces;
	numShadowFaces = NumShadowFaces;
	numShadowVertices = NumShadowVertices;
	if(NumSpheres > 0)
		spheres = (gtaCollisionSphere *)gtaMemAlloc(NumSpheres * sizeof(gtaCollisionSphere));
	if(NumBoxes > 0)
		boxes = (gtaCollisionBox *)gtaMemAlloc(NumBoxes * sizeof(gtaCollisionBox));
	if(NumLines > 0)
		lines = (gtaCollisionLine *)gtaMemAlloc(NumLines * sizeof(gtaCollisionLine));
	if(NumVertices > 0)
		vertices = (gtaCollisionVertex *)gtaMemAlloc(NumVertices * sizeof(gtaCollisionVertex));
	if(NumFaces > 0)
		faces = (gtaCollisionTriangle *)gtaMemAlloc(NumFaces * sizeof(gtaCollisionTriangle));
	if(NumShadowFaces > 0)
		shadowFaces = (gtaCollisionTriangle *)gtaMemAlloc(NumShadowFaces * sizeof(gtaCollisionTriangle));
	if(NumShadowVertices > 0)
		shadowVertices = (gtaCollisionVertex *)gtaMemAlloc(NumSpheres * sizeof(gtaCollisionVertex));
}

void gtaClumpCollisionPlugin::Destroy()
{
	if(spheres)
		free(spheres);
	if(boxes)
		free(boxes);
	if(lines)
		free(lines);
	if(vertices)
		free(vertices);
	if(faces)
		free(faces);
	if(faceGroups)
		free(faceGroups);
	if(shadowVertices)
		free(shadowVertices);
	if(shadowFaces)
		free(shadowFaces);
	memset(this, 0, sizeof(gtaClumpCollisionPlugin));
}

bool gtaClumpCollisionPlugin::StreamWrite(gtaRwStream *stream)
{
	if(enabled)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, gtaID_COLLISIONPLUGIN, 0, gtaRwVersion, gtaRwBuild))
			return false;
	}
	return true;
}

bool gtaClumpCollisionPlugin::StreamRead(gtaRwStream *stream)
{
	memset(this, 0, sizeof(gtaClumpCollisionPlugin));
	if(gtaRwStreamRead(stream, &fourcc, 8) != 8)
		return false;
	gtaCollisionFile *data = (gtaCollisionFile*)gtaMemAlloc(size);
	if(gtaRwStreamRead(stream, data, size) == size)
	{
		strcpy(modelName, data->modelName);
		modelId = data->modelId;
		switch(fourcc)
		{
		case '3LOC':
			enabled = true;
			min = data->col3.min;
			max = data->col3.max;
			center = data->col3.center;
			radius = data->col3.radius;
			numSpheres = data->col3.numSpheres;
			numBoxes = data->col3.numBoxes;
			numLines = data->col3.numLines;
			numFaces = data->col3.numFaces;

			// read spheres
			if(numSpheres > 0 && data->col3.spheres)
			{
				data->col3.spheres = (gtaCollisionSphere_COL2 *)((gtaRwUInt32)data->col3.spheres + (gtaRwUInt32)data - 4);
				spheres = (gtaCollisionSphere *)gtaMemAlloc(sizeof(gtaCollisionSphere) * numSpheres);
				for(int i = 0; i < numSpheres; i++)
				{
					spheres[i].center = data->col3.spheres[i].center;
					spheres[i].radius = data->col3.spheres[i].radius;
					spheres[i].material = data->col3.spheres[i].material;
					spheres[i].piece = data->col3.spheres[i].piece;
					spheres[i].lighting = data->col3.spheres[i].lighting;
				}
			}
			// read boxes
			if(numBoxes > 0 && data->col3.boxes)
			{
				data->col3.boxes = (gtaCollisionBox_COL2 *)((gtaRwUInt32)data->col3.boxes + (gtaRwUInt32)data - 4);
				boxes = (gtaCollisionBox *)gtaMemAlloc(sizeof(gtaCollisionBox) * numBoxes);
				for(int i = 0; i < numBoxes; i++)
				{
					boxes[i].min = data->col3.boxes[i].min;
					boxes[i].max = data->col3.boxes[i].max;
					boxes[i].material = data->col3.boxes[i].material;
					boxes[i].piece = data->col3.boxes[i].piece;
					boxes[i].lighting = data->col3.boxes[i].lighting;
				}
			}
			// read lines or disks
			if(data->col3.usesDisks)
			{
				if(numDisks > 0 && data->col3.disks)
				{
					data->col3.disks = (gtaCollisionDisk *)((gtaRwUInt32)data->col3.disks + (gtaRwUInt32)data - 4);
					disks = (gtaCollisionDisk *)gtaMemAlloc(sizeof(gtaCollisionBox) * numDisks);
					for(int i = 0; i < numDisks; i++)
					{
						disks[i].start = data->col3.disks[i].start;
						disks[i].end = data->col3.disks[i].end;
						disks[i].startRadius = data->col3.disks[i].startRadius;
						disks[i].endRadius = data->col3.disks[i].endRadius;
						disks[i].material = data->col3.disks[i].material;
						disks[i].piece = data->col3.disks[i].piece;
						disks[i].lighting = data->col3.disks[i].lighting;
					}
				}
			}
			else
			{
				if(numLines > 0 && data->col3.lines)
				{
					data->col3.lines = (gtaCollisionLine_COL2 *)((gtaRwUInt32)data->col3.lines + (gtaRwUInt32)data - 4);
					lines = (gtaCollisionLine *)gtaMemAlloc(sizeof(gtaCollisionLine) * numLines);
					for(int i = 0; i < numLines; i++)
					{
						lines[i].start = data->col3.lines[i].start;
						lines[i].end = data->col3.lines[i].end;
						lines[i].startRadius = data->col3.lines[i].startRadius;
						lines[i].endRadius = data->col3.lines[i].endRadius;
					}
				}
			}
			

			/*if(numFaces > 0)
			{
				faces = (gtaCollisionTriangle *)gtaMemAlloc(sizeof(gtaCollisionTriangle) * numFaces);
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
			return true;
		default:
			gtaRwErrorSet("Not supported version (%d) for Collision plugin", fourcc);
			break;
		}
	}
	gtaMemFree(data);
	Destroy();
	return false;
}

unsigned int gtaClumpCollisionPlugin::GetStreamSize()
{
	if(enabled)
	{
		unsigned int size = 12;
		switch(fourcc)
		{
		case '3LOC':
			size += 120;
			if(vertices && numVertices > 0)
			{
				size += numVertices * 6;
				if(!((numVertices * 6) % 4))
					size += 2;
			}
			if(faces && numFaces > 0)
				size += numFaces * 8;
			if(numFaceGroups > 0 && faceGroups)
				size += 4 + numFaceGroups * 28;
			if(numSpheres > 0 && spheres)
				size += numSpheres * 20;
			if(numBoxes > 0 && boxes)
				size += numBoxes * 28;
			if(usesDisks)
			{
				if(numDisks && disks)
					size += numDisks * 36;
			}
			else
			{
				if(numLines && lines)
					size += numLines * 32;
			}
			if(numShadowVertices > 0 && shadowVertices)
			{
				size += numShadowVertices * 6;
				if(!((numShadowVertices * 6) % 4))
					size += 2;
			}
			if(shadowFaces && numShadowFaces > 0)
				size += numShadowFaces * 8;
			return size;
		default:
			gtaRwErrorSet("Not supported version (%d) for Collision plugin", fourcc);
			return 0;
		}
	}
	return 0;
}