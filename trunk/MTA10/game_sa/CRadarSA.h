/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRadarSA.h
*  PURPOSE:     Header file for game radar class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_RADAR
#define __CGAMESA_RADAR

#include <game/CRadar.h>
#include "CMarkerSA.h"

/**
 * \todo GAME RELEASE: Update CMarker array see 0x585fe0 for example
 */
#define ARRAY_CMarker						0xBA86F0 // ##SA##
#define	MAX_MARKERS							175 // ##SA##

//005853d0		public: static void __cdecl CRadar::DrawAreaOnRadar(class CRect const &,class CRGBA const &,bool)
#define FUNC_DrawAreaOnRadar				0x5853d0
//00583820        public: static int __cdecl CRadar::SetCoordBlip(enum eBlipType,class CVector,unsigned int,enum eBlipDisplay,char *)
#define FUNC_SetCoordBlip					0x583820

#define VAR_3DMarkerColorMission			0x68F958
#define VAR_3DMarkerColorNormal				0x68F95C

/**
 * \todo AT SOME POINT: Implement DrawAreaOnRadar
 */
class CRadarSA : public CRadar
{

public:
						CRadarSA (  );
                        ~CRadarSA ( );
	CMarker				* CreateMarker(CVector * vecPosition);
	CMarker				* CreateMarker(CVehicle * vehicle);
	CMarker				* CreateMarker(CObject * object);
	CMarker				* CreateMarker(CPed * ped);

	CMarker				* GetFreeMarker(  );
	CMarker				* GetMarker( DWORD dwMarkerID );

	VOID				ClearMarkerForEntity(CVehicle * vehicle);
	VOID				ClearMarkerForEntity(CObject * object);
	VOID				ClearMarkerForEntity(CPed * ped);

    VOID                DrawAreaOnRadar(float fX1, float fY1, float fX2, float fY2, const SColor color );
};

#endif