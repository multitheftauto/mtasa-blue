/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMapEvent.cpp
*  PURPOSE:     Map event class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Chris McArthur <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CMapEvent::CMapEvent(CLuaMain* pMain, std::string strName, const CLuaFunctionRef& iLuaFunction, bool bPropagated, EEventPriorityType eventPriority, float fPriorityMod)
{
    // Init
    m_pMain = pMain;
    m_bDestroyFunction = false;
    m_iLuaFunction = iLuaFunction;
    m_bPropagated = bPropagated;
    m_bBeingDestroyed = false;
    m_eventPriority = eventPriority;
    m_fPriorityMod = fPriorityMod;
    m_strName = strName;
#ifdef MTA_CLIENT
    // Only allow dxSetAspectRatioAdjustmentEnabled during these events
    m_bAllowAspectRatioAdjustment = (m_strName == "onClientRender") || (m_strName == "onClientPreRender") || (m_strName == "onClientHUDRender");
    // Force aspect ratio adjustment for 'customblips' resource
    m_bForceAspectRatioAdjustment = m_bAllowAspectRatioAdjustment && SStringX(pMain->GetScriptName()) == "customblips";
#endif
}

void CMapEvent::Call(const CLuaArguments& Arguments)
{
    if (m_pMain)
    {
        // Call our function with the given arguments
        Arguments.Call(m_pMain, m_iLuaFunction);
    }
}
