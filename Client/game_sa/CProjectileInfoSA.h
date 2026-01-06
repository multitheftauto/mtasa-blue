/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CProjectileInfoSA.h
 *  PURPOSE:     Header file for projectile type information class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CProjectileInfo.h>
#include "CProjectileSA.h"

#define PROJECTILE_COUNT                    32
#define PROJECTILE_INFO_COUNT               32

#define FUNC_RemoveAllProjectiles           0x7399B0 //##SA##
#define FUNC_RemoveProjectile               0x7388F0 //##SA##
#define FUNC_RemoveIfThisIsAProjectile      0x739A40 //##SA##
#define FUNC_AddProjectile                  0x737C80 //##SA##

#define ARRAY_CProjectile                   0xC89110 //##SA##
#define ARRAY_CProjectileInfo               0xC891A8 //##SA##

static constexpr std::size_t MAX_PROJECTILES_COUNT = 32;

class CProjectileInfoSA final : public CProjectileInfo
{
private:
    // CProjectileInfoSAInterface
    eWeaponType         m_weaponType;
    CEntitySAInterface* m_creator;
    CEntitySAInterface* m_target;
    std::uint32_t       m_counter; // time to destroy
    bool                m_isActive;
    std::uint8_t        m_field_0x11[3]; // pad
    CVector             m_lastPos;
    void*               m_particle; // FxSystem_cSAInterface*
    //

    CProjectileSA* m_projectile;

    static ProjectileHandler               m_projectileCreationHandler;
    static GameProjectileDestructHandler*   m_projectileDestructionHandler;

    static std::array<CProjectileInfoSA, MAX_PROJECTILES_COUNT> ms_projectileInfo; // gaProjectileInfo

public:
    CProjectileInfo* AddProjectile(CEntity* creator, eWeaponType projectileType, CVector pos, float force, CVector* target, CEntity* targetEntity) const override;
    void             RemoveProjectile(CProjectileInfo* info, CProjectile* object, bool blow = true) const override;

    CEntity* GetTarget();
    void     SetTarget(CEntity* pEntity);

    bool IsActive();

    void  SetCounter(DWORD dwCounter);
    DWORD GetCounter();

    eWeaponType GetType() const override { return m_weaponType; }

    CProjectile* GetProjectileObject() override { return m_projectile; }
    CProjectileSA* GetProjectile() { return m_projectile; }

    CProjectileInfoSA* GetProjectileInfo(std::uint32_t index) const noexcept override { return index >= MAX_PROJECTILES_COUNT ? nullptr : &ms_projectileInfo[index]; }

    void SetProjectileCreationHandler(ProjectileHandler handler) override { m_projectileCreationHandler = handler; }
    void SetGameProjectileDestructHandler(GameProjectileDestructHandler* handler) override { m_projectileDestructionHandler = handler; }

    void RemoveEntityReferences(CEntity* entity) override;

    static void StaticSetHooks();

private:
    static void Initialise();
    static void Shutdown();

    static CObjectSAInterface*     CreateProjectileObject(std::uint32_t modelIndex, CProjectileInfoSA* info);
    static void                    DestroyProjectileObject(CObjectSAInterface* object);

    static CProjectileInfo* StaticAddProjectile(CEntitySAInterface* creator, eWeaponType projectileType, CVector pos, float force, CVector* target, CEntitySAInterface* targetEntity);
    static void             StaticRemoveProjectile(CProjectileInfoSA* info, CProjectileSA* projectile, bool blow = true);

    static void RemoveNotAdd(CEntitySAInterface* entity, eWeaponType weaponType, CVector pos);

    static void RemoveDetonatorProjectiles();

    static void RemoveIfThisIsAProjectile(CObjectSAInterface* object);
    static void RemoveAllProjectiles();
    static void RemoveParticle(CProjectileInfoSA* info, bool instantly);
    static bool IsProjectileInRange(float x1, float x2, float y1, float y2, float z1, float z2, bool destroy);
    static void Update();

    static int FindFreeIndex();
    static void InitCollision(CObjectSAInterface* projectile);
    static void UpdateLastPos(CProjectileInfoSA* info);
    static bool CheckIsLineOfSightClear(CProjectileInfoSA* info);
};
