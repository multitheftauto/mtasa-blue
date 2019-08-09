#pragma once

#include "Extension.h"
#include "Stream.h"
#include "RwTypes.h"

struct gtaFrameNodeName : public gtaRwExtension
{
	gtaRwChar *name;

	gtaFrameNodeName();

	bool StreamWrite(gtaRwStream *stream);

	bool StreamRead(gtaRwStream *stream, gtaRwUInt32 length);
	
	unsigned int GetStreamSize();

	void Initialise(gtaRwChar *Name);

	void Destroy();
};