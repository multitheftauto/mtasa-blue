/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CCheckpointsSA.cpp
 *  PURPOSE:     Checkpoint entity manager
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CCheckpointsSA.h"
#include "CCheckpointSA.h"

CCheckpointsSA::CCheckpointsSA()
{
    for (int i = 0; i < MAX_CHECKPOINTS; i++)
        Checkpoints[i] = new CCheckpointSA((CCheckpointSAInterface*)(0xC7F158 + i * sizeof(CCheckpointSAInterface)));
}

CCheckpointsSA::~CCheckpointsSA()
{
    for (int i = 0; i < MAX_CHECKPOINTS; i++)
    {
        delete Checkpoints[i];
    }
}

/**
 * \todo Update default color to SA's orange instead of VC's pink
 */
CCheckpoint* CCheckpointsSA::CreateCheckpoint(DWORD Identifier, WORD wType, CVector* vecPosition, CVector* vecPointDir, float fSize, float fPulseFraction,
                                              const SharedUtil::SColor color)
{
    CCheckpoint* Checkpoint = FindFreeMarker();

    if (Checkpoint)
    {
        ((CCheckpointSA*)(Checkpoint))->SetIdentifier(Identifier);
        ((CCheckpointSA*)(Checkpoint))->Activate();
        ((CCheckpointSA*)(Checkpoint))->SetType(wType);
        Checkpoint->SetPosition(vecPosition);
        Checkpoint->SetPointDirection(vecPointDir);
        Checkpoint->SetSize(fSize);
        Checkpoint->SetColor(color);
        Checkpoint->SetPulsePeriod(1024);
        ((CCheckpointSA*)(Checkpoint))->SetPulseFraction(fPulseFraction);
        Checkpoint->SetRotateRate(1);

        return Checkpoint;
    }

    return nullptr;
}

CCheckpoint* CCheckpointsSA::FindFreeMarker()
{
    for (int i = 0; i < MAX_CHECKPOINTS; i++)
    {
        if (!Checkpoints[i]->IsActive())
            return Checkpoints[i];
    }
    return nullptr;
}
