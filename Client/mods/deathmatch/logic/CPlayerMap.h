/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPlayerMap.h
 *  PURPOSE:     Header for player map class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CClientCommon.h"
#include <CClientManager.h>
#include <CClientTextDisplay.h>
#include <gui/CGUI.h>

class CResource;
class CClientTexture;

enum class ECustomMapResolution : std::uint32_t
{
    Res_1024 = 1024,
    Res_2048 = 2048
};

inline std::size_t MapResolutionToIndex(ECustomMapResolution resolution)
{
    return resolution == ECustomMapResolution::Res_1024 ? 0 : 1;
}

inline std::uint32_t MapResolutionToSize(ECustomMapResolution resolution)
{
    return static_cast<std::uint32_t>(resolution);
}

inline std::optional<ECustomMapResolution> UIntToMapResolution(std::uint32_t value)
{
    switch (value)
    {
        case static_cast<std::uint32_t>(ECustomMapResolution::Res_1024):
            return ECustomMapResolution::Res_1024;
        case static_cast<std::uint32_t>(ECustomMapResolution::Res_2048):
            return ECustomMapResolution::Res_2048;
        default:
            return std::nullopt;
    }
}

class CPlayerMap
{
public:
    CPlayerMap(class CClientManager* pManager);
    virtual ~CPlayerMap();

    void DoPulse();
    void DoRender();

    bool IsPlayerMapShowing();

    bool GetPlayerMapEnabled() const { return m_bIsPlayerMapEnabled; };
    void SetPlayerMapEnabled(bool bIsRadarEnabled);

    bool GetForcedState() const { return m_bForcedState; }
    void SetForcedState(bool bState);

    bool GetBoundingBox(CVector& vecMin, CVector& vecMax);

    void ToggleHelpText();
    void MarkViewportRefreshPending();
    void ClearMovementFlags();

    bool  SetCustomMapImage(const std::string& strTexturePath, ECustomMapResolution resolution, CResource* pResource = nullptr);
    bool  SetCustomMapImageFromTexture(CClientTexture* pTexture, ECustomMapResolution resolution, CResource* pResource = nullptr);
    void  ResetCustomMapImage(std::optional<ECustomMapResolution> resolution = std::nullopt);
    bool  SetMapOpacity(uchar ucOpacity, CResource* pResource = nullptr);
    void  ResetMapOpacity();
    uchar GetMapOpacity() const;
    bool  HasCustomMapImage(ECustomMapResolution resolution) const { return m_customMapData[MapResolutionToIndex(resolution)].bHasCustomMap; }
    void  OnResourceStopping(CResource* pResource);

protected:
    void InternalSetPlayerMapEnabled(bool bEnabled);

    void          CreateMarkerTextures();
    CTextureItem* GetMarkerTexture(CClientRadarMarker* pMarker, float fLocalZ, float* pfScale, SColor* pColor);
    void          CreatePlayerBlipTexture();
    void          CreateOrUpdateMapTexture();
    void          UpdateOrRevertMapTexture(std::size_t imageIndex);
    void          CreateAllTextures();
    void          LoadUserCustomMapIfExists();

public:
    bool IsAttachedToLocalPlayer() const { return m_bIsAttachedToLocal; };
    void SetAttachedToLocalPlayer(bool bIsAttachedToLocal);

    bool IsMovingNorth() const { return m_bIsMovingNorth; };
    void SetMovingNorth(bool bIsMovingNorth) { m_bIsMovingNorth = bIsMovingNorth; };

    bool IsMovingSouth() const { return m_bIsMovingSouth; };
    void SetMovingSouth(bool bIsMovingSouth) { m_bIsMovingSouth = bIsMovingSouth; };

    bool IsMovingEast() const { return m_bIsMovingEast; };
    void SetMovingEast(bool bIsMovingEast) { m_bIsMovingEast = bIsMovingEast; };

    bool IsMovingWest() const { return m_bIsMovingWest; };
    void SetMovingWest(bool bIsMovingWest) { m_bIsMovingWest = bIsMovingWest; };

    void ZoomIn();
    void ZoomOut();

    SString GetBoundKeyName(const SString& strCommand);

private:
    bool CalculateEntityOnScreenPosition(class CClientEntity* pEntity, CVector2D& vecLocalPos);
    bool CalculateEntityOnScreenPosition(CVector vecPosition, CVector2D& vecLocalPos);
    void SetupMapVariables();

    void MoveNorth();
    void MoveSouth();
    void MoveEast();
    void MoveWest();

private:
    class CClientManager*            m_pManager;
    class CClientRadarMarkerManager* m_pRadarMarkerManager;
    class CClientRadarAreaManager*   m_pRadarAreaManager;

    bool m_failedToLoadTextures;

    std::size_t m_playerMapImageIndex;

    CTextureItem*              m_mapImageTexture;
    CTextureItem*              m_playerMarkerTexture;
    std::vector<CTextureItem*> m_markerTextureList;

    unsigned int m_uiHeight;
    unsigned int m_uiWidth;

    float m_fMapSize;
    int   m_iMapMinX;
    int   m_iMapMaxX;
    int   m_iMapMinY;
    int   m_iMapMaxY;

    int m_iHorizontalMovement;
    int m_iVerticalMovement;

    float m_fZoom;

    bool m_bIsPlayerMapEnabled;
    bool m_bForcedState;
    bool m_bIsAttachedToLocal;

    bool m_bIsMovingNorth;
    bool m_bIsMovingSouth;
    bool m_bIsMovingEast;
    bool m_bIsMovingWest;

    unsigned long m_ulUpdateTime;

    std::vector<std::shared_ptr<CClientTextDisplay>> m_HelpTextList;
    bool                                             m_bHideHelpText;

    bool m_bHudVisible;
    bool m_bChatVisible;
    bool m_bChatInputBlocked;
    bool m_bRadarVisible;
    bool m_bDebugVisible;
    bool m_bTextVisible;
    bool m_bPendingViewportRefresh = false;
    struct CustomMapData
    {
        bool            bHasCustomMap = false;
        CTextureItem*   pTexture = nullptr;
        std::string     strPath = "";
        CClientTexture* pTextureElement = nullptr;
        CResource*      pResource = nullptr;
    };
    CustomMapData m_customMapData[2];  // [0] = 1024, [1] = 2048
    std::size_t   m_defaultMapImageIndex;
    uchar         m_ucCustomMapOpacity;
    bool          m_bHasCustomMapOpacity;
    CResource*    m_pCustomOpacityResource;
    CResource*    m_pRadarMapDisabledResource;
};
