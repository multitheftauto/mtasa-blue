/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CFire.h
 *  PURPOSE:     Fire interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CEntity;
class CVector;
class CFireSAInterface;

class CFire
{
public:
    virtual CFireSAInterface* GetInterface() noexcept = 0;

    virtual void Extinguish() = 0;

    virtual void     SetPosition(const CVector& position, bool updateParticle = false) = 0;
    virtual CVector& GetPosition() noexcept = 0;

    virtual void  SetStrength(float strength, bool updateFX = true) = 0;
    virtual float GetStrength() const noexcept = 0;

    virtual void SetLifetime(std::uint32_t lifetime) noexcept = 0;
    virtual std::uint32_t GetLifetime() const noexcept = 0;

    // Created by lua script - createFire
    virtual void SetCreatedByScript(bool createdByScript) noexcept = 0;
    virtual bool IsCreatedByScript() const noexcept = 0;
};
