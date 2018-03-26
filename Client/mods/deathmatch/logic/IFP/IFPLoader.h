#pragma once
#ifndef IFPLOADER_H
#define IFPLOADER_H
#include "wrapper.h"
#include "FileLoader.h"
#include <vector>


typedef void *(__cdecl* hCMemoryMgr_Malloc)
(
    size_t TotalBytesToAllocate
);


typedef void *(__cdecl* hCMemoryMgr_Free)
(
    void *a1
);

void _CAnimBlendHierarchy_Constructor(_CAnimBlendHierarchy * pAnimHierarchy);
void _CAnimBlendSequence_Constructor(_CAnimBlendSequence * pSequence);




// --------------------------------------  For Hierarchy ----------------------------------------------------

void Call__CAnimBlendHierarchy_RemoveQuaternionFlips(_CAnimBlendHierarchy * pAnimHierarchy);

void Call__CAnimBlendHierarchy_CalcTotalTime(_CAnimBlendHierarchy * pAnimHierarchy);

#endif
