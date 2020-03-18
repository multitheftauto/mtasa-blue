/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientAssetInstance.cpp
 *  PURPOSE:     AssetInstance entity class
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CClientAssetInstance.h"

CClientAssetInstance::CClientAssetInstance(class CClientManager* pManager, ElementID ID) : ClassInit(this), CClientEntity(ID)
{
    // Init
    m_pManager = pManager;
    m_pAssetInstanceManager = pManager->GetAssetInstanceManager();
    SetTypeName("asset-instance");

    // Add us to the manager's list
    m_pAssetInstanceManager->AddToList(this);
}

CClientAssetInstance::~CClientAssetInstance()
{
}

void CClientAssetInstance::Unlink()
{
    m_pAssetInstanceManager->RemoveFromList(this);
}


void CClientAssetInstance::DoPulse()
{
    // Update our position/rotation if we're attached
    // DoAttaching();
}
