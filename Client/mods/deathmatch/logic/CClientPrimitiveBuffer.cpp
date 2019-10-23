/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CClientPrimitiveBuffer.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
CClientPrimitiveBuffer::CClientPrimitiveBuffer(class CClientManager* pManager, ElementID ID) : CClientEntity(ID)
{
    m_pManager = pManager;
    SetTypeName("PrimitiveBuffer");
}

void CClientPrimitiveBuffer::Unlink()
{
}
