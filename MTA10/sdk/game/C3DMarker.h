/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/C3DMarker.h
*  PURPOSE:		3D marker entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_3DMARKER
#define __CGAME_3DMARKER

#include "Common.h"
#include "SharedUtil.h"

class CVector;
struct RpClump;
class CMatrix;

/**
 * \todo fix SetColor/GetColor, the format is actually BGRA (strange)
 */
class C3DMarker
{
public:
    virtual                 ~C3DMarker ( void ) {};

    virtual void            GetMatrix       ( CMatrix * pMatrix )=0;
    virtual void            SetMatrix       ( CMatrix * pMatrix )=0;
	virtual VOID			SetPosition(CVector * vecPosition)=0;
	virtual CVector			* GetPosition()=0;
	virtual DWORD			GetType()=0; // need enum?
	virtual BOOL			IsActive()=0;
	virtual DWORD			GetIdentifier()=0;
    virtual SColor          GetColor()=0;
	virtual VOID			SetColor(const SColor color)=0;
	virtual VOID			SetPulsePeriod(WORD wPulsePeriod)=0;
	virtual VOID			SetPulseFraction(FLOAT fPulseFraction)=0;
	virtual VOID			SetRotateRate(short RotateRate)=0;
	virtual FLOAT			GetSize()=0;
	virtual VOID			SetSize(FLOAT fSize)=0;
	virtual FLOAT			GetBrightness()=0;
	virtual VOID			SetBrightness(FLOAT fBrightness)=0;
	virtual VOID			SetCameraRange(FLOAT fCameraRange)=0;
	virtual FLOAT			GetPulseFraction()=0;
	virtual VOID			Disable()=0;
    virtual VOID            SetActive()=0;
    virtual RpClump *       GetRwObject()=0;
};

#endif