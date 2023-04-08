/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CModelRPCs.cpp
 *  PURPOSE:     Custom models remote procedure calls
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CModelRPCs.h"

void CModelRPCs::LoadFunctions()
{
    AddHandler(ALLOCATE_MODEL_FROM_PARENT, AllocateModelFromParent, "AllocateModelFromParent");
    AddHandler(UNLOAD_MODEL, UnloadModel, "UnloadModel");
}

void CModelRPCs::AllocateModelFromParent(NetBitStreamInterface& bitStream)
{
    uint32_t uiNewModelID;
    uint32_t uiParentModelID;

    if (bitStream.Read(uiNewModelID) && bitStream.Read(uiParentModelID))
    {
        const bool status = m_pManager->GetModelManager()->AllocateModelFromParent(uiNewModelID, uiParentModelID);
        if (!status)
        {
            g_pCore->ShowNetErrorMessageBox(_("Error") + _E("CD21"), _("Server allocated existing model."));
            g_pCore->GetModManager()->RequestUnload();
        }
    }
}

void CModelRPCs::UnloadModel(NetBitStreamInterface& bitStream)
{
    uint32_t uiModelID;
    if (bitStream.Read(uiModelID))
    {
        m_pManager->GetModelManager()->Remove(uiModelID);
    }
}
