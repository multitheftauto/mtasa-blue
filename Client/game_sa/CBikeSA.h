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

#define FUNC_CBike_PlaceOnRoadProperly              0x6BEEB0

namespace eBikeNode
{
    enum
    {
        NONE = 0,
        CHASSIS = 1,
        FORKS_FRONT = 2,
        FORKS_REAR = 3,
        WHEEL_FRONT = 4,
        WHEEL_REAR = 5,
        MUDGUARD = 6,
        HANDLEBARS = 7,
        MISC_A = 8,
        MISC_B = 9,
        NUM_NODES
    };
};

class CRideAnimDataSAInterface
{
public:
    unsigned int m_nAnimGroup;
    float        m_fSteerAngle;
    float        m_fAnimLean;
    int          dwordC;
    float        dword10;
    float        m_fHandlebarsAngle;
    float        m_fAnimPercentageState;
};
static_assert(sizeof(CRideAnimDataSAInterface) == 0x1C, "Invalid size for CRideAnimDataSAInterface");

class CBikeSAInterface : public CVehicleSAInterface
{
    RwFrame*                 m_aBikeNodes[eBikeNode::NUM_NODES];
    bool                     m_bLeanMatrixCalculated;
    char                     _pad0[3];
    char                     m_mLeanMatrix[0x48];            // Change the type to CMatrixSAInterface after merging #1673
    unsigned char            m_nDamageFlags;
    char                     field_615[27];
    CVector                  field_630;
    void*                    m_pBikeHandlingData;
    CRideAnimDataSAInterface m_rideAnimData;
    unsigned char            m_anWheelDamageState[2];
    char                     field_65E;
    char                     field_65F;
    CColPointSAInterface     m_anWheelColPoint[4];
    float                    m_wheelsDistancesToGround1[4];
    float                    field_720[4];
    float                    field_730[4];
    float                    field_740;
    int                      m_anWheelSurfaceType[2];
    char                     field_74C[2];
    char                     field_74E[2];
    float                    m_afWheelRotationX[2];
    float                    m_fWheelSpeed[2];
    float                    field_760;
    float                    field_764;
    float                    field_768;
    float                    field_76C;
    float                    field_770[4];
    float                    field_780[4];
    float                    m_fHeightAboveRoad;
    float                    m_fCarTraction;
    float                    field_798;
    float                    field_79C;
    float                    field_7A0;
    float                    field_7A4;
    short                    field_7A8;
    char                     field_7AA[2];
    int                      field_7AC;
    int                      field_7B0;
    bool                     m_bPedLeftHandFixed;
    bool                     m_bPedRightHandFixed;
    char                     field_7B6[2];
    int                      field_7B8;
    int                      field_7BC;
    CEntity*                 m_apWheelCollisionEntity[4];
    CVector                  m_avTouchPointsLocalSpace[4];
    CEntity*                 m_pDamager;
    unsigned char            m_nNumContactWheels;
    unsigned char            m_nNumWheelsOnGround;
    char                     field_806;
    char                     field_807;
    int                      field_808;
    unsigned int             m_anWheelState[2];

    void CalculateLeanMatrix();
};
static_assert(sizeof(CBikeSAInterface) == 0x814, "Invalid size for CBikeSAInterface");

class CBikeSA : public virtual CBike, public virtual CVehicleSA
{
public:
    CBikeSA(){};

    CBikeSA(CBikeSAInterface* bike);
    CBikeSA(eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2);

    // void                    PlaceOnRoadProperly ( void );
};
