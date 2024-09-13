/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBikeSA.h
 *  PURPOSE:     Header file for bike vehicle entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CBike.h>
#include "CVehicleSA.h"

enum class eBikeNodes
{
    NONE = 0,
    CHASSIS,
    FORKS_FRONT,
    FORKS_REAR,
    WHEEL_FRONT,
    WHEEL_REAR,
    MUDGUARD,
    HANDLEBARS,
    MISC_A,
    MISC_B,

    NUM_NODES
};

struct sRideAnimData
{
    int32 iAnimGroup;
    float fSteerAngle;
    float fAnimLean;
    int32 iUnknow;
    int32 iUnknowToo;
    float fHandlebarsAngle;
    float fAnimPercentageState;
};
static_assert(sizeof(sRideAnimData) == 0x1C, "Invalid size for sRideAnimData");

class CBikeSAInterface : public CVehicleSAInterface
{
public:
    RwFrame*             m_apModelNodes[static_cast<std::size_t>(eBikeNodes::NUM_NODES)];
    int8                 m_bLeanMatrixCalculated;
    int8                 pad0[3];            // Maybe prev value is int32
    int8                 m_mLeanMatrix[72];
    int8                 m_cDamageFlags;
    int8                 pad1[27];
    CVector              m_vecUnknowVector;
    tBikeHandlingDataSA* m_pBikeHandlingData;
    sRideAnimData        m_sRideData;
    int8                 m_acWheelDamageState[2];
    int8                 field_65E;
    int8                 field_65F;
    int8                 m_anWheelColPoint[176];
    float                m_afWheelDistanceToGround[4];
    int32                field_720[4];
    int32                field_730[4];
    int32                field_740;
    int32                m_aiWheelSurfaceType[2];
    int8                 field_74C[2];
    int8                 field_74E[2];
    float                m_afWheelRotationX[2];
    int32                field_758[2];
    int32                field_760;
    int32                field_764;
    int32                field_768;
    int32                field_76C;
    int32                field_770[4];
    int32                field_780[4];
    float                m_fHeightAboveRoad;
    float                m_fCarTraction;
    int32                field_798;
    int32                field_79C;
    int32                field_7A0;
    int32                field_7A4;
    int16                field_7A8;
    int8                 field_7AA[2];
    int32                field_7AC;
    int32                field_7B0;
    int8                 m_bPedLeftHandFixed;
    int8                 m_bPedRightHandFixed;
    int8                 field_7B6[2];
    int32                field_7B8;
    int32                field_7BC;
    int32                m_apWheelCollisionEntity[4];
    CVector              m_avTouchPointsLocalSpace[4];
    int32                m_pDamagedEntity;
    int8                 m_cNumContactWheels;
    int8                 m_cNumWheelsOnGround;
    int8                 field_806;
    int8                 field_807;
    int32                field_808;
    int32                m_aiWheelState[2];
};
static_assert(sizeof(CBikeSAInterface) == 0x814, "Invalid size for CBikeSAInterface");

class CBikeSA : public virtual CBike, public virtual CVehicleSA
{
private:
    CBikeHandlingEntrySA* m_pBikeHandlingData = nullptr;

public:
    CBikeSA() = default;
    CBikeSA(CBikeSAInterface* pInterface);

    CBikeSAInterface* GetBikeInterface() { return reinterpret_cast<CBikeSAInterface*>(GetInterface()); }

    CBikeHandlingEntry* GetBikeHandlingData();
    void                SetBikeHandlingData(CBikeHandlingEntry* pHandling);

    void RecalculateBikeHandling();
};
