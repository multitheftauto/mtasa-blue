/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CMarker.h
*  PURPOSE:		Marker entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_MARKER
#define __CGAME_MARKER

#include "Common.h"
#include "CEntity.h"
#include "CObject.h"
#include "CPed.h"
#include "CVehicle.h"

#include <windows.h>

#define MARKER_SCALE_SMALL			1
#define MARKER_SCALE_NORMAL			2

class CMarker
{
public:
	/* Our Functions */
	virtual VOID				SetSprite ( eMarkerSprite Sprite )=0;
	virtual VOID				SetDisplay ( eMarkerDisplay wDisplay )=0;
	virtual VOID				SetScale ( WORD wScale )=0;
	virtual VOID				SetColor ( eMarkerColor color )=0;
	virtual VOID				SetColor ( RGBA color )=0;
	virtual VOID				Remove ( )=0;
	virtual BOOL				IsActive (  )=0;
	virtual VOID				SetPosition ( CVector * vecPosition )=0;
	virtual VOID				SetEntity ( CVehicle * vehicle )=0;
	virtual VOID				SetEntity ( CPed * ped )=0;
	virtual VOID				SetEntity ( CObject * object )=0;
	virtual CEntity				* GetEntity (  )=0;
	virtual CVector			* GetPosition (  )=0;
};

#endif