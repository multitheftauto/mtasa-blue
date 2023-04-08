/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/models/CModelAtomic.cpp
 *  PURPOSE:     Atomic model class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CModelAtomic.h"
#include "CGame.h"
#include "CObjectManager.h"

CModelAtomic* CModelAtomic::Clone(uint32_t uiModelID)
{
    CModelAtomic* pNewModel = new CModelAtomic(uiModelID);

    pNewModel->SetParentModel(m_uiModelID);

    return pNewModel;
}

void CModelAtomic::Unload()
{
    auto iterBegin = g_pGame->GetObjectManager()->IterBegin();
    auto iterEnd = g_pGame->GetObjectManager()->IterEnd();
    for (auto iter = iterBegin; iterBegin != iterEnd; ++iterBegin)
    {
        CObject* pObject = *iter;
        if (pObject->GetModel() == m_uiModelID)
        {
            pObject->SetModel(m_uiParentID);

            CLuaArguments Arguments;
            Arguments.PushNumber(m_uiModelID);
            Arguments.PushNumber(m_uiParentID);
            pObject->CallEvent("onElementModelChange", Arguments);
        }
    }
}
