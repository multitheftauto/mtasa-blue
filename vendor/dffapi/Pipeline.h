#pragma once

#include "Extension.h"
#include "Stream.h"
#include "RwTypes.h"
#include "StdInc.h"

struct gtaRwAtomicPipeline : public gtaRwExtension
{
	gtaRwUInt32 pipelineId;

	gtaRwAtomicPipeline();

	bool StreamWrite(gtaRwStream *stream);

	bool StreamRead(gtaRwStream *stream);
	
	unsigned int GetStreamSize();

	void Initialise(gtaRwUInt32 PipelineId);

	void Destroy();
};