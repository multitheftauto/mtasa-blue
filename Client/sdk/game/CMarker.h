/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CMarker.h
 *  PURPOSE:     Marker entity interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "enums/MarkerSprite.h"
#include "enums/MarkerDisplay.h"
#include "enums/MarkerType.h"
#include "enums/MarkerColor.h"

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
    virtual void     SetSprite(MarkerSprite Sprite) = 0;
    virtual void     SetDisplay(MarkerDisplay wDisplay) = 0;
    virtual void     SetScale(WORD wScale) = 0;
    virtual void     SetColor(MarkerColor color) = 0;
    virtual void     SetColor(const SharedUtil::SColor color) = 0;
    virtual void     Remove() = 0;
    virtual bool     IsActive() = 0;
    virtual void     SetPosition(CVector* vecPosition) = 0;
    virtual CVector* GetPosition() = 0;
};
