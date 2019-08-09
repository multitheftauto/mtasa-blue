#pragma once

#include "Pipeline.h"
#include "StdInc.h"

gtaRwAtomicPipeline::gtaRwAtomicPipeline()
{
	memset(this, 0, sizeof(gtaRwAtomicPipeline));
}

bool gtaRwAtomicPipeline::StreamWrite(gtaRwStream *stream)
{
	if(enabled)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, gtaID_PIPELINE, 4, gtaRwVersion, gtaRwBuild))
			return false;
		if(!gtaRwStreamWrite(stream, &pipelineId, 4))
			return false;
	}
	return true;
}

bool gtaRwAtomicPipeline::StreamRead(gtaRwStream *stream)
{
	memset(this, 0, sizeof(gtaRwAtomicPipeline));
	enabled = true;
	if(gtaRwStreamRead(stream, &pipelineId, 4) != 4)
		return false;
	return true;
}

unsigned int gtaRwAtomicPipeline::GetStreamSize()
{
	if(enabled)
		return 16;
	return 0;
}

void gtaRwAtomicPipeline::Initialise(gtaRwUInt32 PipelineId)
{
	memset(this, 0, sizeof(gtaRwAtomicPipeline));
	enabled = true;
	pipelineId = PipelineId;
}

void gtaRwAtomicPipeline::Destroy()
{
	memset(this, 0, sizeof(gtaRwAtomicPipeline));
}