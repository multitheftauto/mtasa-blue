/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CMarker.h
 *  PURPOSE:     Marker entity interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"

class CEntity;
class CObject;
class CPed;
class CVector;
class CVehicle;

#define MARKER_SCALE_SMALL          1
#define MARKER_SCALE_NORMAL         2

class CMarker
{
public:
    /* Our Functions */
    virtual void     SetSprite(eMarkerSprite Sprite) = 0;
    virtual void     SetDisplay(eMarkerDisplay wDisplay) = 0;
    virtual void     SetScale(WORD wScale) = 0;
    virtual void     SetColor(eMarkerColor color) = 0;
    virtual void     SetColor(const SharedUtil::SColor color) = 0;
    virtual void     Remove() = 0;
    virtual bool     IsActive() = 0;
    virtual void     SetPosition(CVector* vecPosition) = 0;
    virtual CVector* GetPosition() = 0;
};
