#pragma once

#include "Memory.h"
#include <Windows.h>

void *gtaMemAlloc(gtaRwUInt32 Size, gtaRwInt32 DefaultValue)
{
	void *memory = malloc(Size);
	memset(memory, DefaultValue, Size);
	return memory;
}

void gtaMemFree(void *Memory)
{
	free(Memory);
}

void gtaMemZero(void *Memory, gtaRwUInt32 Size)
{
	memset(Memory, 0, Size);
}

void gtaMemCopy(void *Dest, void *Source, gtaRwUInt32 Size)
{
	memcpy(Dest, Source, Size);
}