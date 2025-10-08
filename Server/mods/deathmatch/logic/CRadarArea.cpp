/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CRadarArea.cpp
 *  PURPOSE:     Radar area entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CRadarArea.h"
#include "CRadarAreaManager.h"
#include "CResource.h"
#include "CLogger.h"
#include "Utils.h"
#include "packets/CElementRPCPacket.h"
#include <net/rpc_enums.h>

CRadarArea::CRadarArea(CRadarAreaManager* pRadarAreaManager, CElement* pParent) : CPerPlayerEntity(pParent)
{
    // Init
    m_iType = CElement::RADAR_AREA;
    SetTypeName("radararea");
    m_pRadarAreaManager = pRadarAreaManager;
    m_Color = SColorRGBA(255, 255, 255, 255);
    m_bIsFlashing = false;

    // Pop an id for us and add us to the manger's list
    pRadarAreaManager->AddToList(this);
}

CRadarArea::~CRadarArea()
{
    // Unlink us from manager
    Unlink();
}

CElement* CRadarArea::Clone(bool* bAddEntity, CResource* pResource)
{
    CRadarArea* const pTemp = m_pRadarAreaManager->Create(GetParentEntity());

    if (pTemp)
    {
        pTemp->SetSize(GetSize());
        pTemp->SetColor(GetColor());
        if (pResource->IsClientSynced())
            pTemp->Sync(true);
        *bAddEntity = false;
    }

    return pTemp;
}

void CRadarArea::Unlink()
{
    // Remove us from the manager's list
    m_pRadarAreaManager->RemoveFromList(this);
}

bool CRadarArea::ReadSpecialData(const int iLine)
{
    // Grab the "posX" data
    if (!GetCustomDataFloat("posX", m_vecPosition.fX, true))
    {
        CLogger::ErrorPrintf("Bad/missing 'posX' attribute in <radararea> (line %d)\n", iLine);
        return false;
    }

    // Grab the "posY" data
    if (!GetCustomDataFloat("posY", m_vecPosition.fY, true))
    {
        CLogger::ErrorPrintf("Bad/missing 'posY' attribute in <radararea> (line %d)\n", iLine);
        return false;
    }

    // Grab the "sizeX" data
    if (!GetCustomDataFloat("sizeX", m_vecSize.fX, true))
    {
        CLogger::ErrorPrintf("Bad/missing 'sizeX' attribute in <radararea> (line %d)\n", iLine);
        return false;
    }

    // Grab the "sizeY" data
    if (!GetCustomDataFloat("sizeY", m_vecSize.fY, true))
    {
        CLogger::ErrorPrintf("Bad/missing 'sizeY' attribute in <radararea> (line %d)\n", iLine);
        return false;
    }

    // Grab the "color" data
    char szColor[64];
    if (GetCustomDataString("color", szColor, 64, true))
    {
        // Convert it to RGBA
        if (!XMLColorToInt(szColor, m_Color.R, m_Color.G, m_Color.B, m_Color.A))
        {
            CLogger::ErrorPrintf("Bad 'color' value specified in <radararea> (line %d)\n", iLine);
            return false;
        }
    }
    else
    {
        m_Color = SColorRGBA(255, 0, 0, 255);
    }

    int iTemp;
    if (GetCustomDataInt("dimension", iTemp, true))
        m_usDimension = static_cast<unsigned short>(iTemp);

    return true;
}

void CRadarArea::SetPosition(const CVector& vecPosition)
{
    // Different from our current position?
    if (vecPosition != m_vecPosition)
    {
        // Set the new position
        m_vecPosition = vecPosition;

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

void CRadarArea::SetSize(const CVector2D& vecSize)
{
    // Different from our current size?
    if (vecSize != m_vecSize)
    {
        // Update the size
        m_vecSize = vecSize;

        // Tell all the players that know about us
        CBitStream BitStream;
        BitStream.pBitStream->Write(vecSize.fX);
        BitStream.pBitStream->Write(vecSize.fY);
        BroadcastOnlyVisible(CElementRPCPacket(this, SET_RADAR_AREA_SIZE, *BitStream.pBitStream));
    }
}

void CRadarArea::SetColor(const SColor color)
{
    // Different from our current color?
    if (color != m_Color)
    {
        // Update the color
        m_Color = color;

        // Tell all the players that know about us
        CBitStream BitStream;
        BitStream.pBitStream->Write(color.R);
        BitStream.pBitStream->Write(color.G);
        BitStream.pBitStream->Write(color.B);
        BitStream.pBitStream->Write(color.A);
        BroadcastOnlyVisible(CElementRPCPacket(this, SET_RADAR_AREA_COLOR, *BitStream.pBitStream));
    }
}

void CRadarArea::SetFlashing(bool bFlashing)
{
    // Different from our current flashing status?
    if (bFlashing != m_bIsFlashing)
    {
        // Set the new status
        m_bIsFlashing = bFlashing;

        // Tell all the players that know about us
        CBitStream BitStream;
        BitStream.pBitStream->WriteBit(bFlashing);
        BroadcastOnlyVisible(CElementRPCPacket(this, SET_RADAR_AREA_FLASHING, *BitStream.pBitStream));
    }
}
