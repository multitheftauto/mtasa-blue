#include <StdInc.h>
#include "wrapper.h"

h_CAnimBlendHierarchy_Constructor OLD__CAnimBlendHierarchy_Constructor = (h_CAnimBlendHierarchy_Constructor)0x4CF270;

void _CAnimBlendHierarchy_Constructor ( _CAnimBlendHierarchy * pAnimHierarchy )
{
    int pThis = (int)pAnimHierarchy;


    *(DWORD *)(pThis + 4) = 0;
    *(WORD *)(pThis + 8) = 0;
    *(BYTE *)(pThis + 10) = 0;
    *(BYTE *)(pThis + 11) = 0;
    *(DWORD *)(pThis + 12) = -1;
    *(DWORD *)(pThis + 16) = 0;
    *(DWORD *)(pThis + 20) = 0;
    
}

