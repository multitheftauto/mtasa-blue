/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CEventGroupSA.cpp
 *  PURPOSE:     Base event group
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CEvent* CEventGroupSA::Add(CEvent* pEvent, bool b_1)
{
    // CEventGroup::Add
    ((CEventSAInterface*(__cdecl*)(CEventGroupSAInterface*, CEventSAInterface*, bool))FUNC_CEventGroup_Add)(m_pInterface, pEvent->GetInterface(), b_1);

    return nullptr;
}
