/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRopesSA.h
*  PURPOSE:     Header file for rope entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRopesSA_H
#define __CRopesSA_H

#include <game/CRopes.h>

#define ROPES_COUNT	8

#define FUNC_CRopes_CreateRopeForSwatPed    0x558d10

#define ARRAY_CRopes	0xB768B8

class CRopesSAInterface
{
public:
	CVector m_vecSegments[32];
	CVector m_vecSegmentsReleased[32];
	CEntitySAInterface * m_pRopeEntity;
	float m_pad4;
	float m_fMass;
	float m_fSegmentLength;
	CEntitySAInterface * m_pRopeHolder;
	CEntitySAInterface * m_pRopeAttacherObject;
	CEntitySAInterface * m_pAttachedEntity;
	float m_pad5;
	uint32 m_uiHoldEntityExpireTime;
	uint8 m_ucSegmentCount;
	uint8 m_ucRopeType;
	uint8 m_ucFlags1;
	uint8 m_ucFlags2;
};
C_ASSERT(sizeof(CRopesSAInterface) == 0x328);

class CRopesSA : public CRopes
{
public:
    int     CreateRopeForSwatPed        ( const CVector & vecPosition, DWORD dwDuration = 4000 );
};

#endif