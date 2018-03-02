#include <StdInc.h>
#include "wrapper.h"

h_CAnimBlendSequence_Constructor  OLD__CAnimBlendSequence_Constructor = (h_CAnimBlendSequence_Constructor)0x4D0C10;
h_CAnimBlendSequence_Deconstructor OLD__CAnimBlendSequence_Deconstructor = (h_CAnimBlendSequence_Deconstructor)0x4D0C30; 

void _CAnimBlendSequence_Constructor (_CAnimBlendSequence * pSequence)
{
    WORD * pThis = (WORD *)pSequence;

    pThis[2] = 0;
    pThis[3] = 0;
    *((DWORD *)pThis + 2) = 0;
    *pThis = -1;
}

/*
// The keyframes will not be freed if the deconstructor is not there, but
// it would be better to put the frames in a vector container for better control.
// I prefer to not use the deconstructor, so I guess we don't need to uncomment this

_CAnimBlendSequence::~_CAnimBlendSequence()
{
    OLD__CAnimBlendSequence_Deconstructor();
}
*/
