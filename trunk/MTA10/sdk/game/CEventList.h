/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CEventList.h
*  PURPOSE:		Event list interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_EVENTLIST
#define __CGAME_EVENTLIST

#include "Common.h"
#include "CEvent.h"
#include <CVector.h>

class CEventList
{
public:
    virtual CEvent			* FindClosestEvent ( eEventType eventtype, CVector * vecPosition ) = 0;
	virtual CEvent			* GetEvent ( DWORD ID ) = 0;
	virtual CEvent			* GetEvent ( eEventType eventtype ) = 0;
};

#endif