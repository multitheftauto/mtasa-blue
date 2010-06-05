/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CMarkerSA.cpp
*  PURPOSE:		Marker entity
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CMarkerSA::Init ( void )
{
    internalInterface->position = CVector ( 0, 0, 0 );
    internalInterface->BlipType = (BYTE)MARKER_TYPE_COORDS;
	internalInterface->bTrackingBlip = 1;
	internalInterface->bBlipRemain = FALSE;
//	internalInterface->nColour = MARKER_COLOR_BLUE;
//	internalInterface->bBright = TRUE;
	internalInterface->bTrackingBlip = TRUE;
	internalInterface->bShortRange = FALSE;
	internalInterface->bFriendly = FALSE;
	internalInterface->fBlipMarkerScale = 2.0f;
	internalInterface->PoolIndex = 0;
	internalInterface->nBlipScale = 2;
	internalInterface->bBlipFade = FALSE;
	internalInterface->nBlipDisplayFlag = MARKER_DISPLAY_BLIPONLY;
	internalInterface->nBlipSprite = RADAR_SPRITE_NONE;
	#define COORD_BLIP_APPEARANCE_NORMAL 0
	internalInterface->nCoordBlipAppearance = COORD_BLIP_APPEARANCE_NORMAL;
	internalInterface->pEntryExit = NULL;
}

/**
 * Set the sprite used for this marker
 * @param wSprite a valid eMarkerSprite value. MARKER_SPRITE_NONE for the default sprite.
 */
VOID CMarkerSA::SetSprite ( eMarkerSprite Sprite = (eMarkerSprite)MARKER_SPRITE_NONE )
{
	DEBUG_TRACE("VOID CMarkerSA::SetSprite ( eMarkerSprite Sprite = (eMarkerSprite)MARKER_SPRITE_NONE )");
	internalInterface->nBlipSprite = Sprite;
}

/**
 * Sets how the marker is displayed in-game
 */
VOID CMarkerSA::SetDisplay ( eMarkerDisplay wDisplay )
{
	DEBUG_TRACE("VOID CMarkerSA::SetDisplay ( eMarkerDisplay wDisplay )");
	internalInterface->nBlipDisplayFlag = wDisplay;
}

/**
 * Set the size of the sprite
 * @param wScale the relative size of the sprite. 1 is default.
 */
VOID CMarkerSA::SetScale ( WORD wScale = MARKER_SCALE_NORMAL)
{
	DEBUG_TRACE("VOID CMarkerSA::SetScale ( WORD wScale = MARKER_SCALE_NORMAL)");
	internalInterface->nBlipScale = wScale;
	internalInterface->fBlipMarkerScale = 20.0f;
}

/**
 * Sets the color of the marker when MARKER_SPRITE_NONE is used
 * @param color eMarkerColor containing a valid colour id
 */
VOID CMarkerSA::SetColor ( eMarkerColor color )
{
	DEBUG_TRACE("VOID CMarkerSA::SetColor ( eMarkerColor color )");
	if(color >= MARKER_COLOR_PLUM && color <= MARKER_COLOR_DARK_TURQUOISE)
	{
		if(color >= MARKER_COLOR_RED && color <= MARKER_COLOR_DARK_TURQUOISE)
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
 * @param color RGBA containing a valid colour in RGBA format. Use the COLOR_RGBA macro to convert.
 */
VOID CMarkerSA::SetColor ( RGBA color )
{
	DEBUG_TRACE("VOID CMarkerSA::SetColor ( RGBA color )");
	internalInterface->nColour = color;
}

/**
 * \todo Complete CMarkerSA::Remove when CEntity is ready
 */
VOID CMarkerSA::Remove ( )
{
	DEBUG_TRACE("VOID CMarkerSA::Remove ( )");
	internalInterface->BlipType = MARKER_TYPE_UNUSED;
	internalInterface->nBlipDisplayFlag = MARKER_DISPLAY_NEITHER;
	internalInterface->nBlipSprite = MARKER_SPRITE_NONE;
	internalInterface->bTrackingBlip = false;
}

BOOL CMarkerSA::IsActive (  )
{
	DEBUG_TRACE("BOOL CMarkerSA::IsActive (  )");
	if(internalInterface->BlipType != MARKER_TYPE_UNUSED)
		return TRUE;
	else
		return FALSE;
}

VOID CMarkerSA::SetPosition ( CVector * vecPosition )
{
	DEBUG_TRACE("VOID CMarkerSA::SetPosition ( CVector * vecPosition )");
	memcpy(&internalInterface->position,vecPosition, sizeof(CVector));
}


VOID CMarkerSA::SetEntity ( CVehicle * vehicle )
{	
	DEBUG_TRACE("VOID CMarkerSA::SetEntity ( CVehicle * vehicle )");
	CPoolsSA * pPools = (CPoolsSA *)pGame->GetPools();
	DWORD dwID = pPools->GetVehicleRef((CVehicle*)vehicle);
	internalInterface->PoolIndex = dwID;
	internalInterface->BlipType = (BYTE)MARKER_TYPE_CAR;
}

VOID CMarkerSA::SetEntity ( CPed * ped )
{
	DEBUG_TRACE("VOID CMarkerSA::SetEntity ( CPed * ped )");
	CPoolsSA * pPools = (CPoolsSA *)pGame->GetPools();
	DWORD dwID = pPools->GetPedRef((CPed*)ped);
	internalInterface->PoolIndex = dwID;
	internalInterface->BlipType = (BYTE)MARKER_TYPE_CHAR;
}

VOID CMarkerSA::SetEntity ( CObject * object )
{
	DEBUG_TRACE("VOID CMarkerSA::SetEntity ( CObject * object )");
	CPoolsSA * pPools = (CPoolsSA *)pGame->GetPools();
	DWORD dwID = pPools->GetObjectRef((CObject*)object);
	internalInterface->PoolIndex = dwID;
	internalInterface->BlipType = (eMarkerType)MARKER_TYPE_OBJECT;
}

CEntity	* CMarkerSA::GetEntity (  )
{
	DEBUG_TRACE("CEntity * CMarkerSA::GetEntity (  )");
	CPoolsSA * pPools = (CPoolsSA *)pGame->GetPools();

	switch(internalInterface->BlipType)
	{
	case MARKER_TYPE_CAR:
		return (CEntity *)pPools->GetVehicleFromRef((DWORD)internalInterface->PoolIndex);
	case MARKER_TYPE_CHAR:
		return (CEntity *)pPools->GetPedFromRef((DWORD)internalInterface->PoolIndex);
	case MARKER_TYPE_OBJECT:
		return (CEntity *)pPools->GetObjectFromRef((DWORD)internalInterface->PoolIndex);
	default:
		return NULL;
	}	
}

CVector * CMarkerSA::GetPosition (  )
{
	DEBUG_TRACE("CVector * CMarkerSA::GetPosition (  )");
	return &internalInterface->position;
}
