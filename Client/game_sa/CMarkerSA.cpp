/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CMarkerSA.cpp
 *  PURPOSE:     Marker entity
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <CVector.h>
#include "CMarkerSA.h"
#include <enums/RadarSprite.h>

void CMarkerSA::Init()
{
    internalInterface->position = CVector(0, 0, 0);
    internalInterface->BlipType = (BYTE)MarkerType::MARKER_TYPE_COORDS;
    internalInterface->bBlipRemain = false;
    //  internalInterface->nColour = MARKER_COLOR_BLUE;
    //  internalInterface->bBright = TRUE;
    internalInterface->bTrackingBlip = true;
    internalInterface->bShortRange = false;
    internalInterface->bFriendly = false;
    internalInterface->fBlipMarkerScale = 2.0f;
    internalInterface->PoolIndex = 0;
    internalInterface->nBlipScale = 2;
    internalInterface->bBlipFade = false;
    internalInterface->nBlipDisplayFlag = (BYTE)MarkerDisplay::MARKER_DISPLAY_BLIPONLY;
    internalInterface->nBlipSprite = (BYTE)RadarSprite::RADAR_SPRITE_NONE;
    #define COORD_BLIP_APPEARANCE_NORMAL 0
    internalInterface->nCoordBlipAppearance = COORD_BLIP_APPEARANCE_NORMAL;
    internalInterface->pEntryExit = nullptr;
}

/**
 * Set the sprite used for this marker
 * @param wSprite a valid eMarkerSprite value. MARKER_SPRITE_NONE for the default sprite.
 */
void CMarkerSA::SetSprite(MarkerSprite Sprite = MarkerSprite::MARKER_SPRITE_NONE)
{
    if (Sprite >= MarkerSprite::MARKER_SPRITE_NONE && Sprite <= MarkerSprite::MARKER_SPRITE_SPRAY)
    {
        internalInterface->nBlipSprite = (BYTE)Sprite;
    }
}

/**
 * Sets how the marker is displayed in-game
 */
void CMarkerSA::SetDisplay(MarkerDisplay wDisplay)
{
    internalInterface->nBlipDisplayFlag = (BYTE)wDisplay;
}

/**
 * Set the size of the sprite
 * @param wScale the relative size of the sprite. 1 is default.
 */
void CMarkerSA::SetScale(WORD wScale = MARKER_SCALE_NORMAL)
{
    internalInterface->nBlipScale = wScale;
    internalInterface->fBlipMarkerScale = 20.0f;
}

/**
 * Sets the color of the marker when MARKER_SPRITE_NONE is used
 * @param color eMarkerColor containing a valid colour id
 */
void CMarkerSA::SetColor(MarkerColor color)
{
    if (color >= MarkerColor::MARKER_COLOR_PLUM && color <= MarkerColor::MARKER_COLOR_DARK_TURQUOISE)
    {
        if (color >= MarkerColor::MARKER_COLOR_RED && color <= MarkerColor::MARKER_COLOR_DARK_TURQUOISE)
        {
            internalInterface->bBright = 1;
            internalInterface->nColour = static_cast<DWORD>(color) - static_cast<DWORD>(MarkerColor::MARKER_COLOR_RED);
        }
        else
        {
            internalInterface->bBright = 0;
            internalInterface->nColour = static_cast<DWORD>(color);
        }
    }
}

/**
 * Sets the color of the marker when MARKER_SPRITE_NONE is used
 * @param color RGBA containing a valid colour in RGBA format.
 */
void CMarkerSA::SetColor(const SharedUtil::SColor color)
{
    // Convert to required rgba at the last moment
    internalInterface->nColour = color.R << 24 | color.G << 16 | color.B << 8 | color.A;
}

/**
 * \todo Complete CMarkerSA::Remove when CEntity is ready
 */
void CMarkerSA::Remove()
{
    internalInterface->BlipType = (BYTE)MarkerType::MARKER_TYPE_UNUSED;
    internalInterface->nBlipDisplayFlag = (BYTE)MarkerDisplay::MARKER_DISPLAY_NEITHER;
    internalInterface->nBlipSprite = (BYTE)MarkerSprite::MARKER_SPRITE_NONE;
    internalInterface->bTrackingBlip = false;
}

bool CMarkerSA::IsActive()
{
    return internalInterface->BlipType != (BYTE)MarkerType::MARKER_TYPE_UNUSED;
}

void CMarkerSA::SetPosition(CVector* vecPosition)
{
    MemCpyFast(&internalInterface->position, vecPosition, sizeof(CVector));
}

CVector* CMarkerSA::GetPosition()
{
    return &internalInterface->position;
}
