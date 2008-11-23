/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CEventListSA.h
*  PURPOSE:		Header file for event list class
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_EVENTLIST
#define __CGAMESA_EVENTLIST

#include <game/CEventList.h>
#include "CEventSA.h"

#define FUNC_ClearEvent				0x489F50
#define FUNC_FindClosestEvent		0x489E70
#define FUNC_GetEvent				0x489F70

#define ARRAY_CEvent				0x7DAAB8

#define MAX_EVENTS					64

class CEventListSA : public CEventList
{
	CEvent			* FindClosestEvent ( eEventType eventtype, CVector * vecPosition );
	CEvent			* GetEvent ( DWORD ID );
	CEvent			* GetEvent ( eEventType eventtype );
};

#endif