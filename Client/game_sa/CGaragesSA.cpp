/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CGaragesSA.cpp
 *  PURPOSE:     Garage manager
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CGaragesSA.h"

CGaragesSA::CGaragesSA(CGaragesSAInterface* pInterface)
{
    internalInterface = pInterface;

    for (int i = 0; i < MAX_GARAGES; i++)
    {
        Garages[i] = new CGarageSA((CGarageSAInterface*)(CLASS_CGarages + 216 * i));
    }
}

CGaragesSA::~CGaragesSA()
{
    for (int i = 0; i < MAX_GARAGES; i++)
    {
        delete Garages[i];
        Garages[i] = NULL;
    }
}

void CGaragesSA::Initialize()
{
    // Disable pay and sprays and mod shops
    static const int iPayAndSprays[] = {7, 8, 10, 11, 12, 15, 18, 19, 24, 27, 32, 33, 36, 40, 41, 47};
    for (unsigned int i = 0; i < sizeof(iPayAndSprays) / sizeof(int); ++i)
    {
        Garages[iPayAndSprays[i]]->SetType(1);
    }
}

CGarageSA* CGaragesSA::GetGarage(DWORD dwID)
{
    if (dwID >= 0 && dwID < MAX_GARAGES)
    {
        return Garages[dwID];
    }
    return NULL;
}
