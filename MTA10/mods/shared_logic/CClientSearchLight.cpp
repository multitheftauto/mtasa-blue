/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientSearchLight.cpp
*  PURPOSE:     HeliLight entity class source
*
*****************************************************************************/
#include <StdInc.h>

CClientSearchLight::CClientSearchLight ( CClientManager* pManager, ElementID ID ) : ClassInit ( this ), CClientStreamElement ( pManager->GetLightStreamer (), ID )
{
    m_pManager = pManager;
    pManager->GetPointLightsManager ()->AddToSearchLightList ( this );

    SetTypeName ( "searchlight" );
}

CClientSearchLight::~CClientSearchLight ()
{
    Unlink ();
}

void CClientSearchLight::Unlink ()
{
    g_pClientGame->GetManager ()->GetPointLightsManager ()->RemoveSearchLightFromList ( this );
}

void CClientSearchLight::Render ()
{
    DoAttaching ();

    if ( IsStreamedIn () )
        g_pGame->GetPointLights ()->RenderHeliLight ( m_StartPosition, m_EndPosition, m_StartRadius, m_EndRadius, m_bRenderSpot );
}
