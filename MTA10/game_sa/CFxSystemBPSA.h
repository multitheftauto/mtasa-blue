/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/FxSystemBP_cSA.h
*  PURPOSE:     Game effects blueprint interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CFxSystemBPSA
#define __CFxSystemBPSA

class CFxSystemBPSAInterface 
{
public:
    // inherited from ListItem_c
    CFxSystemBPSAInterface * pPrevious;       // 0x00
    CFxSystemBPSAInterface * pNext;           // 0x04

    // Actual members
    char * szNameHash;              // 0x08
    float fLength;                  // 0x0C
    float fLoopIntervalMinimum;     // 0x10
    float fLoopIntervalLength;      // 0x14
    short sCullDistance;            // 0x18
    char cPlayMode;                 // 0x1A
    char cSizeOfPrims;              // 0x1B (This is actually the size in bytes of the following array of pointers)
    void ** pPrims;                 // 0x1C (pPrims[0] to pPrims[(cSizeOfPrims/4)-1])
    void * pBoundingSphere;         // 0x20
};
C_ASSERT(sizeof(CFxSystemBPSAInterface) == 0x24);

#endif