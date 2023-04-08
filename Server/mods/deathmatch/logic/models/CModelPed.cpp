/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/models/CModelPed.cpp
 *  PURPOSE:     Ped model class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CModelPed.h"
#include "CGame.h"
#include "CPlayerManager.h"
#include "CPedManager.h"

CModelPed* CModelPed::Clone(uint32_t uiModelID)
{
    CModelPed* pNewModel = new CModelPed(uiModelID);

    pNewModel->SetParentModel(m_uiModelID);

    return pNewModel;
}

void CModelPed::Unload()
{
    auto unloadModelsAndCallEvents = [&](auto iterBegin, auto iterEnd, auto setElementModelLambda) {
        for (auto iter = iterBegin; iter != iterEnd; iter++)
        {
            auto& element = **iter;

            if (element.GetModel() != m_uiModelID)
                continue;

            setElementModelLambda(element);

            CLuaArguments Arguments;
            Arguments.PushNumber(m_uiModelID);
            Arguments.PushNumber(m_uiParentID);
            element.CallEvent("onElementModelChange", Arguments);
        }
    };

    CPedManager* pPedManager = g_pGame->GetPedManager();
    unloadModelsAndCallEvents(pPedManager->IterBegin(), pPedManager->IterEnd(), [&](auto& element) { element.SetModel(m_uiParentID); });

    CPlayerManager* pPlayerManager = g_pGame->GetPlayerManager();
    unloadModelsAndCallEvents(pPlayerManager->IterBegin(), pPlayerManager->IterEnd(), [&](auto& element) { element.SetModel(m_uiParentID); });
}
