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
    SetTypeName ( "fire" );
    m_pManager->AddToList ( this ); 
}

CClientFire::~CClientFire ( )
{
    Unlink ( );
    if ( m_pFire )
        m_pFire->Extinguish ( );
}

void CClientFire::GetPosition ( CVector& vecPosition ) const
{
    vecPosition = *m_pFire->GetPosition ( );
}

void CClientFire::SetPosition ( const CVector& vecPosition )
{
    //m_pFire->SetPosition ( vecPosition );
}

void CClientFire::Unlink ( ) // apparently necessary.
{
    m_pManager->RemoveFromList ( this );
}