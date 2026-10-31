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
#include <gui/CGUI.h>

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

protected:
    void InternalSetPlayerMapEnabled(bool bEnabled);

    void          CreateMarkerTextures();
    CTextureItem* GetMarkerTexture(CClientRadarMarker* pMarker, float fLocalZ, float* pfScale, SColor* pColor);
    void          CreatePlayerBlipTexture();
    void          CreateOrUpdateMapTexture();
    void          UpdateOrRevertMapTexture(std::size_t imageIndex);
    void          CreateAllTextures();

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

    void MoveNorth();
    void MoveSouth();
    void MoveEast();
    void MoveWest();

    SString GetBoundKeyName(const SString& strCommand);

private:
    bool CalculateEntityOnScreenPosition(class CClientEntity* pEntity, CVector2D& vecLocalPos);
    bool CalculateEntityOnScreenPosition(CVector vecPosition, CVector2D& vecLocalPos);
    void SetupMapVariables();

private:
    class CClientManager*            m_pManager;
    class CClientRadarMarkerManager* m_pRadarMarkerManager;
    class CClientRadarAreaManager*   m_pRadarAreaManager;

    bool m_failedToLoadTextures;

    std::size_t m_playerMapImageIndex;

    CTextureItem*                  m_mapImageTexture;
    CTextureItem*                  m_playerMarkerTexture;
    std::vector<CTextureItem*>     m_markerTextureList;
    std::array<CTextureItem*, 144> m_radarTileTextures{};
    bool                           m_radarTilesLoaded = false;

    void LoadRadarTileTextures();
    void ReleaseRadarTileTextures();

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

    bool m_bHideHelpText = false;

    bool m_bHudVisible;
    bool m_bChatVisible;
    bool m_bChatInputBlocked;
    bool m_bDebugVisible;
    bool m_bPendingViewportRefresh = false;

    bool      m_isDragging = false;
    bool      m_cursorEnabled = false;
    float     m_savedZoomLevel = 2.0f;
    CVector2D m_dragStartCursor;
    int       m_dragStartHorizontal = 0;
    int       m_dragStartVertical = 0;

    class CClientRadarMarker* m_waypointMarker = nullptr;

public:
    bool ProcessMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void ZoomAtCursor(float factor, float cursorX, float cursorY);
    void ToggleWaypoint(float worldX, float worldY);
    void ClearWaypoint();
};
