/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTXD.cpp
*  PURPOSE:     Animation manager class
*  DEVELOPERS:  Cazomino05 <>
*
*****************************************************************************/

#include <StdInc.h>

CClientIfp::CClientIfp ( class CClientManager* pManager, ElementID ID ) : CClientEntity ( ID )
{
    // Init
    m_pManager = pManager;

    SetTypeName ( "IFP" );

    m_pAnimBlock = NULL;

    m_iAnimBlock = 0;

}


CClientIfp::~CClientIfp ( void )
{
    Remove();
}
bool CClientIfp::LoadIFP ( const char* szFile )
{
    // Are we not already loaded?
    if ( !m_pAnimBlock )
    {
        // Try to load it
        CAnimBlock* pIFP = g_pGame->GetRenderWare ()->ReadIFP ( szFile );
        if ( pIFP )
        {
            m_pAnimBlock = pIFP;
            m_iAnimBlock = g_pGame->GetAnimManager()->RegisterAnimBlock( m_pAnimBlock->GetName() );
            return true;
        }
    }

    // Failed
    return false;
}


void CClientIfp::Remove ()
{
    // Remove the model
    if ( m_pAnimBlock ) {
        g_pGame->GetAnimManager()->RemoveLastAnimFile ( );
        g_pGame->GetAnimManager()->RemoveAnimBlockRefWithoutDelete ( m_iAnimBlock );
        g_pGame->GetAnimManager()->RemoveAnimBlock ( m_iAnimBlock );


    }
}