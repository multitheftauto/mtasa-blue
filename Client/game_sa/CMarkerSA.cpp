/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CMarkerSA.cpp
 *  PURPOSE:     Marker entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <CVector.h>
#include "CMarkerSA.h"

void CMarkerSA::Init()
{
    internalInterface->position = CVector(0, 0, 0);
    internalInterface->BlipType = (BYTE)MARKER_TYPE_COORDS;
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
    internalInterface->nBlipDisplayFlag = MARKER_DISPLAY_BLIPONLY;
    internalInterface->nBlipSprite = RADAR_SPRITE_NONE;
    #define COORD_BLIP_APPEARANCE_NORMAL 0
    internalInterface->nCoordBlipAppearance = COORD_BLIP_APPEARANCE_NORMAL;
    internalInterface->pEntryExit = nullptr;
}

/**
 * Set the sprite used for this marker
 * @param wSprite a valid eMarkerSprite value. MARKER_SPRITE_NONE for the default sprite.
 */
void CMarkerSA::SetSprite(eMarkerSprite Sprite = (eMarkerSprite)MARKER_SPRITE_NONE)
{
    if (Sprite >= MARKER_SPRITE_NONE && Sprite <= MARKER_SPRITE_SPRAY)
    {
        internalInterface->nBlipSprite = Sprite;
    }
}

/**
 * Sets how the marker is displayed in-game
 */
void CMarkerSA::SetDisplay(eMarkerDisplay wDisplay)
{
    internalInterface->nBlipDisplayFlag = wDisplay;
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
void CMarkerSA::SetColor(eMarkerColor color)
{
    if (color >= MARKER_COLOR_PLUM && color <= MARKER_COLOR_DARK_TURQUOISE)
    {
        if (color >= MARKER_COLOR_RED && color <= MARKER_COLOR_DARK_TURQUOISE)
        {
            internalInterface->bBright = 1;
            internalInterface->nColour = color - MARKER_COLOR_RED;
        }
        else
        {
            internalInterface->bBright = 0;
            internalInterface->nColour = color;
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
    internalInterface->BlipType = MARKER_TYPE_UNUSED;
    internalInterface->nBlipDisplayFlag = MARKER_DISPLAY_NEITHER;
    internalInterface->nBlipSprite = MARKER_SPRITE_NONE;
    internalInterface->bTrackingBlip = false;
}

bool CMarkerSA::IsActive()
{
    return internalInterface->BlipType != MARKER_TYPE_UNUSED;
}

void CMarkerSA::SetPosition(CVector* vecPosition)
{
    MemCpyFast(&internalInterface->position, vecPosition, sizeof(CVector));
}

CVector* CMarkerSA::GetPosition()
{
    return &internalInterface->position;
}
