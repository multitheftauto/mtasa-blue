/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CCheckpointsSA.cpp
 *  PURPOSE:     Checkpoint entity manager
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CCheckpointsSA.h"
#include "CCheckpointSA.h"

CCheckpointsSA::CCheckpointsSA()
{
    for (auto i = 0; i < MAX_CHECKPOINTS; i++)
        Checkpoints[i] = new CCheckpointSA((CCheckpointSAInterface*)(ARRAY_CHECKPOINTS + i * sizeof(CCheckpointSAInterface)));
}

CCheckpointsSA::~CCheckpointsSA()
{
    for (auto i = 0; i < MAX_CHECKPOINTS; i++)
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
    /*
    static CCheckpoint  *PlaceMarker(std::uint32_t nIdentifier, std::uint16_t nType, CVector &vecPosition, CVector &pointDir,
    float fSize, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a, std::uint16_t nPeriod, float fPulseFrac, short nRotRate);
    */

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
    return NULL;
}

CCheckpoint* CCheckpointsSA::FindFreeMarker()
{
    for (auto i = 0; i < MAX_CHECKPOINTS; i++)
    {
        if (!Checkpoints[i]->IsActive())
            return Checkpoints[i];
    }
    return NULL;
}
