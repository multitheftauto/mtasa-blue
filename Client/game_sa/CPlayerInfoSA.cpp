/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPlayerInfoSA.cpp
 *  PURPOSE:     Player ped type information
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerInfoSA.h"

CWanted* CPlayerInfoSA::GetWanted()
{
    if (!wanted)
        wanted = new CWantedSA(internalInterface->PlayerPedData.m_Wanted);

    return wanted;
}

/**
 * Gets the amount of money the player has
 * @return DWORD containing the ammount of money the player has
 */
long CPlayerInfoSA::GetPlayerMoney()
{
    // return internalInterface->DisplayScore;
    return *(long*)(0xB7CE50);
}

/**
 * Sets the amount of money the player has
 * @param dwMoney DWORD containing the ammount of money you wish the player to have
 */
void CPlayerInfoSA::SetPlayerMoney(long lMoney, bool bInstant)
{
    MemPutFast<long>(0xB7CE50, lMoney);

    if (bInstant)
        MemPutFast<long>(0xB7CE54, lMoney);
}

float CPlayerInfoSA::GetFPSMoveHeading()
{
    return GetInterface()->PlayerPedData.m_fFPSMoveHeading;
}

void CPlayerInfoSA::SetDoesNotGetTired(bool bDoesNotGetTired)
{
    internalInterface->DoesNotGetTired = bDoesNotGetTired;
}

void CPlayerInfoSA::SetLastTimeEaten(short sTime)
{
    internalInterface->TimeLastEaten = sTime;
}

byte CPlayerInfoSA::GetCamDrunkLevel()
{
    return internalInterface->PlayerPedData.m_nDrunkenness;
}

void CPlayerInfoSA::SetCamDrunkLevel(byte level)
{
    internalInterface->PlayerPedData.m_nDrunkenness = level;
    internalInterface->PlayerPedData.m_bFadeDrunkenness = level == 0;

    // SCM opcode handler for 0x52C sets 0xB7CD64 which is the game speed additionally to 0
}
