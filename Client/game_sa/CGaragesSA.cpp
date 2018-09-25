/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CGaragesSA.cpp
 *  PURPOSE:     Garage manager
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

// Disables pay and sprays (making sounds when closing) / https://wiki.multitheftauto.com/wiki/Garage
static const int iDisabledGarages[] = {1, 8, 11, 12, 19, 23, 26, 27, 29, 32, 36, 40, 41, 47};

CGaragesSA::CGaragesSA(CGaragesSAInterface* pInterface)
{
    this->internalInterface = pInterface;

    for (int i = 0; i < MAX_GARAGES; i++)
    {
        this->Garages[i] = new CGarageSA((CGarageSAInterface*)(CLASS_CGarages + 216 * i));
    }
}

CGaragesSA::~CGaragesSA()
{
    for (int i = 0; i < MAX_GARAGES; i++)
    {
        delete this->Garages[i];
        this->Garages[i] = NULL;
    }
}

void CGaragesSA::Initialize()
{
    for (unsigned int i = 0; i < sizeof(iDisabledGarages) / sizeof(int); ++i)
    {
        this->Garages[iDisabledGarages[i]]->SetType(1);
    }
}

CGarageSA* CGaragesSA::GetGarage(DWORD dwID)
{
    bool bIsDisabled = false;

    // Check if garage has been disabled
    for (unsigned int i = 0; i < sizeof(iDisabledGarages) / sizeof(int); ++i)
    {
        if (dwID == iDisabledGarages[i])
        {
            bIsDisabled = true;
            break;
        }
    }

    if (!bIsDisabled && dwID >= 0 && dwID < MAX_GARAGES)
    {
        return this->Garages[dwID];
    }

    return NULL;
}
