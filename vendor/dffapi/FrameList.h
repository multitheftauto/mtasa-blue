#pragma once

#include "RwTypes.h"
#include "Stream.h"
#include "NodeName.h"
#include "HAnim.h"

struct gtaRwFrame
{
	gtaRwV3d right;
	gtaRwV3d up;
	gtaRwV3d at;
	gtaRwV3d pos;
	gtaRwInt32 parent;
	gtaRwUInt32 matrixFlags;
	struct{
		gtaRwFrameHAnim hAnim;
		gtaFrameNodeName nodeName;
	} Extension;

	gtaRwFrame();

	void Initialise(gtaRwInt32 Parent, gtaRwUInt32 MatrixFlags);

	void Initialise(gtaRwV3d *Rotation, gtaRwV3d *Position, gtaRwInt32 Parent, gtaRwUInt32 MatrixFlags);

	void Initialise(gtaRwV3d *Right, gtaRwV3d *Up, gtaRwV3d *At, gtaRwV3d *Position, gtaRwInt32 Parent, gtaRwUInt32 MatrixFlags);

	void Destroy();

	void SetName(gtaRwChar *Name);
};

struct gtaRwFrameList
{
	gtaRwUInt32 frameCount;
	gtaRwFrame *frames;

	gtaRwFrameList();

	void Initialise(gtaRwInt32 FrameCount);

	void Destroy();

	gtaRwBool StreamRead(gtaRwStream *stream);

	gtaRwBool StreamWrite(gtaRwStream *stream);

	gtaRwUInt32 GetStreamSize();
};