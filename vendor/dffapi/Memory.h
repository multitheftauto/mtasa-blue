#pragma once

#include "RwTypes.h"

void *gtaMemAlloc(gtaRwUInt32 Size, gtaRwInt32 DefaultValue = 0);

void gtaMemFree(void *Memory);

void gtaMemZero(void *Memory, gtaRwUInt32 Size);

void gtaMemCopy(void *Dest, void *Source, gtaRwUInt32 Size);