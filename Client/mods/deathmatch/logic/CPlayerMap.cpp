/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPlayerMap.cpp
 *  PURPOSE:     Full screen player map renderer
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

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
    m_bTextVisible = false;

    // Set the update time to the current time
    m_ulUpdateTime = GetTickCount32();

    // Get the window sizes and set the map variables to default zoom/movement
    m_uiHeight = g_pCore->GetGraphics()->GetViewportHeight();
    m_uiWidth = g_pCore->GetGraphics()->GetViewportWidth();
    m_fZoom = 1;
    m_iHorizontalMovement = 0;
    m_iVerticalMovement = 0;

    // Init texture vars
    m_mapImageTexture = nullptr;
    m_playerMarkerTexture = nullptr;

    // Create all map textures
    CreateAllTextures();

    // Create the text displays for the help text
    const SColorRGBA colorWhiteTransparent(255, 255, 255, 200);
    const SColorRGBA colorWhite(255, 255, 255, 255);
    struct
    {
        SColor  color;
        float   fPosY;
        float   fScale;
        SString strMessage;
    } messageList[] = {
        {colorWhiteTransparent, 0.92f, 1.5f, ""},
        {colorWhite, 0.95f, 1.0f, SString(_("Change mode: %s"), *GetBoundKeyName("radar_attach"))},

        {colorWhite, 0.05f, 1.0f, SString(_("Zoom: %s/%s     Movement: %s, %s, %s, %s     Opacity: %s/%s"),
                 *GetBoundKeyName("radar_zoom_in"), *GetBoundKeyName("radar_zoom_out"), *GetBoundKeyName("radar_move_north"),
                 *GetBoundKeyName("radar_move_east"), *GetBoundKeyName("radar_move_south"), *GetBoundKeyName("radar_move_west"),
                 *GetBoundKeyName("radar_opacity_down"), *GetBoundKeyName("radar_opacity_up"))},
        {colorWhite, 0.07f, 1.0f, SString(_("Toggle map: %s     Toggle help text: %s"),
                 *GetBoundKeyName("radar"), *GetBoundKeyName("radar_help"))},
    };

    for (uint i = 0; i < NUMELMS(messageList); i++)
    {
        CClientTextDisplay* pTextDisplay = new CClientTextDisplay(m_pManager->GetDisplayManager());
        pTextDisplay->SetCaption(messageList[i].strMessage);
        pTextDisplay->SetColor(messageList[i].color);
        pTextDisplay->SetPosition(CVector(0.50f, messageList[i].fPosY, 0));
        pTextDisplay->SetFormat(DT_CENTER | DT_VCENTER);
        pTextDisplay->SetScale(messageList[i].fScale);
        pTextDisplay->SetVisible(false);

        m_HelpTextList.push_back(pTextDisplay);
    }

    // Default to attached to player
    SetAttachedToLocalPlayer(true);

    SetupMapVariables();
}

CPlayerMap::~CPlayerMap()
{
    // Delete our images
    SAFE_RELEASE(m_mapImageTexture);
    SAFE_RELEASE(m_playerMarkerTexture);
    for (uint i = 0; i < m_markerTextureList.size(); i++)
        SAFE_RELEASE(m_markerTextureList[i]);
    m_markerTextureList.clear();

    // Don't need to delete the help texts as those are destroyed by the display manager
}

void CPlayerMap::CreateOrUpdateMapTexture()
{
    const std::uint32_t mapSize = MAP_IMAGE_SIZES[m_playerMapImageIndex];
    const SString       fileName("MTA\\cgui\\images\\map_%d.png", mapSize);

    auto* newTexture = g_pCore->GetGraphics()->GetRenderItemManager()->CreateTexture(CalcMTASAPath(fileName), nullptr, false, mapSize, mapSize, RFORMAT_DXT1);
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
    m_playerMarkerTexture = g_pCore->GetGraphics()->GetRenderItemManager()->CreateTexture(CalcMTASAPath("MTA\\cgui\\images\\radarset\\02.png"));
    if (!m_playerMarkerTexture)
        throw std::runtime_error("Failed to load player blip image");
}

void CPlayerMap::CreateAllTextures()
{
    try
    {
        // Create the map texture
        m_playerMapImageIndex = g_pCore->GetCVars()->GetValue<std::size_t>("mapimage");
        CreateOrUpdateMapTexture();

        // Create the player blip texture
        CreatePlayerBlipTexture();

        // Create the other marker textures
        CreateMarkerTextures();
    }
    catch (const std::exception& e)
    {
        m_failedToLoadTextures = true;
        g_pCore->GetConsole()->Printf("Problem initializing player map: %s", e.what());
    }
}

void CPlayerMap::DoPulse()
{
    // If our map image exists
    if (IsPlayerMapShowing())
    {
        // If we are following the local player blip
        if (m_bIsAttachedToLocal)
        {
            // Get the latest vars for the map
            SetupMapVariables();
        }

        // If the update time is more than 50ms behind
        if (GetTickCount32() >= m_ulUpdateTime + 50)
        {
            // Set the update time
            m_ulUpdateTime = GetTickCount32();

            // If we are set to moving then do a zoom/move level jump
            if (m_bIsMovingNorth)
            {
                MoveNorth();
            }
            else if (m_bIsMovingSouth)
            {
                MoveSouth();
            }
            else if (m_bIsMovingEast)
            {
                MoveEast();
            }
            else if (m_bIsMovingWest)
            {
                MoveWest();
            }
        }
    }
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
        fScale = 1;
    }
    else
    {
        // ulSprite == 0 so draw a square or triangle depending on relative z position
        CVector vecMarker;
        pMarker->GetPosition(vecMarker);

        if (fLocalZ > vecMarker.fZ + 4.0f)
            uiListIndex = MARKER_DOWN_TRIANGLE_INDEX;            // We're higher than this marker, so draw the arrow pointing down
        else if (fLocalZ < vecMarker.fZ - 4.0f)
            uiListIndex = MARKER_UP_TRIANGLE_INDEX;            // We're lower than this entity, so draw the arrow pointing up
        else
            uiListIndex = MARKER_SQUARE_INDEX;            // We're at the same level so draw a square

        fScale /= 4;
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

    // Render if showing and textures are all loaded
    if (isMapShowing && !m_failedToLoadTextures)
    {
        // Get the alpha value from the settings
        int mapAlpha;
        g_pCore->GetCVars()->Get("mapalpha", mapAlpha);
        const SColorARGB mapColor(mapAlpha, 255, 255, 255);

        // Update the image if the user changed it via a setting
        auto mapImageIndex = g_pCore->GetCVars()->GetValue<std::size_t>("mapimage");
        if (mapImageIndex != m_playerMapImageIndex)
        {
            UpdateOrRevertMapTexture(mapImageIndex);
        }

        g_pCore->GetGraphics()->DrawTexture(m_mapImageTexture, static_cast<float>(m_iMapMinX), static_cast<float>(m_iMapMinY),
                                            m_fMapSize / m_mapImageTexture->m_uiSizeX, m_fMapSize / m_mapImageTexture->m_uiSizeY, 0.0f, 0.0f, 0.0f, mapColor);

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

                g_pCore->GetGraphics()->DrawRectangle(vecPos.fX, vecPos.fY, vecSize.fX, -vecSize.fY, color);
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
                    g_pCore->GetGraphics()->DrawTexture(pTexture, vecPos.fX, vecPos.fY, fScale, fScale, 0.0f, 0.5f, 0.5f, color);
                }
            }
        }

        g_pCore->GetGraphics()->DrawTexture(m_playerMarkerTexture, vecLocalPos.fX, vecLocalPos.fY, 1.0, 1.0, vecLocalRot.fZ, 0.5f, 0.5f);
    }

    // Update visibility of help text
    bool bRequiredTextVisible = isMapShowing && !m_bHideHelpText;
    if (bRequiredTextVisible != m_bTextVisible)
    {
        m_bTextVisible = bRequiredTextVisible;
        for (uint i = 0; i < m_HelpTextList.size(); i++)
            m_HelpTextList[i]->SetVisible(m_bTextVisible);

        SetupMapVariables();
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
    }
    else
    {
        g_pGame->GetHud()->Disable(false);
        g_pMultiplayer->HideRadar(false);
        g_pCore->SetChatVisible(m_bChatVisible, m_bChatInputBlocked);
        g_pCore->SetDebugVisible(m_bDebugVisible);
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
    // Calculate the map size and the middle of the screen coords
    m_fMapSize = static_cast<float>(m_uiHeight * m_fZoom);
    int iMiddleX = static_cast<int>(m_uiWidth / 2);
    int iMiddleY = static_cast<int>(m_uiHeight / 2);

    // If we are attached to the local player and zoomed in at all
    if (m_bIsAttachedToLocal && m_fZoom > 1)
    {
        // Get the local player position
        CVector        vec;
        CClientPlayer* pLocalPlayer = m_pManager->GetPlayerManager()->GetLocalPlayer();
        if (pLocalPlayer)
            pLocalPlayer->GetPosition(vec);

        // Calculate the maps min and max vector positions putting the local player in the middle of the map
        m_iMapMinX = static_cast<int>(iMiddleX - (iMiddleY * m_fZoom) - ((vec.fX * m_fMapSize) / 6000.0f));
        m_iMapMaxX = static_cast<int>(m_iMapMinX + m_fMapSize);
        m_iMapMinY = static_cast<int>(iMiddleY - (iMiddleY * m_fZoom) + ((vec.fY * m_fMapSize) / 6000.0f));
        m_iMapMaxY = static_cast<int>(m_iMapMinY + m_fMapSize);

        // If we are moving the map too far then stop centering the local player blip
        if (m_iMapMinX > 0)
        {
            m_iMapMinX = 0;
            m_iMapMaxX = static_cast<int>(m_iMapMinX + m_fMapSize);
        }
        else if (m_iMapMaxX <= static_cast<int>(m_uiWidth))
        {
            m_iMapMaxX = m_uiWidth;
            m_iMapMinX = static_cast<int>(m_iMapMaxX - m_fMapSize);
        }

        if (m_iMapMinY > 0)
        {
            m_iMapMinY = 0;
            m_iMapMaxY = static_cast<int>(m_iMapMinY + m_fMapSize);
        }
        else if (m_iMapMaxY <= static_cast<int>(m_uiHeight))
        {
            m_iMapMaxY = m_uiHeight;
            m_iMapMinY = static_cast<int>(m_iMapMaxY - m_fMapSize);
        }
    }
    // If we are in free roam mode or not zoomed in
    else
    {
        // Set the maps min and max vector positions relative to the movement selected
        m_iMapMinX = static_cast<int>(iMiddleX - (iMiddleY * m_fZoom) - ((m_iHorizontalMovement * m_fMapSize) / 6000.0f));
        m_iMapMaxX = static_cast<int>(m_iMapMinX + m_fMapSize);
        m_iMapMinY = static_cast<int>(iMiddleY - (iMiddleY * m_fZoom) + ((m_iVerticalMovement * m_fMapSize) / 6000.0f));
        m_iMapMaxY = static_cast<int>(m_iMapMinY + m_fMapSize);

        // If we are zoomed in
        if (m_fZoom > 1)
        {
            if (m_iMapMinX >= 0)
            {
                m_iMapMinX = 0;
                m_iMapMaxX = static_cast<int>(m_iMapMinX + m_fMapSize);
            }
            else if (m_iMapMaxX <= static_cast<int>(m_uiWidth))
            {
                m_iMapMaxX = m_uiWidth;
                m_iMapMinX = static_cast<int>(m_iMapMaxX - m_fMapSize);
            }

            if (m_iMapMinY >= 0)
            {
                m_iMapMinY = 0;
                m_iMapMaxY = static_cast<int>(m_iMapMinY + m_fMapSize);
            }
            else if (m_iMapMaxY <= static_cast<int>(m_uiHeight))
            {
                m_iMapMaxY = m_uiHeight;
                m_iMapMinY = static_cast<int>(m_iMapMaxY - m_fMapSize);
            }
        }
        // If we are not zoomed in
        else
        {
            // Set the movement margins to 0
            m_iHorizontalMovement = 0;
            m_iVerticalMovement = 0;
        }
    }

    // Show mode only when zoomed in
    if (!m_HelpTextList.empty())
    {
        m_HelpTextList[0]->SetVisible(m_fZoom > 1 && m_bTextVisible);
        m_HelpTextList[1]->SetVisible(m_fZoom > 1 && m_bTextVisible);
    }
}

void CPlayerMap::ZoomIn()
{
    if (m_fZoom <= 4)
    {
        m_fZoom = m_fZoom * 2;
        SetupMapVariables();
    }
}

void CPlayerMap::ZoomOut()
{
    if (m_fZoom >= 1)
    {
        m_fZoom = m_fZoom / 2;

        if (m_fZoom > 1)
        {
            m_iVerticalMovement = static_cast<int>(m_iVerticalMovement / 1.7f);
            m_iHorizontalMovement = static_cast<int>(m_iHorizontalMovement / 1.7f);
        }
        else
        {
            m_iVerticalMovement = 0;
            m_iHorizontalMovement = 0;
            // Stop the movement
            m_bIsMovingNorth = false;
            m_bIsMovingSouth = false;
            m_bIsMovingEast = false;
            m_bIsMovingWest = false;
        }

        SetupMapVariables();
    }
}

void CPlayerMap::MoveNorth()
{
    if (!m_bIsAttachedToLocal)
    {
        if (m_fZoom > 1)
        {
            if (m_iMapMinY >= 0)
            {
                m_iMapMinY = 0;
                m_iMapMaxY = static_cast<int>(m_iMapMinY + m_fMapSize);
            }
            else
            {
                m_iVerticalMovement = m_iVerticalMovement + 20;
                SetupMapVariables();
            }
        }
    }
}

void CPlayerMap::MoveSouth()
{
    if (!m_bIsAttachedToLocal)
    {
        if (m_fZoom > 1)
        {
            if (m_iMapMaxY <= static_cast<int>(m_uiHeight))
            {
                m_iMapMaxY = m_uiHeight;
                m_iMapMinY = static_cast<int>(m_iMapMaxY - m_fMapSize);
            }
            else
            {
                m_iVerticalMovement = m_iVerticalMovement - 20;
                SetupMapVariables();
            }
        }
    }
}

void CPlayerMap::MoveEast()
{
    if (!m_bIsAttachedToLocal)
    {
        if (m_fZoom > 1)
        {
            if (m_iMapMaxX <= static_cast<int>(m_uiWidth))
            {
                m_iMapMaxX = m_uiWidth;
                m_iMapMinX = static_cast<int>(m_iMapMaxX - m_fMapSize);
            }
            else
            {
                m_iHorizontalMovement = m_iHorizontalMovement + 20;
                SetupMapVariables();
            }
        }
    }
}

void CPlayerMap::MoveWest()
{
    if (!m_bIsAttachedToLocal)
    {
        if (m_fZoom > 1)
        {
            if (m_iMapMinX >= 0)
            {
                m_iMapMinX = 0;
                m_iMapMaxX = static_cast<int>(m_iMapMinX + m_fMapSize);
            }
            else
            {
                m_iHorizontalMovement = m_iHorizontalMovement - 20;
                SetupMapVariables();
            }
        }
    }
}

void CPlayerMap::SetAttachedToLocalPlayer(bool bIsAttachedToLocal)
{
    m_bIsAttachedToLocal = bIsAttachedToLocal;
    SetupMapVariables();

    if (m_bIsAttachedToLocal)
        m_HelpTextList[0]->SetCaption(_("Following Player"));
    else
        m_HelpTextList[0]->SetCaption(_("Free Movement"));
}

bool CPlayerMap::IsPlayerMapShowing()
{
    return ((m_bIsPlayerMapEnabled || m_bForcedState) && m_mapImageTexture && m_playerMarkerTexture && (!g_pCore->GetConsole()->IsVisible() && !g_pCore->IsMenuVisible()));
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
