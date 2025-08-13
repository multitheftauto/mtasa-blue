/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPlayerPedSA.h
 *  PURPOSE:     Header file for player ped entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPlayerPed.h>
#include "CPedSA.h"
#include "CWantedSA.h"

#define FUNC_SetInitialState            0x60CD20
#define FUNC_ClearWeaponTarget          0x533B30

#define FUNC_CPedOperatorNew            0x5E4720 // ##SA##
#define FUNC_CPlayerPedConstructor      0x60D5B0 // ##SA##

#define FUNC_CPlayerPedDestructor       0x6093B0 // ##SA##
#define FUNC_CPlayerPedOperatorDelete   0x5E4760 // ##SA##

#define FUNC_CPlayerPed_ReApplyMoveAnims    0x609650

#define SIZEOF_CPLAYERPED               1956

class CPlayerPedSAInterface : public CPedSAInterface
{
public:
    CEntitySAInterface* mouseTargetEntity;
    std::uint8_t        field_7A0[4];
};
static_assert(sizeof(CPlayerPedSAInterface) == 0x7A4, "Invalid size for CPlayerPedSAInterface");

class CPlayerPedSA : public virtual CPlayerPed, public virtual CPedSA
{
private:
    bool                             m_bIsLocal;
    class CPlayerPedDataSAInterface* m_pData;
    CWantedSA*                       m_pWanted;

public:
    CPlayerPedSA(unsigned int nModelIndex);
    CPlayerPedSA(CPlayerPedSAInterface* ped);
    ~CPlayerPedSA();

    CWanted* GetWanted();

    void SetInitialState();

    eMoveAnim GetMoveAnim();
    void      SetMoveAnim(eMoveAnim iAnimGroup);

    CEntity* GetTargetedEntity() const override;
    void SetTargetedEntity(CEntity* targetEntity) override;

    CPlayerPedSAInterface* GetPlayerPedInterface() const noexcept { return static_cast<CPlayerPedSAInterface*>(m_pInterface); };

    static void StaticSetHooks();
};
