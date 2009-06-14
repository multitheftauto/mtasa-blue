/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CCam.h
*  PURPOSE:		Camera entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_CAM
#define __CGAME_CAM

#include "CCam.h"
#include <CVector.h>

class CCam
{
public:
	virtual CVector				* GetFront()=0;
	virtual CVector				* GetUp()=0;
	virtual CVector				* GetSource()=0;
	virtual CVector				* GetFixedModeSource()=0;
	virtual CVector				* GetFixedModeVector()=0;
    virtual unsigned int        GetMode()=0;
    virtual CEntity *           GetTargetEntity()=0;
    virtual void                AdjustToNewGravity ( const CVector* pvecOldGravity, const CVector* pvecNewGravity ) = 0;
};

#endif