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

class CBikeSAInterface : public CVehicleSAInterface
{
public:
    DOUBLE                      m_apModelNodes[10];
    unsigned char               bLeanMatrixCalculated;
    char                        pad0[3];
    CMatrix                     mLeanMatrix;
    unsigned char               cDamageFlags;
    char                        pad1[27];
    CVector                     unknowVector;
    tBikeHandlingDataSA*        pBikeHandlingData;
    //00000640 m_rideAnimData  CRideAnimData ?
    //0000065C m_anWheelDamageState db 2 dup(?)
    //0000065E field_65E       db ?
    //0000065F field_65F       db ?
    //00000660 m_anWheelColPoint CColPoint 4 dup(?)
    //00000710 m_wheelsDistancesToGround1 dd 4 dup(?)
    //00000720 field_720       dd 4 dup(?)
    //00000730 field_730       dd 4 dup(?)
    //00000740 field_740       dd ?
    //00000744 m_anWheelSurfaceType dd 2 dup(?)
    //0000074C field_74C       db 2 dup(?)
    //0000074E field_74E       db 2 dup(?)
    //00000750 m_afWheelRotationX dd 2 dup(?)
    //00000758 field_758       dd 2 dup(?)
    //00000760 field_760       dd ?
    //00000764 field_764       dd ?
    //00000768 field_768       dd ?
    //0000076C field_76C       dd ?
    //00000770 field_770       dd 4 dup(?)
    //00000780 field_780       dd 4 dup(?)
    //00000790 m_fHeightAboveRoad dd ?
    //00000794 m_fCarTraction  dd ?
    //00000798 field_798       dd ?
    //0000079C field_79C       dd ?
    //000007A0 field_7A0       dd ?
    //000007A4 field_7A4       dd ?
    //000007A8 field_7A8       dw ?
    //000007AA field_7AA       db 2 dup(?)
    //000007AC field_7AC       dd ?
    //000007B0 field_7B0       dd ?
    //000007B4 m_bPedLeftHandFixed db ?
    //000007B5 m_bPedRightHandFixed db ?
    //000007B6 field_7B6       db 2 dup(?)
    //000007B8 field_7B8       dd ?
    //000007BC field_7BC       dd ?
    //000007C0 m_apWheelCollisionEntity dd 4 dup(?)
    //000007D0 m_avTouchPointsLocalSpace CVector 4 dup(?)
    //00000800 damagedEntity   dd ?                    ;
    //00000804 m_nNumContactWheels db ?
    //00000805 m_nNumWheelsOnGround db ?
    //00000806 field_806 db ?
    //00000807 field_807 db ?
    //00000808 field_808 dd
    //0000080C m_anWheelState dd 2 dup(?);
};
// Size = 0x814

class CBikeSA : public virtual CBike, public virtual CVehicleSA
{
private:
    CBikeHandlingEntrySA* m_pBikeHandlingData;
public:
    CBikeSA(){};
    CBikeSA(CBikeSAInterface* bike);
    CBikeSA(eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2);

    CBikeSAInterface*   GetBikeInterface() { return (CBikeSAInterface*)m_pInterface; };

    CBikeHandlingEntry* GetBikeHandlingData();
    void                SetBikeHandlingData(CBikeHandlingEntry* pHandling);

    void                RecalculateBikeHandling();
};
