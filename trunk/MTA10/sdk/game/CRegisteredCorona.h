/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CRegisteredCorona.h
*  PURPOSE:		Registered corona entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_REGISTEREDCORONA
#define __CGAME_REGISTEREDCORONA

#include "Common.h"
#include <CVector.h>

#include <windows.h>
#include "RenderWare.h"

class CRegisteredCorona
{
public:
	virtual CVector		* GetPosition()=0;
	virtual VOID			SetPosition(CVector * vector)=0;
	virtual FLOAT			GetSize()=0;
	virtual VOID			SetSize(FLOAT fSize)=0;
	virtual FLOAT			GetRange()=0;
	virtual VOID			SetRange(FLOAT fRange)=0;
	virtual FLOAT			GetPullTowardsCamera()=0;
	virtual VOID			SetPullTowardsCamera(FLOAT fPullTowardsCamera)=0;
	virtual VOID			SetColor(BYTE Red, BYTE Green, BYTE Blue, BYTE Alpha)=0;
	virtual VOID			SetTexture(RwTexture * texture)=0;
	virtual VOID			SetTexture(eCoronaType texture)=0;
	virtual BYTE			GetFlareType()=0;
	virtual VOID			SetFlareType(BYTE fFlareType)=0;
	virtual DWORD			GetIdentifier()=0;
	virtual DWORD			GetID()=0;
	virtual VOID			Refresh()=0;
	virtual VOID			Disable()=0;
};
#endif