/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CMarkerRPCs.cpp
 *  PURPOSE:     Marker remote procedure calls
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CModelRPCs.h"

void CModelRPCs::LoadFunctions()
{
    AddHandler(ALLOCATE_MODEL_FROM_PARENT, AllocateModelFromParent, "AllocateModelFromParent");
}

void CModelRPCs::AllocateModelFromParent(NetBitStreamInterface& bitStream)
{
    uint32_t uiNewModelID;
    uint32_t uiParentModelID;

    if (bitStream.Read(uiNewModelID) && bitStream.Read(uiParentModelID))
    {
        m_pManager->GetModelManager()->AllocateModelFromParent(uiNewModelID, uiParentModelID);
    }
}
