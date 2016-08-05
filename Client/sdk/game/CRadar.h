/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CRadar.h
*  PURPOSE:     Game radar interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_RADAR
#define __CGAME_RADAR

#include "Common.h"

#include "CMarker.h"
#include "CObject.h"
#include "CPed.h"
#include "CVehicle.h"

#include <windows.h>

class CRadar
{
public:
    virtual CMarker             * CreateMarker(CVector * vecPosition)=0;
    virtual CMarker             * CreateMarker(CVehicle * vehicle)=0;
    virtual CMarker             * CreateMarker(CObject * object)=0;
    virtual CMarker             * CreateMarker(CPed * ped)=0;

    virtual CMarker             * GetFreeMarker(  )=0;
    virtual CMarker             * GetMarker( DWORD dwMarkerID )=0;

    virtual VOID                ClearMarkerForEntity(CVehicle * vehicle)=0;
    virtual VOID                ClearMarkerForEntity(CObject * object)=0;
    virtual VOID                ClearMarkerForEntity(CPed * ped)=0;

    virtual VOID                DrawAreaOnRadar(float fX1, float fY1, float fX2, float fY2, const SColor color )=0;
};

#endif
