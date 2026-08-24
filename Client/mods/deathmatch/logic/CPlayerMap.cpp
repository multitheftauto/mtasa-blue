/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPlayerMap.cpp
 *  PURPOSE:     Full screen player map renderer
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>
#include <game/CRadar.h>
#include <CClientPlayer.h>
#include <CClientPlayerManager.h>
#include <CClientRadarMarker.h>

using SharedUtil::CalcMTASAPath;
using std::list;

enum
{
    MARKER_SQUARE_INDEX = 0,
    MARKER_UP_TRIANGLE_INDEX = 1,
    MARKER_DOWN_TRIANGLE_INDEX = 2,
    MARKER_FIRST_SPRITE_INDEX = 3,
    MARKER_LAST_SPRITE_INDEX = MARKER_FIRST_SPRITE_INDEX + RADAR_MARKER_LIMIT - 1,
};

constexpr std::array<std::uint32_t, 2> MAP_IMAGE_SIZES = {1024, 2048};

CPlayerMap::CPlayerMap(CClientManager* pManager)
{
    m_failedToLoadTextures = false;

    // Setup our managers
    m_pManager = pManager;
    m_pRadarMarkerManager = pManager->GetRadarMarkerManager();
    m_pRadarAreaManager = m_pManager->GetRadarAreaManager();

    // Set the map bools
    m_bIsPlayerMapEnabled = false;
    m_bForcedState = false;
    m_bIsAttachedToLocal = false;
    m_bHideHelpText = false;

    // Set the movement bools
    m_bIsMovingNorth = false;
    m_bIsMovingSouth = false;
    m_bIsMovingEast = false;
    m_bIsMovingWest = false;

    // Set the update time to the current time
    m_ulUpdateTime = GetTickCount32();

    // Get the window sizes and set the map variables to default zoom/movement
    m_uiHeight = g_pCore->GetGraphics()->GetViewportHeight();
    m_uiWidth = g_pCore->GetGraphics()->GetViewportWidth();
    m_savedZoomLevel = 1.0f;
    g_pCore->GetCVars()->Get("map_zoom", m_savedZoomLevel);
    if (m_savedZoomLevel < 0.45f || m_savedZoomLevel > 16.0f)
    {
        m_savedZoomLevel = 1.0f;
    }
    m_fZoom = m_savedZoomLevel;
    m_iHorizontalMovement = 0;
    m_iVerticalMovement = 0;

    // Init texture vars
    m_mapImageTexture = nullptr;
    m_playerMarkerTexture = nullptr;

    // Create all map textures
    CreateAllTextures();

    // Default to attached to player
    SetAttachedToLocalPlayer(true);

    SetupMapVariables();
}

CPlayerMap::~CPlayerMap()
{
    // Delete our images
    ClearWaypoint();
    ReleaseRadarTileTextures();
    SAFE_RELEASE(m_mapImageTexture);
    SAFE_RELEASE(m_playerMarkerTexture);
    for (uint i = 0; i < m_markerTextureList.size(); i++)
        SAFE_RELEASE(m_markerTextureList[i]);
    m_markerTextureList.clear();
}

void CPlayerMap::LoadRadarTileTextures()
{
    if (m_radarTilesLoaded)
        return;

    int validCount = 0;
    for (int index = 0; index < 144; ++index)
    {
        m_radarTileTextures[index] = nullptr;

        SString candidatePaths[] = {
            CalcMTASAPath(SString("MTA\\cgui\\images\\radar_tiles\\radar%02d.png", index)),
            CalcMTASAPath(SString("cgui\\images\\radar_tiles\\radar%02d.png", index)),
        };

        for (const auto& path : candidatePaths)
        {
            if (FileExists(path))
            {
                // Disable mipmaps to eliminate tile boundary seams during scaling
                m_radarTileTextures[index] =
                    g_pCore->GetGraphics()->GetRenderItemManager()->CreateTexture(path, nullptr, false, RDEFAULT, RDEFAULT, RFORMAT_UNKNOWN, TADDRESS_CLAMP);
                if (m_radarTileTextures[index])
                {
                    validCount++;
                    break;
                }
            }
        }
    }

    // Only mark as loaded if at least one valid tile texture was created
    m_radarTilesLoaded = (validCount > 0);
}

void CPlayerMap::ReleaseRadarTileTextures()
{
    for (int index = 0; index < 144; ++index)
    {
        SAFE_RELEASE(m_radarTileTextures[index]);
    }
    m_radarTilesLoaded = false;
}

void CPlayerMap::CreateOrUpdateMapTexture()
{
    if (m_playerMapImageIndex >= MAP_IMAGE_SIZES.size())
        m_playerMapImageIndex = 0;

    const std::uint32_t mapSize = MAP_IMAGE_SIZES[m_playerMapImageIndex];
    const SString       fileName("MTA\\cgui\\images\\map_%d.png", mapSize);

    auto* newTexture = g_pCore->GetGraphics()->GetRenderItemManager()->CreateTexture(CalcMTASAPath(fileName));
    if (!newTexture)
    {
        // Try fallback 1024 resolution
        newTexture = g_pCore->GetGraphics()->GetRenderItemManager()->CreateTexture(CalcMTASAPath("MTA\\cgui\\images\\map_1024.png"));
    }

    if (!newTexture)
        throw std::runtime_error("Failed to load map image");

    SAFE_RELEASE(m_mapImageTexture);
    m_mapImageTexture = newTexture;
}

void CPlayerMap::UpdateOrRevertMapTexture(std::size_t newImageIndex)
{
    const std::size_t oldImageIndex = m_playerMapImageIndex;
    try
    {
        m_playerMapImageIndex = newImageIndex;
        CreateOrUpdateMapTexture();
    }
    catch (const std::exception& e)
    {
        m_playerMapImageIndex = oldImageIndex;
        g_pCore->GetConsole()->Printf("Problem updating map image: %s", e.what());
    }
}

void CPlayerMap::CreatePlayerBlipTexture()
{
    m_playerMarkerTexture = g_pCore->GetGraphics()->GetRenderItemManager()->CreateTexture(CalcMTASAPath("MTA\\cgui\\images\\radarset\\03.png"));
    if (!m_playerMarkerTexture)
        throw std::runtime_error("Failed to load player blip image");
}

void CPlayerMap::CreateAllTextures()
{
    try
    {
        m_playerMapImageIndex = g_pCore->GetCVars()->GetValue<std::size_t>("mapimage");
        CreateOrUpdateMapTexture();
        CreatePlayerBlipTexture();
        CreateMarkerTextures();

        try
        {
            LoadRadarTileTextures();
        }
        catch (...)
        {
            m_radarTilesLoaded = false;
        }

        m_failedToLoadTextures = false;
    }
    catch (const std::exception& e)
    {
        m_failedToLoadTextures = true;
        g_pCore->GetConsole()->Printf("Problem initializing player map: %s", e.what());
    }
}

void CPlayerMap::DoPulse()
{
    const uint uiViewportWidth = g_pCore->GetGraphics()->GetViewportWidth();
    const uint uiViewportHeight = g_pCore->GetGraphics()->GetViewportHeight();
    if (uiViewportWidth > 0 && uiViewportHeight > 0 && (m_bPendingViewportRefresh || m_uiWidth != uiViewportWidth || m_uiHeight != uiViewportHeight))
    {
        m_uiWidth = uiViewportWidth;
        m_uiHeight = uiViewportHeight;
        SetupMapVariables();
        m_bPendingViewportRefresh = false;
    }

    if (m_waypointMarker)
    {
        CClientPlayer* localPlayer = m_pManager->GetPlayerManager()->GetLocalPlayer();
        if (localPlayer)
        {
            CVector playerPos;
            localPlayer->GetPosition(playerPos);
            CVector markerPos;
            m_waypointMarker->GetPosition(markerPos);
            float dist = std::hypot(playerPos.fX - markerPos.fX, playerPos.fY - markerPos.fY);
            if (dist <= 15.0f)
            {
                ClearWaypoint();
            }
        }
    }

    // If our map image exists
    if (IsPlayerMapShowing())
    {
        // If we are following the local player blip
        if (m_bIsAttachedToLocal)
        {
            // Get the latest vars for the map
            SetupMapVariables();
        }
    }
}

void CPlayerMap::MarkViewportRefreshPending()
{
    m_bPendingViewportRefresh = true;
}

void CPlayerMap::ClearMovementFlags()
{
    m_bIsMovingNorth = false;
    m_bIsMovingSouth = false;
    m_bIsMovingEast = false;
    m_bIsMovingWest = false;
}

//
// Precreate all the textures for the player map markers
//
void CPlayerMap::CreateMarkerTextures()
{
    m_markerTextureList.clear();
    SString strRadarSetDirectory = CalcMTASAPath("MTA\\cgui\\images\\radarset\\");

    // Load the 3 shapes
    const char* shapeFileNames[] = {"square.png", "up.png", "down.png"};
    for (uint i = 0; i < NUMELMS(shapeFileNames); i++)
    {
        CTextureItem* pTextureItem = g_pCore->GetGraphics()->GetRenderItemManager()->CreateTexture(PathJoin(strRadarSetDirectory, shapeFileNames[i]));
        m_markerTextureList.push_back(pTextureItem);
    }

    if (m_markerTextureList.size() != MARKER_FIRST_SPRITE_INDEX)
        throw std::runtime_error("Failed to load marker textures [1]");

    // Load the icons
    for (uint i = 0; i < RADAR_MARKER_LIMIT; i++)
    {
        CTextureItem* pTextureItem = g_pCore->GetGraphics()->GetRenderItemManager()->CreateTexture(PathJoin(strRadarSetDirectory, SString("%02u.png", i + 1)));
        m_markerTextureList.push_back(pTextureItem);
    }

    if (m_markerTextureList.size() != MARKER_LAST_SPRITE_INDEX + 1)
        throw std::runtime_error("Failed to load marker textures [2]");
}

//
// Get a texture for a marker, including scale and color
//
CTextureItem* CPlayerMap::GetMarkerTexture(CClientRadarMarker* pMarker, float fLocalZ, float* pfScale, SColor* pColor)
{
    float  fScale = pMarker->GetScale();
    ulong  ulSprite = pMarker->GetSprite();
    SColor color = pMarker->GetColor();

    // Make list index
    uint uiListIndex = 0;

    if (ulSprite)
    {
        // ulSprite >= 1 and <= 63
        // Remap to texture list index
        uiListIndex = ulSprite - 1 + MARKER_FIRST_SPRITE_INDEX;
        color = SColorARGB(255, 255, 255, 255);
        fScale = 1.0f;
    }
    else
    {
        // ulSprite == 0 so draw a square or triangle depending on relative z position
        CVector vecMarker;
        pMarker->GetPosition(vecMarker);

        if (fLocalZ > vecMarker.fZ + 4.0f)
            uiListIndex = MARKER_DOWN_TRIANGLE_INDEX;  // We're higher than this marker, so draw the arrow pointing down
        else if (fLocalZ < vecMarker.fZ - 4.0f)
            uiListIndex = MARKER_UP_TRIANGLE_INDEX;  // We're lower than this entity, so draw the arrow pointing up
        else
            uiListIndex = MARKER_SQUARE_INDEX;  // We're at the same level so draw a square

        fScale = 0.85f;
    }

    *pfScale = fScale;
    *pColor = color;

    if (uiListIndex >= m_markerTextureList.size())
        return NULL;

    return m_markerTextureList[uiListIndex];
}

void CPlayerMap::DoRender()
{
    bool isMapShowing = IsPlayerMapShowing();
    if (isMapShowing)
    {
        g_pCore->GetGraphics()->RefreshViewportIfNeeded();
        if (!g_pCore->GetGraphics()->GetRenderItemManager()->IsUsingDefaultRenderTarget())
        {
            g_pCore->GetGraphics()->GetRenderItemManager()->RestoreDefaultRenderTarget();
        }
        g_pCore->GetGraphics()->ApplyMTARenderViewportIfNeeded();
        const uint uiViewportWidth = g_pCore->GetGraphics()->GetViewportWidth();
        const uint uiViewportHeight = g_pCore->GetGraphics()->GetViewportHeight();
        if (uiViewportWidth > 0 && uiViewportHeight > 0 && (m_bPendingViewportRefresh || m_uiWidth != uiViewportWidth || m_uiHeight != uiViewportHeight))
        {
            m_uiWidth = uiViewportWidth;
            m_uiHeight = uiViewportHeight;
            SetupMapVariables();
            m_bPendingViewportRefresh = false;
        }
    }

    // Render if showing and textures are all loaded
    if (isMapShowing && !m_failedToLoadTextures)
    {
        // Get the alpha value from the settings
        int mapAlpha;
        g_pCore->GetCVars()->Get("mapalpha", mapAlpha);
        const SColorARGB mapColor(mapAlpha, 255, 255, 255);

        // Draw outer ocean borders only in the viewport areas NOT covered by map tiles to eliminate double-alpha overdraw
        const SColorARGB oceanColor(mapAlpha, 104, 136, 168);
        float            screenW = static_cast<float>(m_uiWidth);
        float            screenH = static_cast<float>(m_uiHeight);
        float            minX = static_cast<float>(m_iMapMinX);
        float            maxX = static_cast<float>(m_iMapMaxX);
        float            minY = static_cast<float>(m_iMapMinY);
        float            maxY = static_cast<float>(m_iMapMaxY);

        if (minX > 0.0f)
        {
            g_pCore->GetGraphics()->DrawRectQueued(0.0f, 0.0f, minX, screenH, oceanColor.ulARGB, false);
        }
        if (maxX < screenW)
        {
            g_pCore->GetGraphics()->DrawRectQueued(maxX, 0.0f, screenW - maxX, screenH, oceanColor.ulARGB, false);
        }

        float clampMinX = std::clamp(minX, 0.0f, screenW);
        float clampMaxX = std::clamp(maxX, 0.0f, screenW);
        if (clampMaxX > clampMinX)
        {
            if (minY > 0.0f)
            {
                g_pCore->GetGraphics()->DrawRectQueued(clampMinX, 0.0f, clampMaxX - clampMinX, minY, oceanColor.ulARGB, false);
            }
            if (maxY < screenH)
            {
                g_pCore->GetGraphics()->DrawRectQueued(clampMinX, maxY, clampMaxX - clampMinX, screenH - maxY, oceanColor.ulARGB, false);
            }
        }

        // Draw the 144 HD radar tiles using MTA's modern vertex queue renderer (matching dxDrawImage), or fallback to single map texture

        if (m_radarTilesLoaded)
        {
            float screenW = static_cast<float>(m_uiWidth);
            float screenH = static_cast<float>(m_uiHeight);
            float tileSize = m_fMapSize / 12.0f;

            for (int row = 0; row < 12; ++row)
            {
                float tileY = static_cast<float>(m_iMapMinY) + (row * tileSize);
                if (tileY + tileSize < 0.0f || tileY > screenH)
                    continue;

                for (int col = 0; col < 12; ++col)
                {
                    float tileX = static_cast<float>(m_iMapMinX) + (col * tileSize);
                    if (tileX + tileSize < 0.0f || tileX > screenW)
                        continue;

                    int index = row * 12 + col;
                    if (index >= 0 && index < 144 && m_radarTileTextures[index])
                    {
                        g_pCore->GetGraphics()->DrawTextureQueued(tileX, tileY, tileSize, tileSize, 0.0f, 0.0f, 1.0f, 1.0f, true, m_radarTileTextures[index],
                                                                  0.0f, 0.0f, 0.0f, mapColor.ulARGB, false);
                    }
                }
            }
        }
        else if (m_mapImageTexture)
        {
            g_pCore->GetGraphics()->DrawTextureQueued(static_cast<float>(m_iMapMinX), static_cast<float>(m_iMapMinY), m_fMapSize, m_fMapSize, 0.0f, 0.0f, 1.0f,
                                                      1.0f, true, m_mapImageTexture, 0.0f, 0.0f, 0.0f, mapColor.ulARGB, false);
        }

        // Grab the info for the local player blip
        CVector2D vecLocalPos;
        CVector   vecLocal;
        CVector   vecLocalRot;
        if (m_pManager->GetCamera()->IsInFixedMode())
        {
            m_pManager->GetCamera()->GetPosition(vecLocal);
            m_pManager->GetCamera()->GetRotationDegrees(vecLocalRot);
        }
        else
        {
            CClientPlayer* pLocalPlayer = m_pManager->GetPlayerManager()->GetLocalPlayer();
            if (!pLocalPlayer)
                return;
            pLocalPlayer->GetPosition(vecLocal);
            pLocalPlayer->GetRotationDegrees(vecLocalRot);
        }

        CalculateEntityOnScreenPosition(vecLocal, vecLocalPos);

        // Now loop our radar areas
        unsigned short                          usDimension = m_pRadarAreaManager->GetDimension();
        CClientRadarArea*                       pArea = NULL;
        list<CClientRadarArea*>::const_iterator areaIter = m_pRadarAreaManager->IterBegin();
        for (; areaIter != m_pRadarAreaManager->IterEnd(); ++areaIter)
        {
            pArea = *areaIter;

            if (pArea->GetDimension() == usDimension)
            {
                // Grab the area image and calculate the position to put it on the screen
                CVector2D vecPos;
                CalculateEntityOnScreenPosition(pArea, vecPos);

                // Get the area size and work out the ratio
                CVector2D vecSize;
                float     fX = (*areaIter)->GetSize().fX;
                float     fY = (*areaIter)->GetSize().fY;
                float     fRatio = 6000.0f / m_fMapSize;

                // Calculate the size of the area
                vecSize.fX = static_cast<float>(fX / fRatio);
                vecSize.fY = static_cast<float>(fY / fRatio);

                SColor color = pArea->GetColor();
                if (pArea->IsFlashing())
                {
                    color.A = static_cast<unsigned char>(color.A * pArea->GetAlphaFactor());
                }

                g_pCore->GetGraphics()->DrawRectQueued(vecPos.fX, vecPos.fY, vecSize.fX, -vecSize.fY, color.ulARGB, false);
            }
        }

        // Now loop our radar markers
        usDimension = m_pRadarMarkerManager->GetDimension();
        list<CClientRadarMarker*>::const_iterator markerIter = m_pRadarMarkerManager->IterBegin();
        for (; markerIter != m_pRadarMarkerManager->IterEnd(); ++markerIter)
        {
            if ((*markerIter)->IsVisible() && (*markerIter)->GetDimension() == usDimension)
            {
                // Grab the marker image and calculate the position to put it on the screen
                float         fScale = 1;
                SColor        color;
                CTextureItem* pTexture = GetMarkerTexture(*markerIter, vecLocal.fZ, &fScale, &color);

                if (pTexture)
                {
                    CVector2D vecPos;
                    CalculateEntityOnScreenPosition(*markerIter, vecPos);
                    float blipPixelSize = 22.0f * fScale;
                    g_pCore->GetGraphics()->DrawTextureQueued(vecPos.fX - blipPixelSize * 0.5f, vecPos.fY - blipPixelSize * 0.5f, blipPixelSize, blipPixelSize,
                                                              0.0f, 0.0f, 1.0f, 1.0f, true, pTexture, 0.0f, 0.0f, 0.0f, color.ulARGB, false);
                }
            }
        }

        float playerSize = 26.0f;
        g_pCore->GetGraphics()->DrawTextureQueued(vecLocalPos.fX - playerSize * 0.5f, vecLocalPos.fY - playerSize * 0.5f, playerSize, playerSize, 0.0f, 0.0f,
                                                  1.0f, 1.0f, true, m_playerMarkerTexture, vecLocalRot.fZ, 0.0f, 0.0f, 0xFFFFFFFF, false);

        if (!m_bHideHelpText)
        {
            float screenWidth = static_cast<float>(m_uiWidth);
            float screenHeight = static_cast<float>(m_uiHeight);

            ID3DXFont* pBankGothic = g_pCore->GetGraphics()->GetFont(FONT_BANKGOTHIC);

            struct HelpEntry
            {
                std::string action;
                std::string keys;
            };

            auto ToUpperString = [](std::string str) -> std::string
            {
                std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });
                return str;
            };

            std::string helpKey = ToUpperString(GetBoundKeyName("radar_help"));
            if (helpKey.empty() || helpKey == "RADAR_HELP")
                helpKey = "NUM_1";

            std::string zoomInKey = ToUpperString(GetBoundKeyName("radar_zoom_in"));
            std::string zoomOutKey = ToUpperString(GetBoundKeyName("radar_zoom_out"));
            std::string zoomKeys = "MOUSE_WHEEL_DOWN / MOUSE_WHEEL_UP";
            if (!zoomInKey.empty() && zoomInKey != "RADAR_ZOOM_IN" && !zoomOutKey.empty() && zoomOutKey != "RADAR_ZOOM_OUT")
            {
                zoomKeys += " / " + zoomInKey + " / " + zoomOutKey;
            }
            else
            {
                zoomKeys += " / NUM_ADD / NUM_SUB";
            }

            std::string opDownKey = ToUpperString(GetBoundKeyName("radar_opacity_down"));
            std::string opUpKey = ToUpperString(GetBoundKeyName("radar_opacity_up"));
            std::string opacityKeys = "NUM_DIV / NUM_MUL";
            if (!opDownKey.empty() && opDownKey != "RADAR_OPACITY_DOWN" && !opUpKey.empty() && opUpKey != "RADAR_OPACITY_UP")
            {
                opacityKeys = opDownKey + " / " + opUpKey;
            }

            std::vector<HelpEntry> helpRows = {
                {"HELP", helpKey},    {"ZOOM", zoomKeys},     {"MOVE", "MOUSE1 / NUM_2 / NUM_4 / NUM_6 / NUM_8"}, {"ATTACH", "SPACE"}, {"OPACITY", opacityKeys},
                {"CURSOR", "MOUSE3"}, {"WAYPOINT", "MOUSE2"},
            };

            float fontScale = 0.52f;
            float rowHeight = 22.0f;
            float padX = 22.0f;
            float padY = 14.0f;
            float actionColWidth = 140.0f;
            float keysColWidth = 630.0f;
            float panelWidth = actionColWidth + keysColWidth + padX * 2.0f;
            float panelHeight = static_cast<float>(helpRows.size()) * rowHeight + padY * 2.0f;

            // Perfectly centered horizontally at the bottom of the screen
            float panelX = std::floor((screenWidth - panelWidth) / 2.0f);
            float panelY = screenHeight - panelHeight - 16.0f;

            g_pCore->GetGraphics()->DrawRectQueued(panelX, panelY, panelWidth, panelHeight, 0xC0050505, false);

            for (size_t i = 0; i < helpRows.size(); ++i)
            {
                float rowY = panelY + padY + static_cast<float>(i) * rowHeight;

                // Action Column (e.g. HELP, ZOOM, MOVE)
                float actionX = panelX + padX;
                // Shadow
                g_pCore->GetGraphics()->DrawStringQueued(actionX + 1.0f, rowY + 1.0f, actionX + actionColWidth + 1.0f, rowY + rowHeight + 1.0f, 0xDD000000,
                                                         helpRows[i].action.c_str(), fontScale, fontScale, DT_LEFT | DT_NOCLIP, pBankGothic, false);
                // Text
                g_pCore->GetGraphics()->DrawStringQueued(actionX, rowY, actionX + actionColWidth, rowY + rowHeight, 0xFFF1F1F1, helpRows[i].action.c_str(),
                                                         fontScale, fontScale, DT_LEFT | DT_NOCLIP, pBankGothic, false);

                // Keys Column (e.g. NUM_1, MOUSE_WHEEL_DOWN / ...)
                float keysX = actionX + actionColWidth + 15.0f;
                // Shadow
                g_pCore->GetGraphics()->DrawStringQueued(keysX + 1.0f, rowY + 1.0f, panelX + panelWidth - padX + 1.0f, rowY + rowHeight + 1.0f, 0xDD000000,
                                                         helpRows[i].keys.c_str(), fontScale, fontScale, DT_LEFT | DT_NOCLIP, pBankGothic, false);
                // Text
                g_pCore->GetGraphics()->DrawStringQueued(keysX, rowY, panelX + panelWidth - padX, rowY + rowHeight, 0xFFF1F1F1, helpRows[i].keys.c_str(),
                                                         fontScale, fontScale, DT_LEFT | DT_NOCLIP, pBankGothic, false);
            }
        }

        // If cursor is active on the Big Map, check for entity hover and render a clean tooltip badge
        if (m_cursorEnabled)
        {
            POINT mousePt;
            GetCursorPos(&mousePt);
            HWND gameHwnd = g_pCore->GetHookedWindow();
            ScreenToClient(gameHwnd, &mousePt);
            float cursorX = static_cast<float>(mousePt.x);
            float cursorY = static_cast<float>(mousePt.y);

            SString hoveredLabel = "";
            SColor  labelColor = SColorARGB(255, 255, 255, 255);

            // 1. Check Local Player
            float distToLocal = std::hypot(cursorX - vecLocalPos.fX, cursorY - vecLocalPos.fY);
            if (distToLocal <= 18.0f)
            {
                CClientPlayer* localPlayer = m_pManager->GetPlayerManager()->GetLocalPlayer();
                if (localPlayer)
                {
                    hoveredLabel = SString("%s (You)", localPlayer->GetNick());
                    labelColor = SColorARGB(255, 80, 220, 255);
                }
            }

            // 2. Check Remote Players
            if (hoveredLabel.empty())
            {
                for (auto iter = m_pManager->GetPlayerManager()->IterBegin(); iter != m_pManager->GetPlayerManager()->IterEnd(); ++iter)
                {
                    CClientPlayer* player = *iter;
                    if (player && player != m_pManager->GetPlayerManager()->GetLocalPlayer() && player->GetDimension() == usDimension)
                    {
                        CVector playerWorldPos;
                        player->GetPosition(playerWorldPos);
                        CVector2D playerScreenPos;
                        if (CalculateEntityOnScreenPosition(playerWorldPos, playerScreenPos))
                        {
                            float dist = std::hypot(cursorX - playerScreenPos.fX, cursorY - playerScreenPos.fY);
                            if (dist <= 18.0f)
                            {
                                hoveredLabel = player->GetNick();
                                labelColor = SColorARGB(255, 255, 230, 100);
                                break;
                            }
                        }
                    }
                }
            }

            // 3. Check Radar Markers / Blips
            if (hoveredLabel.empty())
            {
                for (auto markerIter = m_pRadarMarkerManager->IterBegin(); markerIter != m_pRadarMarkerManager->IterEnd(); ++markerIter)
                {
                    CClientRadarMarker* marker = *markerIter;
                    if (marker && marker->IsVisible() && marker->GetDimension() == usDimension)
                    {
                        CVector2D markerScreenPos;
                        CalculateEntityOnScreenPosition(marker, markerScreenPos);
                        float dist = std::hypot(cursorX - markerScreenPos.fX, cursorY - markerScreenPos.fY);
                        if (dist <= 16.0f)
                        {
                            ulong sprite = marker->GetSprite();
                            if (sprite == 41)
                            {
                                hoveredLabel = "Target Waypoint";
                                labelColor = SColorARGB(255, 255, 90, 90);
                            }
                            else if (sprite > 0)
                            {
                                static const char* spriteNames[] = {"Marker",
                                                                    "Center",
                                                                    "Map Arrow",
                                                                    "White Square",
                                                                    "Player Indicator",
                                                                    "Air Yard",
                                                                    "Ammu-Nation",
                                                                    "Barber",
                                                                    "Big Smoke",
                                                                    "Boat / Marina",
                                                                    "Burger Shot",
                                                                    "Quarry",
                                                                    "Catalina",
                                                                    "Cesar",
                                                                    "Cluckin' Bell",
                                                                    "Carl Johnson",
                                                                    "Crash",
                                                                    "Diner",
                                                                    "Emmet",
                                                                    "Enemy",
                                                                    "Fire Station",
                                                                    "Girlfriend",
                                                                    "Hospital",
                                                                    "City Hall",
                                                                    "The Johnson House",
                                                                    "Madd Dogg",
                                                                    "Caligula's Casino",
                                                                    "Mafia",
                                                                    "MC Loc",
                                                                    "Mod Garage",
                                                                    "OG Loc",
                                                                    "Well Stacked Pizza",
                                                                    "Police Department",
                                                                    "Property For Sale",
                                                                    "Property Not For Sale",
                                                                    "Race",
                                                                    "Ryder",
                                                                    "Safehouse",
                                                                    "School",
                                                                    "Mystery",
                                                                    "Waypoint",
                                                                    "Sweet",
                                                                    "Tattoo",
                                                                    "The Truth",
                                                                    "Transfender",
                                                                    "Triads",
                                                                    "Four Dragons Casino",
                                                                    "Bar / Drinks",
                                                                    "Clothes Store",
                                                                    "Woozie",
                                                                    "Zero",
                                                                    "Date / Club",
                                                                    "Drinks",
                                                                    "Barber / Hairdresser",
                                                                    "Pay 'n' Spray",
                                                                    "Garage",
                                                                    "Strip Club",
                                                                    "Gym",
                                                                    "Weapon",
                                                                    "Clothes",
                                                                    "Lowrider Tuning",
                                                                    "Wheels / Tuning",
                                                                    "Diner / Restaurant"};
                                if (sprite < sizeof(spriteNames) / sizeof(spriteNames[0]))
                                {
                                    hoveredLabel = spriteNames[sprite];
                                    labelColor = SColorARGB(255, 255, 255, 255);
                                }
                            }
                            if (!hoveredLabel.empty())
                                break;
                        }
                    }
                }
            }

            // 4. Render sleek hover tooltip badge
            if (!hoveredLabel.empty())
            {
                float textScale = 1.0f;
                float approxWidth = static_cast<float>(hoveredLabel.length()) * 8.5f + 16.0f;
                float badgeHeight = 22.0f;
                float badgeX = cursorX + 14.0f;
                float badgeY = cursorY - 26.0f;

                if (badgeX + approxWidth > static_cast<float>(m_uiWidth))
                    badgeX = cursorX - approxWidth - 8.0f;
                if (badgeY < 8.0f)
                    badgeY = cursorY + 22.0f;

                // Dark background card with high contrast border
                g_pCore->GetGraphics()->DrawRectQueued(badgeX - 1.0f, badgeY - 1.0f, approxWidth + 2.0f, badgeHeight + 2.0f, 0xFF0A0A0A, false);
                g_pCore->GetGraphics()->DrawRectQueued(badgeX, badgeY, approxWidth, badgeHeight, 0xE61E1E1E, false);

                // Tooltip text with shadow
                g_pCore->GetGraphics()->DrawStringQueued(badgeX + 1.0f, badgeY + 4.0f + 1.0f, badgeX + approxWidth + 1.0f, badgeY + badgeHeight, 0xCC000000,
                                                         hoveredLabel.c_str(), textScale, textScale, DT_CENTER | DT_NOCLIP, nullptr, false);
                g_pCore->GetGraphics()->DrawStringQueued(badgeX, badgeY + 4.0f, badgeX + approxWidth, badgeY + badgeHeight, labelColor.ulARGB,
                                                         hoveredLabel.c_str(), textScale, textScale, DT_CENTER | DT_NOCLIP, nullptr, false);
            }
        }
    }
}

void CPlayerMap::SetPlayerMapEnabled(bool show)
{
    bool alreadyEnabled = (m_bIsPlayerMapEnabled || m_bForcedState);
    bool definitiveShow = (show || m_bForcedState);
    if (alreadyEnabled != definitiveShow)
    {
        InternalSetPlayerMapEnabled(definitiveShow);
    }
    m_bIsPlayerMapEnabled = show;
}

void CPlayerMap::SetForcedState(bool state)
{
    bool currState = (m_bIsPlayerMapEnabled || m_bForcedState);
    bool definitiveState = (m_bIsPlayerMapEnabled || state);
    if (currState != definitiveState)
    {
        InternalSetPlayerMapEnabled(definitiveState);
    }
    m_bForcedState = state;
}

void CPlayerMap::InternalSetPlayerMapEnabled(bool enable)
{
    if (enable)
    {
        m_bChatVisible = g_pCore->IsChatVisible();
        m_bChatInputBlocked = g_pCore->IsChatInputBlocked();
        m_bDebugVisible = g_pCore->IsDebugVisible();

        g_pGame->GetHud()->Disable(true);
        g_pMultiplayer->HideRadar(true);
        g_pCore->SetChatVisible(false);
        g_pCore->SetDebugVisible(false);

        // Disable GTA game controls and clear pad state so mouse wheel & clicks do not affect vehicle radio/weapons in the background
        g_pGame->GetPad()->Disable(true);
        g_pGame->GetPad()->Clear();

        // Keep mouse cursor hidden by default (toggleable via Mouse 3)
        g_pCore->ForceCursorVisible(false, false);
        m_cursorEnabled = false;
        m_isDragging = false;

        // Restore player's persistent zoom level
        m_fZoom = m_savedZoomLevel;
        m_bIsAttachedToLocal = true;
        SetupMapVariables();
    }
    else
    {
        g_pGame->GetHud()->Disable(false);
        g_pMultiplayer->HideRadar(false);
        g_pCore->SetChatVisible(m_bChatVisible, m_bChatInputBlocked);
        g_pCore->SetDebugVisible(m_bDebugVisible);

        // Re-enable GTA game controls
        g_pGame->GetPad()->Disable(false);

        g_pCore->ForceCursorVisible(false, false);
        m_isDragging = false;
        m_cursorEnabled = false;
    }
}

bool CPlayerMap::CalculateEntityOnScreenPosition(CClientEntity* pEntity, CVector2D& vecLocalPos)
{
    // If the entity exists
    if (pEntity)
    {
        // Get the Entities ingame position
        CVector vecPosition;
        pEntity->GetPosition(vecPosition);

        // Adjust to the map variables and create the map ratio
        float fX = vecPosition.fX + 3000.0f;
        float fY = vecPosition.fY + 3000.0f;
        float fRatio = 6000.0f / m_fMapSize;

        // Calculate the screen position for the marker
        vecLocalPos.fX = static_cast<float>(m_iMapMinX) + (fX / fRatio);
        vecLocalPos.fY = static_cast<float>(m_iMapMaxY) - (fY / fRatio);

        // If the position is on the screen
        if (vecLocalPos.fX >= 0.0f && vecLocalPos.fX <= static_cast<float>(m_uiWidth) && vecLocalPos.fY >= 0.0f &&
            vecLocalPos.fY <= static_cast<float>(m_uiHeight))
        {
            // Then return true as it is on the screen
            return true;
        }
    }

    // Return false as it is not on the screen
    return false;
}

bool CPlayerMap::CalculateEntityOnScreenPosition(CVector vecPosition, CVector2D& vecLocalPos)
{
    // Adjust to the map variables and create the map ratio
    float fX = vecPosition.fX + 3000.0f;
    float fY = vecPosition.fY + 3000.0f;
    float fRatio = 6000.0f / m_fMapSize;

    // Calculate the screen position for the marker
    vecLocalPos.fX = static_cast<float>(m_iMapMinX) + (fX / fRatio);
    vecLocalPos.fY = static_cast<float>(m_iMapMaxY) - (fY / fRatio);

    // If the position is on the screen
    if (vecLocalPos.fX >= 0.0f && vecLocalPos.fX <= static_cast<float>(m_uiWidth) && vecLocalPos.fY >= 0.0f && vecLocalPos.fY <= static_cast<float>(m_uiHeight))
    {
        // Then return true as it is on the screen
        return true;
    }

    // Return false as it is not on the screen
    return false;
}

void CPlayerMap::SetupMapVariables()
{
    float baseDimension = static_cast<float>(std::min(m_uiWidth, m_uiHeight));
    m_fMapSize = baseDimension * m_fZoom;
    float middleX = static_cast<float>(m_uiWidth) / 2.0f;
    float middleY = static_cast<float>(m_uiHeight) / 2.0f;

    if (m_bIsAttachedToLocal)
    {
        CVector        vec(0.0f, 0.0f, 0.0f);
        CClientPlayer* pLocalPlayer = m_pManager->GetPlayerManager()->GetLocalPlayer();
        if (pLocalPlayer)
            pLocalPlayer->GetPosition(vec);

        m_iHorizontalMovement = static_cast<int>(vec.fX);
        m_iVerticalMovement = static_cast<int>(vec.fY);
    }

    // Direct center-relative projection
    float mapMinX = middleX - ((static_cast<float>(m_iHorizontalMovement) + 3000.0f) * m_fMapSize / 6000.0f);
    float mapMinY = middleY - ((3000.0f - static_cast<float>(m_iVerticalMovement)) * m_fMapSize / 6000.0f);

    m_iMapMinX = static_cast<int>(std::round(mapMinX));
    m_iMapMaxX = static_cast<int>(std::round(mapMinX + m_fMapSize));
    m_iMapMinY = static_cast<int>(std::round(mapMinY));
    m_iMapMaxY = static_cast<int>(std::round(mapMinY + m_fMapSize));
}

void CPlayerMap::ZoomIn()
{
    if (m_fZoom < 16.0f)
    {
        m_fZoom = std::clamp(m_fZoom * 1.25f, 0.45f, 16.0f);
        SetupMapVariables();
    }
}

void CPlayerMap::ZoomOut()
{
    if (m_fZoom > 0.45f)
    {
        m_fZoom = std::clamp(m_fZoom / 1.25f, 0.45f, 16.0f);
        SetupMapVariables();
    }
}

void CPlayerMap::MoveNorth()
{
    SetAttachedToLocalPlayer(false);
    m_iVerticalMovement += static_cast<int>(300.0f / m_fZoom);
    SetupMapVariables();
}

void CPlayerMap::MoveSouth()
{
    SetAttachedToLocalPlayer(false);
    m_iVerticalMovement -= static_cast<int>(300.0f / m_fZoom);
    SetupMapVariables();
}

void CPlayerMap::MoveEast()
{
    SetAttachedToLocalPlayer(false);
    m_iHorizontalMovement += static_cast<int>(300.0f / m_fZoom);
    SetupMapVariables();
}

void CPlayerMap::MoveWest()
{
    SetAttachedToLocalPlayer(false);
    m_iHorizontalMovement -= static_cast<int>(300.0f / m_fZoom);
    SetupMapVariables();
}

void CPlayerMap::SetAttachedToLocalPlayer(bool bIsAttachedToLocal)
{
    m_bIsAttachedToLocal = bIsAttachedToLocal;
    SetupMapVariables();
}

bool CPlayerMap::IsPlayerMapShowing()
{
    return ((m_bIsPlayerMapEnabled || m_bForcedState) && m_mapImageTexture && m_playerMarkerTexture &&
            (!g_pCore->GetConsole()->IsVisible() && !g_pCore->IsMenuVisible()));
}

bool CPlayerMap::GetBoundingBox(CVector& vecMin, CVector& vecMax)
{
    // If our map image exists (Values are not calculated unless map is showing)
    if (IsPlayerMapShowing())
    {
        vecMin.fX = static_cast<float>(m_iMapMinX);
        vecMin.fY = static_cast<float>(m_iMapMinY);

        vecMax.fX = static_cast<float>(m_iMapMaxX);
        vecMax.fY = static_cast<float>(m_iMapMaxY);

        return true;
    }
    else
    {
        return false;
    }
}

void CPlayerMap::ToggleHelpText()
{
    m_bHideHelpText = !m_bHideHelpText;
}

SString CPlayerMap::GetBoundKeyName(const SString& strCommand)
{
    CCommandBind* pCommandBind = g_pCore->GetKeyBinds()->GetBindFromCommand(strCommand, 0, 0, 0, false, 0);
    if (!pCommandBind)
        return strCommand;
    return pCommandBind->boundKey->szKey;
}

bool CPlayerMap::ProcessMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!IsPlayerMapShowing())
        return false;

    switch (uMsg)
    {
        case WM_KEYDOWN:
        {
            if (wParam == VK_SPACE)
            {
                SetAttachedToLocalPlayer(true);
                return true;
            }
            break;
        }

        case WM_MBUTTONDOWN:
        {
            m_cursorEnabled = !m_cursorEnabled;
            g_pCore->ForceCursorVisible(m_cursorEnabled, m_cursorEnabled);
            if (!m_cursorEnabled)
            {
                m_isDragging = false;
            }
            return true;
        }

        case WM_LBUTTONDOWN:
        {
            if (!m_cursorEnabled)
                return false;

            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);

            m_isDragging = true;
            m_dragStartCursor = CVector2D(static_cast<float>(pt.x), static_cast<float>(pt.y));

            if (m_bIsAttachedToLocal)
            {
                CVector localPos;
                if (CClientPlayer* localPlayer = m_pManager->GetPlayerManager()->GetLocalPlayer())
                {
                    localPlayer->GetPosition(localPos);
                    m_iHorizontalMovement = static_cast<int>(localPos.fX);
                    m_iVerticalMovement = static_cast<int>(localPos.fY);
                }
            }

            m_dragStartHorizontal = m_iHorizontalMovement;
            m_dragStartVertical = m_iVerticalMovement;
            return true;
        }

        case WM_MOUSEMOVE:
        {
            if (m_isDragging && (wParam & MK_LBUTTON))
            {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hwnd, &pt);

                float deltaScreenX = static_cast<float>(pt.x) - m_dragStartCursor.fX;
                float deltaScreenY = static_cast<float>(pt.y) - m_dragStartCursor.fY;

                float deltaWorldX = deltaScreenX * (6000.0f / m_fMapSize);
                float deltaWorldY = deltaScreenY * (6000.0f / m_fMapSize);

                m_iHorizontalMovement = static_cast<int>(static_cast<float>(m_dragStartHorizontal) - deltaWorldX);
                m_iVerticalMovement = static_cast<int>(static_cast<float>(m_dragStartVertical) + deltaWorldY);

                SetAttachedToLocalPlayer(false);
                SetupMapVariables();
                return true;
            }
            return false;
        }

        case WM_LBUTTONUP:
        {
            if (m_isDragging)
            {
                m_isDragging = false;
                return true;
            }
            return false;
        }

        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        {
            if (uMsg == WM_RBUTTONDOWN)
            {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hwnd, &pt);

                float worldX = ((static_cast<float>(pt.x) - static_cast<float>(m_iMapMinX)) * 6000.0f / m_fMapSize) - 3000.0f;
                float worldY = 3000.0f - ((static_cast<float>(pt.y) - static_cast<float>(m_iMapMinY)) * 6000.0f / m_fMapSize);

                ToggleWaypoint(worldX, worldY);
                return true;
            }
            return true;
        }

        case WM_MOUSEWHEEL:
        {
            short deltaWheel = GET_WHEEL_DELTA_WPARAM(wParam);
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);

            float targetX = m_cursorEnabled ? static_cast<float>(pt.x) : static_cast<float>(m_uiWidth) / 2.0f;
            float targetY = m_cursorEnabled ? static_cast<float>(pt.y) : static_cast<float>(m_uiHeight) / 2.0f;

            if (deltaWheel > 0)
            {
                ZoomAtCursor(1.20f, targetX, targetY);
            }
            else if (deltaWheel < 0)
            {
                ZoomAtCursor(1.0f / 1.20f, targetX, targetY);
            }
            return true;
        }

        default:
            break;
    }

    return false;
}

void CPlayerMap::ZoomAtCursor(float factor, float cursorX, float cursorY)
{
    float oldZoom = m_fZoom;
    float newZoom = std::clamp(m_fZoom * factor, 0.45f, 16.0f);

    if (std::abs(newZoom - oldZoom) < 0.001f)
        return;

    // World coordinate currently under cursor before zoom
    float worldX = ((cursorX - static_cast<float>(m_iMapMinX)) * 6000.0f / m_fMapSize) - 3000.0f;
    float worldY = 3000.0f - ((cursorY - static_cast<float>(m_iMapMinY)) * 6000.0f / m_fMapSize);

    m_fZoom = newZoom;
    m_savedZoomLevel = newZoom;
    g_pCore->GetCVars()->Set("map_zoom", m_savedZoomLevel);

    float baseDimension = static_cast<float>(std::min(m_uiWidth, m_uiHeight));
    m_fMapSize = baseDimension * m_fZoom;

    // Calculate new center offset so that (worldX, worldY) stays at the exact same screen pixel (cursorX, cursorY)
    float middleX = static_cast<float>(m_uiWidth) / 2.0f;
    float middleY = static_cast<float>(m_uiHeight) / 2.0f;

    m_iHorizontalMovement = static_cast<int>(worldX + ((middleX - cursorX) * 6000.0f / m_fMapSize));
    m_iVerticalMovement = static_cast<int>(worldY - ((middleY - cursorY) * 6000.0f / m_fMapSize));

    SetAttachedToLocalPlayer(false);
    SetupMapVariables();
}

void CPlayerMap::ToggleWaypoint(float worldX, float worldY)
{
    if (m_waypointMarker)
    {
        CVector position;
        m_waypointMarker->GetPosition(position);
        float deltaX = position.fX - worldX;
        float deltaY = position.fY - worldY;
        if (std::sqrt(deltaX * deltaX + deltaY * deltaY) < 40.0f)
        {
            ClearWaypoint();
            return;
        }
    }

    if (!m_waypointMarker)
    {
        m_waypointMarker = new CClientRadarMarker(m_pManager, INVALID_ELEMENT_ID, 0, 0);
        m_waypointMarker->SetSprite(41);
        m_waypointMarker->SetScale(2);
        m_waypointMarker->SetColor(SColorRGBA(235, 45, 45, 230));
    }
    m_waypointMarker->SetPosition(CVector(worldX, worldY, 10.0f));
    m_waypointMarker->SetVisible(true);
}

void CPlayerMap::ClearWaypoint()
{
    if (m_waypointMarker)
    {
        delete m_waypointMarker;
        m_waypointMarker = nullptr;
    }
}
