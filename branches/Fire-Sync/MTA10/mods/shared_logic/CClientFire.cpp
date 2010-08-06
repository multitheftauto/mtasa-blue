/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientFire.cpp
*  PURPOSE:     Fire Entity Class
*  DEVELOPERS:  Fedor Sinev
*
*****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

CClientFire::CClientFire ( CClientManager* pManager, ElementID ID, CVector& vecPosition, float fSize, bool bSilent ) : CClientEntity ( ID )
{
    // TODO: Add a server-side fire manager
    m_pManager = pManager->GetFireManager ( ); // needs verification
    m_pFire = g_pGame->GetFireManager ( )->StartFire ( vecPosition, fSize );
    m_pFireInterface->bSilent = false;
    SetNumOfGenerationsAllowed ( 0 );
    SetSilent ( bSilent );
    SetTypeName ( "fire" );
    m_pManager->AddToList ( this ); 
}

CClientFire::~CClientFire ( )
{
    Unlink ( );
    if ( m_pFire )
        Extinguish ( );
}

void CClientFire::Unlink ( )
{
    m_pManager->RemoveFromList ( this );
}