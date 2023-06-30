/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CCheckpointsSA.h
 *  PURPOSE:     Header file for checkpoint entity manager class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CCheckpoints.h>

class CCheckpointSA;
class CVector;

#define MAX_CHECKPOINTS         32

class CCheckpointsSA : public CCheckpoints
{
private:
    CCheckpointSA* Checkpoints[MAX_CHECKPOINTS];

public:
    CCheckpointsSA();
    ~CCheckpointsSA();

    CCheckpoint* CreateCheckpoint(DWORD Identifier, WORD wType, CVector* vecPosition, CVector* vecPointDir, float fSize, float fPulseFraction,
                                  const SharedUtil::SColor color);
    CCheckpoint* FindFreeMarker();
};
