/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRopesSA.h
 *  PURPOSE:     Header file for rope entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CRopes.h>

#define ROPES_COUNT    8

#define FUNC_CRopes_CreateRopeForSwatPed    0x558d10

enum class eRopeType : uint8
{
    NONE = 0,
    CRANE_MAGNET1 = 1,
    CRANE_HARNESS = 2,
    MAGNET = 3,
    CRANE_MAGNO = 4,            // see ModelIndices::MI_MAGNOCRANE
    WRECKING_BALL = 5,
    QUARRY_CRANE_ARM = 6,
    CRANE_TROLLEY = 7,
    SWAT = 8
};

class CRopesSAInterface
{
public:
    CVector             m_vecSegments[32];
    CVector             m_vecSegmentsReleased[32];
    CEntitySAInterface* m_pRopeEntity;
    float               m_fGroundZ;
    float               m_fMass;
    float               m_fTotalLength;
    CEntitySAInterface* m_pRopeHolder;
    CEntitySAInterface* m_pRopeAttacherObject;
    CEntitySAInterface* m_pAttachedEntity;
    float               m_fSegmentLength;
    uint32              m_uiHoldEntityExpireTime;
    uint8               m_ucSegmentCount;
    eRopeType           m_ucRopeType;
    uint8               m_ucFlags1;
    uint8               m_ucFlags2;
};
static_assert(sizeof(CRopesSAInterface) == 0x328, "Invalid size for CRopesSAInterface");

class CRopesSA : public CRopes
{
public:
    int  CreateRopeForSwatPed(const CVector& vecPosition, DWORD dwDuration = 4000);
    void RemoveEntityRope(CEntitySAInterface* pObject);
    static int32 FindRope(CEntitySAInterface* id);
    static CRopesSAInterface& GetRope(int32 index);

private:
    static CRopesSAInterface (&ms_aRopes)[8];
};
