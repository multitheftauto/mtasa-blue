/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CObjectRPCs.cpp
 *  PURPOSE:     Object remote procedure calls
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CObjectRPCs.h"

void CObjectRPCs::LoadFunctions()
{
    AddHandler(DESTROY_ALL_OBJECTS, DestroyAllObjects, "DestroyAllObjects");
    AddHandler(SET_OBJECT_ROTATION, SetObjectRotation, "SetObjectRotation");
    AddHandler(MOVE_OBJECT, MoveObject, "MoveObject");
    AddHandler(STOP_OBJECT, StopObject, "StopObject");
    AddHandler(SET_OBJECT_SCALE, SetObjectScale, "SetObjectScale");
    AddHandler(SET_OBJECT_VISIBLE_IN_ALL_DIMENSIONS, SetObjectVisibleInAllDimensions, "SetObjectVisibleInAllDimensions");
    AddHandler(SET_OBJECT_BREAKABLE, SetObjectBreakable, "SetObjectBreakable");
    AddHandler(BREAK_OBJECT, BreakObject, "BreakObject");
    AddHandler(RESPAWN_OBJECT, RespawnObject, "RespawnObject");
    AddHandler(TOGGLE_OBJECT_RESPAWN, ToggleObjectRespawn, "ToggleObjectRespawn");
}

void CObjectRPCs::DestroyAllObjects(NetBitStreamInterface& bitStream)
{
    m_pObjectManager->DeleteAll();
}

void CObjectRPCs::SetObjectRotation(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    // Grab the object
    CDeathmatchObject* pObject = static_cast<CDeathmatchObject*>(m_pObjectManager->Get(pSource->GetID()));
    if (pObject)
    {
        // Read out the new rotation
        CVector vecRotation;
        if (bitStream.Read(vecRotation.fX) && bitStream.Read(vecRotation.fY) && bitStream.Read(vecRotation.fZ))
        {
            // Set the new rotation
            pObject->SetRotationRadians(vecRotation);

            // Kayl: removed update of target rotation, move uses delta and anyway setRotation is NOT possible when moving
        }
    }
}

void CObjectRPCs::MoveObject(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    // Grab the object
    CDeathmatchObject* pObject = static_cast<CDeathmatchObject*>(m_pObjectManager->Get(pSource->GetID()));
    if (pObject)
    {
        CPositionRotationAnimation* pAnimation = CPositionRotationAnimation::FromBitStream(bitStream);

        if (pAnimation)
        {
            pObject->StartMovement(*pAnimation);
            delete pAnimation;
        }
    }
}

void CObjectRPCs::StopObject(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    // Grab the object
    CDeathmatchObject* pObject = static_cast<CDeathmatchObject*>(m_pObjectManager->Get(pSource->GetID()));
    if (pObject)
    {
        // Read out the position and rotation
        CVector vecSourcePosition;
        CVector vecSourceRotation;
        if (bitStream.Read(vecSourcePosition.fX) && bitStream.Read(vecSourcePosition.fY) && bitStream.Read(vecSourcePosition.fZ) &&
            bitStream.Read(vecSourceRotation.fX) && bitStream.Read(vecSourceRotation.fY) && bitStream.Read(vecSourceRotation.fZ))
        {
            // Stop the movement
            pObject->StopMovement();
            // Set it to the source position and rotation
            pObject->SetOrientation(vecSourcePosition, vecSourceRotation);
        }
    }
}

void CObjectRPCs::SetObjectScale(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    CDeathmatchObject* pObject = static_cast<CDeathmatchObject*>(m_pObjectManager->Get(pSource->GetID()));
    if (pObject)
    {
        CVector vecScale;

        bitStream.Read(vecScale.fX);
        vecScale.fY = vecScale.fX;
        vecScale.fZ = vecScale.fX;
        if (bitStream.Version() >= 0x40)
        {
            bitStream.Read(vecScale.fY);
            bitStream.Read(vecScale.fZ);
        }
        pObject->SetScale(vecScale);
    }
}

void CObjectRPCs::SetObjectVisibleInAllDimensions(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    CDeathmatchObject* pObject = static_cast<CDeathmatchObject*>(m_pObjectManager->Get(pSource->GetID()));

    if (pObject)
    {
        bool           bVisible;
        unsigned short usNewDimension;

        bitStream.ReadBit(bVisible);
        bitStream.Read(usNewDimension);

        pObject->SetVisibleInAllDimensions(bVisible, usNewDimension);
    }
}

void CObjectRPCs::SetObjectBreakable(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    CDeathmatchObject* pObject = static_cast<CDeathmatchObject*>(m_pObjectManager->Get(pSource->GetID()));

    if (pObject)
    {
        pObject->SetBreakable(bitStream.ReadBit());
    }
}

void CObjectRPCs::BreakObject(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
   auto* pObject = static_cast<CDeathmatchObject*>(m_pObjectManager->Get(pSource->GetID()));

    if (pObject)
        pObject->Break();
}

void CObjectRPCs::RespawnObject(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    auto* pObject = static_cast<CDeathmatchObject*>(m_pObjectManager->Get(pSource->GetID()));

    if (pObject)
        g_pClientGame->GetObjectRespawner()->Respawn(pObject);
}

void CObjectRPCs::ToggleObjectRespawn(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    auto* pObject = static_cast<CDeathmatchObject*>(m_pObjectManager->Get(pSource->GetID()));

    if (pObject)
        pObject->SetRespawnEnabled(bitStream.ReadBit());
}
