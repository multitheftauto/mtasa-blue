/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/C3DMarkers.h
*  PURPOSE:		3D marker manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_3DMARKERS
#define __CGAME_3DMARKERS

#include <CVector.h>
#include "C3DMarker.h"

class C3DMarkers
{
public:
	virtual C3DMarker		* CreateMarker(DWORD Identifier, e3DMarkerType dwType, CVector * vecPosition, FLOAT fSize, FLOAT fPulseFraction, BYTE r, BYTE g, BYTE b, BYTE a )=0;
	virtual C3DMarker		* FindFreeMarker()=0;
};

#endif