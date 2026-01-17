/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include "TaskSA.h"
#include <game/TaskSimpleSwim.h>
#include <enums/SurfaceType.h>
#include <game/CTasks.h>

class CPedSAInterface;
class CEntitySAInterface;

class CTaskSimpleSwimSAInterface : public CTaskSimpleSAInterface
{
public:
    bool                m_finishedBlending;
    bool                m_animBlockRefAdded;
    swimState           m_swimState;
    int                 m_animID;
    float               m_animSpeed;
    CVector             m_pos;
    CPedSAInterface*    m_ped;
    float               m_rotationX;
    float               m_turningRotationY;
    float               m_upperTorsoRotationX;
    float               m_aimingRotation;
    float               m_stateChanger;
    CEntitySAInterface* m_entity;
    CVector             m_climbPos;
    float               m_angle;
    SurfaceTypes::Enum  m_surfaceType;
    std::uint8_t        m_field4D[3];
    float               m_randomMoveBlendRatio;
    float               m_swimStopTime;
    std::uint32_t       m_timeStep;
    void*               m_fxSystem;            // FxSystem_cSAInterface*
    bool                m_triggerWaterSplash;
    std::uint8_t        m_field61[3];
};

class CTaskSimpleSwimSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleSwim
{
public:
    CTaskSimpleSwimSA() {};
    CTaskSimpleSwimSA(CPed* ped, CVector* pos);

    const CTaskSimpleSwimSAInterface* GetTaskInterface() const { return static_cast<const CTaskSimpleSwimSAInterface*>(GetInterface()); }
    CTaskSimpleSwimSAInterface* GetTaskInterface() { return static_cast<CTaskSimpleSwimSAInterface*>(GetInterface()); }

    swimState GetSwimState() const override { return GetTaskInterface()->m_swimState; }
};
