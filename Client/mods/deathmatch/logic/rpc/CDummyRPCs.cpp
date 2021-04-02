/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CDummyRPCs.cpp
 *  PURPOSE:     Dummy remote procedure calls
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CDummyRPCs.h"

void CDummyRPCs::LoadFunctions()
{
    AddHandler(SET_DUMMY_ROTATION, SetDummyRotation);
}

void CDummyRPCs::SetDummyRotation(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    // Grab the dummy element
    CClientDummy* pDummy = static_cast<CClientDummy*>(m_pGroups->Get(pSource->GetID()));
    if (pDummy)
    {
        // Read out the new rotation
        CVector vecRotation;
        if (bitStream.Read(vecRotation.fX) && bitStream.Read(vecRotation.fY) && bitStream.Read(vecRotation.fZ))
        {
            pDummy->SetRotation(vecRotation);
        }
    }
}
