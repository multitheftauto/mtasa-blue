/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CMarker.cpp
 *  PURPOSE:     Marker entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CMarker.h"
#include "CMarkerManager.h"
#include "CColCircle.h"
#include "CColSphere.h"
#include "CResource.h"
#include "CLogger.h"
#include "Utils.h"
#include "CGame.h"
#include "packets/CElementRPCPacket.h"
#include <net/rpc_enums.h>

CMarker::CMarker(CMarkerManager* pMarkerManager, CColManager* pColManager, CElement* pParent) : CPerPlayerEntity(pParent)
{
    // Init
    m_pMarkerManager = pMarkerManager;
    m_pColManager = pColManager;
    m_iType = CElement::MARKER;
    SetTypeName("marker");
    m_ucType = TYPE_CHECKPOINT;
    m_fSize = 4.0f;
    m_Color = SColorRGBA(255, 255, 255, 255);
    m_bHasTarget = false;
    m_ucIcon = ICON_NONE;
    m_ignoreAlphaLimits = false;
    m_TargetArrowColor = SColorRGBA(255, 64, 64, 255);
    m_TargetArrowSize = m_fSize * 0.625f;

    // Create our collision object
    m_pCollision = new CColCircle(pColManager, nullptr, m_vecPosition, m_fSize, true);
    m_pCollision->SetCallback(this);
    m_pCollision->SetAutoCallEvent(false);
    m_pCollision->SetCanBeDestroyedByScript(false);

    // Add us to the marker manager
    pMarkerManager->AddToList(this);
    UpdateSpatialData();
}

CMarker::~CMarker()
{
    // Delete our collision object
    if (m_pCollision)
        delete m_pCollision;

    // Unlink from manager
    Unlink();
}

CElement* CMarker::Clone(bool* bAddEntity, CResource* pResource)
{
    CMarker* pTemp = m_pMarkerManager->Create(GetParentEntity());
    if (pTemp)
    {
        pTemp->SetMarkerType(GetMarkerType());
        pTemp->SetColor(GetColor());
        pTemp->SetSize(GetSize());
        if (pResource->IsClientSynced())
            pTemp->Sync(true);
        *bAddEntity = false;
    }
    return pTemp;
}

void CMarker::Unlink()
{
    // Remove us from the marker manager
    m_pMarkerManager->RemoveFromList(this);
}

bool CMarker::ReadSpecialData(const int iLine)
{
    // Grab the "posX" data
    if (!GetCustomDataFloat("posX", m_vecPosition.fX, true))
    {
        CLogger::ErrorPrintf("Bad/missing 'posX' attribute in <marker> (line %d)\n", iLine);
        return false;
    }

    // Grab the "posY" data
    if (!GetCustomDataFloat("posY", m_vecPosition.fY, true))
    {
        CLogger::ErrorPrintf("Bad/missing 'posY' attribute in <marker> (line %d)\n", iLine);
        return false;
    }

    // Grab the "posZ" data
    if (!GetCustomDataFloat("posZ", m_vecPosition.fZ, true))
    {
        CLogger::ErrorPrintf("Bad/missing 'posZ' attribute in <marker> (line %d)\n", iLine);
        return false;
    }

    // Set the position in the col object
    if (m_pCollision)
        m_pCollision->SetPosition(m_vecPosition);

    // Grab the "type" data
    char          szBuffer[128];
    unsigned char ucType;
    if (GetCustomDataString("type", szBuffer, 128, true))
    {
        // Convert it to a type
        ucType = static_cast<unsigned char>(CMarkerManager::StringToType(szBuffer));
        if (ucType == CMarker::TYPE_INVALID)
        {
            CLogger::LogPrintf("WARNING: Unknown 'type' value specified in <marker>; defaulting to \"default\" (line %d)\n", iLine);
            ucType = CMarker::TYPE_CHECKPOINT;
        }
    }
    else
    {
        ucType = CMarker::TYPE_CHECKPOINT;
    }
    SetMarkerType(ucType);

    // Grab the "color" data
    if (GetCustomDataString("color", szBuffer, 128, true))
    {
        // Convert the HTML-style color to RGB
        if (!XMLColorToInt(szBuffer, m_Color.R, m_Color.G, m_Color.B, m_Color.A))
        {
            CLogger::ErrorPrintf("Bad 'color' specified in <marker> (line %d)\n", iLine);
            return false;
        }
    }
    else
    {
        SetColor(SColorRGBA(255, 0, 0, 255));
    }

    float fSize;
    if (GetCustomDataFloat("size", fSize, true))
        m_fSize = fSize;

    int iTemp;
    if (GetCustomDataInt("dimension", iTemp, true))
        m_usDimension = static_cast<unsigned short>(iTemp);

    if (GetCustomDataInt("interior", iTemp, true))
        m_ucInterior = static_cast<unsigned char>(iTemp);

    // Success
    return true;
}

void CMarker::SetPosition(const CVector& vecPosition)
{
    // Different from our current position?
    if (m_vecPosition != vecPosition)
    {
        // Set the new position
        m_vecPosition = vecPosition;
        if (m_pCollision)
            m_pCollision->SetPosition(vecPosition);
        UpdateSpatialData();

        // If attached, client should handle the position correctly
        if (m_pAttachedTo)
            return;

        // We need to make sure the time context is replaced
        // before that so old packets don't arrive after this.
        GenerateSyncTimeContext();

        // Tell all the players that know about us
        CBitStream BitStream;
        BitStream.pBitStream->Write(vecPosition.fX);
        BitStream.pBitStream->Write(vecPosition.fY);
        BitStream.pBitStream->Write(vecPosition.fZ);
        BitStream.pBitStream->Write(GetSyncTimeContext());
        BroadcastOnlyVisible(CElementRPCPacket(this, SET_ELEMENT_POSITION, *BitStream.pBitStream));
    }
}

void CMarker::SetTarget(const CVector* pTargetVector)
{
    // Got a target vector?
    if (pTargetVector)
    {
        // Different from our current target
        if (!m_bHasTarget || m_vecTarget != *pTargetVector)
        {
            // Is this a marker type that has a destination?
            if (m_ucType == CMarker::TYPE_CHECKPOINT || m_ucType == CMarker::TYPE_RING)
            {
                // Set the target position
                m_bHasTarget = true;
                m_vecTarget = *pTargetVector;

                // Tell everyone that knows about this marker
                CBitStream BitStream;
                BitStream.pBitStream->Write(static_cast<unsigned char>(1));
                BitStream.pBitStream->Write(m_vecTarget.fX);
                BitStream.pBitStream->Write(m_vecTarget.fY);
                BitStream.pBitStream->Write(m_vecTarget.fZ);
                BroadcastOnlyVisible(CElementRPCPacket(this, SET_MARKER_TARGET, *BitStream.pBitStream));
            }
            else
            {
                // Reset the target position
                m_bHasTarget = false;
            }
        }
    }
    else
    {
        // Not already without target?
        if (m_bHasTarget)
        {
            // Reset the target position
            m_bHasTarget = false;

            // Is this a marker type that has a destination?
            if (m_ucType == CMarker::TYPE_CHECKPOINT || m_ucType == CMarker::TYPE_RING)
            {
                // Tell everyone that knows about this marker
                CBitStream BitStream;
                BitStream.pBitStream->Write(static_cast<unsigned char>(0));
                BroadcastOnlyVisible(CElementRPCPacket(this, SET_MARKER_TARGET, *BitStream.pBitStream));
            }
        }
    }
}

void CMarker::SetMarkerType(unsigned char ucType)
{
    // Different from our current type?
    if (ucType != m_ucType)
    {
        // Set the new type
        unsigned char ucOldType = m_ucType;
        m_ucType = ucType;
        UpdateCollisionObject(ucOldType);

        // Tell all players
        CBitStream BitStream;
        BitStream.pBitStream->Write(ucType);
        BroadcastOnlyVisible(CElementRPCPacket(this, SET_MARKER_TYPE, *BitStream.pBitStream));

        // Is the new type not a checkpoint or a ring? Remove the target
        if (ucType != CMarker::TYPE_CHECKPOINT && ucType != CMarker::TYPE_RING)
        {
            m_bHasTarget = false;
        }
    }
}

void CMarker::SetSize(float fSize)
{
    // Different from our current size?
    if (fSize != m_fSize)
    {
        // Set the new size and update the col object
        m_fSize = fSize;
        m_TargetArrowSize = fSize * 0.625f;

        UpdateCollisionObject(m_ucType);

        // Tell all players
        CBitStream BitStream;
        BitStream.pBitStream->Write(fSize);
        BroadcastOnlyVisible(CElementRPCPacket(this, SET_MARKER_SIZE, *BitStream.pBitStream));
    }
}

void CMarker::SetColor(const SColor color)
{
    // Different from our current color?
    if (color != m_Color)
    {
        // Set the new color
        m_Color = color;

        if (!m_ignoreAlphaLimits)
        {
            if (m_ucType == CMarker::TYPE_CHECKPOINT)
                m_Color.A = 128;
            else if (m_ucType == CMarker::TYPE_ARROW)
                m_Color.A = 255;
        }

        // Tell all the players
        CBitStream BitStream;
        BitStream.pBitStream->Write(m_Color.B);
        BitStream.pBitStream->Write(m_Color.G);
        BitStream.pBitStream->Write(m_Color.R);
        BitStream.pBitStream->Write(m_Color.A);
        BroadcastOnlyVisible(CElementRPCPacket(this, SET_MARKER_COLOR, *BitStream.pBitStream));
    }
}

void CMarker::SetIcon(unsigned char ucIcon)
{
    if (m_ucIcon != ucIcon)
    {
        m_ucIcon = ucIcon;

        // Tell everyone that knows about this marker
        CBitStream BitStream;
        BitStream.pBitStream->Write(m_ucIcon);
        BroadcastOnlyVisible(CElementRPCPacket(this, SET_MARKER_ICON, *BitStream.pBitStream));
    }
}

void CMarker::SetTargetArrowProperties(const SColor color, float size) noexcept
{
    if (m_TargetArrowColor == color && m_TargetArrowSize == size)
        return;

    m_TargetArrowColor = color;
    m_TargetArrowSize = size;

    CBitStream BitStream;
    BitStream.pBitStream->Write(color.R);
    BitStream.pBitStream->Write(color.G);
    BitStream.pBitStream->Write(color.B);
    BitStream.pBitStream->Write(color.A);
    BitStream.pBitStream->Write(size);
    BroadcastOnlyVisible(CElementRPCPacket(this, SET_MARKER_TARGET_ARROW_PROPERTIES, *BitStream.pBitStream));
}

void CMarker::Callback_OnCollision(CColShape& Shape, CElement& Element)
{
    // Do not call on ourselves #7359
    if (this == &Element)
        return;

    // Matching interior?
    if (GetInterior() == Element.GetInterior())
    {
        // Call the marker hit event
        CLuaArguments Arguments;
        Arguments.PushElement(&Element);                                            // Hit element
        Arguments.PushBoolean(GetDimension() == Element.GetDimension());            // Matching dimension?
        CallEvent("onMarkerHit", Arguments);

        if (IS_PLAYER(&Element))
        {
            CLuaArguments Arguments2;
            Arguments2.PushElement(this);                                                // marker
            Arguments2.PushBoolean(GetDimension() == Element.GetDimension());            // Matching dimension?
            Element.CallEvent("onPlayerMarkerHit", Arguments2);
        }
    }
}

void CMarker::Callback_OnLeave(CColShape& Shape, CElement& Element)
{
    // Matching interior?
    if (GetInterior() == Element.GetInterior())
    {
        // Call the marker hit event
        CLuaArguments Arguments;
        Arguments.PushElement(&Element);                                            // Hit element
        Arguments.PushBoolean(GetDimension() == Element.GetDimension());            // Matching dimension?
        CallEvent("onMarkerLeave", Arguments);

        if (IS_PLAYER(&Element))
        {
            CLuaArguments Arguments2;
            Arguments2.PushElement(this);                                                // marker
            Arguments2.PushBoolean(GetDimension() == Element.GetDimension());            // Matching dimension?
            Element.CallEvent("onPlayerMarkerLeave", Arguments2);
        }
    }
}

void CMarker::Callback_OnCollisionDestroy(CColShape* pCollision)
{
    if (pCollision == m_pCollision)
        m_pCollision = NULL;
}

void CMarker::UpdateCollisionObject(unsigned char ucOldType)
{
    // Different type than before?
    if (m_ucType != ucOldType)
    {
        // Are we becomming a checkpoint? Make the col object a circle. If we're becomming something and we were a checkpoint, make the col object a sphere.

        // lil_Toady: Simply deleting the colshape may cause a dangling pointer in CColManager if we're in DoHitDetection loop
        if (m_ucType == CMarker::TYPE_CHECKPOINT)
        {
            if (m_pCollision)
                g_pGame->GetElementDeleter()->Delete(m_pCollision);

            m_pCollision = new CColCircle(m_pColManager, nullptr, m_vecPosition, m_fSize, true);
        }
        else if (ucOldType == CMarker::TYPE_CHECKPOINT)
        {
            if (m_pCollision)
                g_pGame->GetElementDeleter()->Delete(m_pCollision);

            m_pCollision = new CColSphere(m_pColManager, nullptr, m_vecPosition, m_fSize, true);
        }

        m_pCollision->SetCallback(this);
        m_pCollision->SetAutoCallEvent(false);
        m_pCollision->SetCanBeDestroyedByScript(false);
    }

    // Set the radius after the size
    if (m_ucType == CMarker::TYPE_CHECKPOINT)
    {
        static_cast<CColCircle*>(m_pCollision)->SetRadius(m_fSize);
    }
    else
    {
        static_cast<CColSphere*>(m_pCollision)->SetRadius(m_fSize);
    }
}

CSphere CMarker::GetWorldBoundingSphere()
{
    return CSphere(GetPosition(), GetSize());
}
