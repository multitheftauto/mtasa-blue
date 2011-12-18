/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CMarkerSA.h
*  PURPOSE:     Header file for marker entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_MARKER
#define __CGAMESA_MARKER

#include <game/CMarker.h>
#include "Common.h"

/**
 * \todo Ignore this class?
 */
class CEntryExit;

class CMarkerSAInterface
{
public:
    /* GTA Variables */
    DWORD nColour;
    // The following parameter is used when Blip Type is CAR, CHAR or OBJECT
    long PoolIndex; 
    // The following parameter is used when Blip Type is COORDS 
    CVector position;//vec2DBlip;

    WORD ReferenceIndex;

    FLOAT fBlipMarkerScale;
    WORD nBlipScale;
    CEntryExit* pEntryExit;

    BYTE nBlipSprite;

    BYTE bBright                : 1;
    BYTE bTrackingBlip          : 1;
    BYTE bShortRange            : 1;
    BYTE bFriendly              : 1;    
    BYTE bBlipRemain            : 1;
    BYTE bBlipFade              : 1;
    BYTE nCoordBlipAppearance   : 2;

    BYTE nBlipDisplayFlag       : 2;
    BYTE BlipType               : 4;
};

class CMarkerSA : public CMarker
{
public:
    /* Our Functions */
                        CMarkerSA       ( CMarkerSAInterface * markerInterface ) { internalInterface = markerInterface; };

    void                Init            ( void );

    VOID                SetSprite ( eMarkerSprite Sprite );
    VOID                SetDisplay ( eMarkerDisplay wDisplay );
    VOID                SetScale ( WORD wScale );
    VOID                SetColor ( eMarkerColor color );
    VOID                SetColor ( const SColor color );
    VOID                Remove ( );
    BOOL                IsActive (  );
    VOID                SetPosition ( CVector * vecPosition );
    VOID                SetEntity ( CVehicle * vehicle );
    VOID                SetEntity ( CPed * ped );
    VOID                SetEntity ( CObject * object );
    CEntity             * GetEntity (  );
    CVector         * GetPosition (  );
    CMarkerSAInterface  * GetInterface (  ) { return internalInterface; };
private:
    CMarkerSAInterface  * internalInterface;
};

#endif
