/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CCheckpoint.h
*  PURPOSE:		Checkpoint entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_CHECKPOINT
#define __CGAME_CHECKPOINT

#include <CVector.h>
#include "Common.h"

/**
 * \todo fix SetColor/GetColor, the format is actually BGRA (strange)
 */

enum
{
	CHECKPOINT_TUBE,		// a long tube with arrow as direction
	CHECKPOINT_ENDTUBE,		// finish flag long tube checkpoint
	CHECKPOINT_EMPTYTUBE,	// just a long tube checkpoint
	CHECKPOINT_TORUS,
	CHECKPOINT_TORUS_NOFADE,
	CHECKPOINT_TORUSROT,
	CHECKPOINT_TORUSTHROUGH,
	CHECKPOINT_TORUS_UPDOWN,
	CHECKPOINT_TORUS_DOWN,
	CHECKPOINT_NUM
};

class CCheckpoint
{
public:
	virtual VOID			SetPosition(CVector * vecPosition)=0;
	virtual CVector		* GetPosition()=0;
	virtual VOID			SetPointDirection(CVector * vecPointDirection)=0;
	virtual CVector		* GetPointDirection()=0;
	virtual DWORD			GetType()=0; // need enum?
	virtual BOOL			IsActive()=0;
	virtual DWORD			GetIdentifier()=0;
	virtual SColor			GetColor()=0;
	virtual VOID			SetColor(const SColor color)=0;
	virtual VOID			SetPulsePeriod(WORD wPulsePeriod)=0;
	virtual VOID			SetRotateRate(short RotateRate)=0;
	virtual FLOAT			GetSize()=0;
	virtual VOID			SetSize(FLOAT fSize)=0;
	virtual VOID			SetCameraRange(FLOAT fCameraRange)=0;
	virtual FLOAT			GetPulseFraction()=0;
	virtual VOID			Remove()=0;
};

#endif