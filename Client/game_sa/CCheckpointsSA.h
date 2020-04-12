/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CCheckpointsSA.h
 *  PURPOSE:     Header file for checkpoint entity manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

// 00722c40      public: static class CCheckpoint * __cdecl CCheckpoints::PlaceMarker(unsigned int,unsigned short,class CVector &,class CVector &,float,unsigned
// char,unsigned char,unsigned char,unsigned char,unsigned short,float,short)
#define FUNC_CCheckpoints__PlaceMarker  0x722c40 // ##SA##

#define MAX_CHECKPOINTS         32

#define ARRAY_CHECKPOINTS       0xC7F158

#include <game/CCheckpoints.h>
#include "CCheckpointSA.h"

class CCheckpointSA;

class CCheckpointsSA : public CCheckpoints
{
private:
    CCheckpointSA* Checkpoints[MAX_CHECKPOINTS];

public:
    // constructor
    CCheckpointsSA();
    ~CCheckpointsSA();

    CCheckpoint* CreateCheckpoint(DWORD Identifier, WORD wType, CVector* vecPosition, CVector* vecPointDir, FLOAT fSize, FLOAT fPulseFraction,
                                  const SColor color);
    CCheckpoint* FindFreeMarker();
};
