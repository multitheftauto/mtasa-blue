/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientDisplay.cpp
*  PURPOSE:     Text display class
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

#include <StdInc.h>

CClientDisplay::CClientDisplay ( CClientDisplayManager* pDisplayManager, unsigned long ulID )
{
    m_pDisplayManager = pDisplayManager;
    m_ulID = ulID;

    m_ulExpirationTime = 0;
    m_bVisible = true;
    m_rgbaColor = 0xFFFFFFFF;

    m_pDisplayManager->AddToList ( this );
}


CClientDisplay::~CClientDisplay ( void )
{
    // Remove us from the manager
    m_pDisplayManager->RemoveFromList ( this );
}


void CClientDisplay::SetColorAlpha ( unsigned char ucAlpha )
{
    unsigned char ucRGBA [4];
    GetColor ( ucRGBA [0], ucRGBA [1], ucRGBA [2], ucRGBA [3] );
    ucRGBA [3] = ucAlpha;
    SetColor ( ucRGBA [0], ucRGBA [1], ucRGBA [2], ucRGBA [3] );
}