/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CMapEvent.cpp
*  PURPOSE:     Map event class
*  DEVELOPERS:  Jax <>
*               Oliver Brown <>
*               Kevin Whiteside <kevuwk@gmail.com>
*
*****************************************************************************/

#include <StdInc.h>

CMapEvent::CMapEvent ( CLuaMain* pMain, const char* szName, const CLuaFunctionRef& iLuaFunction, bool bPropagated, EEventPriorityType eventPriority, float fPriorityMod )
{
    // Init
    m_pMain = pMain;
    m_bDestroyFunction = false;
    m_iLuaFunction = iLuaFunction;
    m_bPropagated = bPropagated;
    m_bBeingDestroyed = false;
    m_eventPriority = eventPriority;
    m_fPriorityMod = fPriorityMod;
    m_strName.AssignLeft ( szName, MAPEVENT_MAX_LENGTH_NAME );
    // Only allow dxSetAspectRatioAdjustmentEnabled during these events
    m_bAllowAspectRatioAdjustment = ( m_strName == "onClientRender" ) || ( m_strName == "onClientPreRender" ) || ( m_strName == "onClientHUDRender" );
    // Force aspect ratio adjustment for 'customblips' resource
    m_bForceAspectRatioAdjustment = m_bAllowAspectRatioAdjustment && SStringX( pMain->GetScriptName() ) == "customblips";
}


CMapEvent::~CMapEvent ( void )
{
}


void CMapEvent::Call ( const CLuaArguments& Arguments )
{
    if ( m_pMain )
    {
        // Call our function with the given arguments
        Arguments.Call ( m_pMain, m_iLuaFunction );
    }
}


bool CMapEvent::IsHigherPriorityThan ( const CMapEvent* pOther )
{
    return m_eventPriority > pOther->m_eventPriority ||
         ( m_eventPriority == pOther->m_eventPriority &&
           m_fPriorityMod > pOther->m_fPriorityMod );
}
