//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

void *gtaRwMemAlloc(gtaRwUInt32 size) {
    return malloc(size);
}

void *gtaRwMemAllocAndInit(gtaRwUInt32 size, gtaRwInt32 initValue) {
    void *memory = malloc(size);
    if (memory)
        memset(memory, initValue, size);
    return memory;
}

void *gtaRwMemAllocAndZero(gtaRwUInt32 size) {
    return gtaRwMemAllocAndInit(size, 0);
}

void gtaRwMemFree(void *memory) {
    free(memory);
}

void gtaRwMemZero(void *memory, gtaRwUInt32 size) {
    memset(memory, 0, size);
}

void gtaRwMemCopy(void *dest, void const *source, gtaRwUInt32 size) {
    memcpy(dest, source, size);
}

void *gtaRwMemRealloc(void *memory, gtaRwUInt32 newSize) {
    return realloc(memory, newSize);
}

void *gtaRwMemResize(void *oldMemory, gtaRwUInt32 oldSize, gtaRwUInt32 newSize) {
    if (oldSize <= newSize)
        return oldMemory;
    void *newMem = gtaRwMemRealloc(oldMemory, newSize);
    if (!newMem) {
        newMem = gtaRwMemAlloc(newSize);
        if (!newMem) {
            gtaRwMemFree(oldMemory);
            return rwNULL;
        }
        gtaRwMemCopy(newMem, oldMemory, oldSize);
        gtaRwMemFree(oldMemory);
    }
    gtaRwMemZero((gtaRwUInt8 *)newMem + oldSize, newSize - oldSize);
    return newMem;
}
