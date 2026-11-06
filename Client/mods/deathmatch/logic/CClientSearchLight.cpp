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
#include <game/CPointLights.h>

CClientSearchLight::CClientSearchLight(CClientManager* pManager, ElementID ID) : ClassInit(this), CClientStreamElement(pManager->GetLightStreamer(), ID)
{
    m_pManager = pManager;
    pManager->GetPointLightsManager()->AddToSearchLightList(this);

    SetTypeName("searchlight");
}

CClientSearchLight::~CClientSearchLight()
{
    Unlink();
}

void CClientSearchLight::Unlink()
{
    g_pClientGame->GetManager()->GetPointLightsManager()->RemoveSearchLightFromList(this);
}

void CClientSearchLight::Render()
{
    DoAttaching();

    if (!IsStreamedIn())
        return;

    // Tell the texture replacer which element the cone belongs to
    CRenderWare* pRenderWare = g_pGame->GetRenderWare();
    pRenderWare->SetRenderingClientEntity(this, 0xFFFF, TYPE_MASK_OTHER);
    g_pGame->GetPointLights()->RenderHeliLight(m_StartPosition, m_EndPosition, m_StartRadius, m_EndRadius, m_bRenderSpot, m_color);
    pRenderWare->SetRenderingClientEntity(nullptr, 0xFFFF, TYPE_MASK_WORLD);
}
