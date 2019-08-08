//
// dffapi
// https://github.com/DK22Pac/dffapi
//

void *gtaRwMemAlloc(gtaRwUInt32 size);
void *gtaRwMemAllocAndInit(gtaRwUInt32 size, gtaRwInt32 initValue);
void *gtaRwMemAllocAndZero(gtaRwUInt32 size);
void gtaRwMemFree(void *memory);
void gtaRwMemZero(void *memory, gtaRwUInt32 sgtaMemZeroize);
void gtaRwMemCopy(void *dest, void const *source, gtaRwUInt32 size);
void *gtaRwMemRealloc(void *memory, gtaRwUInt32 newSize);
void *gtaRwMemResize(void *oldMemory, gtaRwUInt32 oldSize, gtaRwUInt32 newSize);
