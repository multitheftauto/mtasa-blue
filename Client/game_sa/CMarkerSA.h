/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CMarkerSA.h
 *  PURPOSE:     Header file for marker entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CMarker.h>

class CEntryExit;

class CMarkerSAInterface
{
public:
    /* GTA Variables */
    DWORD nColour;
    // The following parameter is used when Blip Type is CAR, CHAR or OBJECT
    long PoolIndex;
    // The following parameter is used when Blip Type is COORDS
    CVector position;            // vec2DBlip;

    WORD ReferenceIndex;

    float       fBlipMarkerScale;
    WORD        nBlipScale;
    CEntryExit* pEntryExit;

    BYTE nBlipSprite;

    BYTE bBright : 1;
    BYTE bTrackingBlip : 1;
    BYTE bShortRange : 1;
    BYTE bFriendly : 1;
    BYTE bBlipRemain : 1;
    BYTE bBlipFade : 1;
    BYTE nCoordBlipAppearance : 2;

    BYTE nBlipDisplayFlag : 2;
    BYTE BlipType : 4;
};

class CMarkerSA : public CMarker
{
public:
    CMarkerSA(CMarkerSAInterface* markerInterface) { internalInterface = markerInterface; };

    void Init();

    void                SetSprite(MarkerSprite Sprite);
    void                SetDisplay(MarkerDisplay wDisplay);
    void                SetScale(WORD wScale);
    void                SetColor(MarkerColor color);
    void                SetColor(const SharedUtil::SColor color);
    void                Remove();
    bool                IsActive();
    void                SetPosition(CVector* vecPosition);
    CVector*            GetPosition();
    CMarkerSAInterface* GetInterface() { return internalInterface; };

private:
    CMarkerSAInterface* internalInterface;
};
