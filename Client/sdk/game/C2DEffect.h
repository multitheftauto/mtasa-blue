/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/C2DEffect.h
 *  PURPOSE:     Game 2dfx interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *
 *****************************************************************************/

#pragma once
#include "C2DEffects.h"

struct RwColor;
struct RwV2d;
struct RwV3d;
class CVector;

class C2DEffect
{
public:
    virtual void SetPosition(const CVector& position) = 0;
    virtual CVector& GetPosition() const = 0;

    virtual e2dEffectType GetEffectType() = 0;

    // Light properties
    // Set
    virtual void SetCoronaFarClip(float clip) = 0;
    virtual void SetCoronaPointLightRange(float range) = 0;
    virtual void SetCoronaSize(float size) = 0;
    virtual void SetShadowSize(float size) = 0;
    virtual void SetShadowMultiplier(std::uint8_t multiplier) = 0;
    virtual void SetCoronaShowMode(e2dCoronaFlashType showMode) = 0;
    virtual void SetCoronaReflectionsEnabled(bool enable) = 0;
    virtual void SetCoronaFlareType(std::uint8_t flareType) = 0;
    virtual void SetLightFlags(std::uint16_t flags) = 0;
    virtual void SetShadowDistance(std::int8_t distance) = 0;
    virtual void SetCoronaOffsets(const CVector& offsets) = 0;
    virtual void SetCoronaColor(const RwColor& color) = 0;
    virtual void SetCoronaTexture(const std::string& name) = 0;
    virtual void SetShadowTexture(const std::string& name) = 0;

    // Get
    virtual float   GetCoronaFarClip() const = 0;
    virtual float   GetCoronaPointLightRange() const = 0;
    virtual float   GetCoronaSize() const = 0;
    virtual float   GetShadowSize() const = 0;
    virtual std::uint8_t GetShadowMultiplier() const = 0;
    virtual e2dCoronaFlashType GetCoronaShowMode() const = 0;
    virtual bool                GetCoronaReflectionsEnabled() const = 0;
    virtual std::uint8_t        GetCoronaFlareType() const = 0;
    virtual std::uint16_t       GetLightFlags() const = 0;
    virtual std::int8_t         GetShadowDistance() const = 0;
    virtual CVector             GetCoronaOffsets() const = 0;
    virtual RwColor            GetCoronaColor() const = 0;
    virtual std::string        GetCoronaTexture() const = 0;
    virtual std::string        GetShadowTexture() const = 0;

    // Particle properties
    // Set
    virtual void SetParticleName(const std::string& name) = 0;

    // Get
    virtual std::string GetParticleName() const = 0;

    // Roadsign properties
    // Set
    virtual void SetRoadsignSize(const RwV2d& size) = 0;
    virtual void SetRoadsignRotation(const RwV3d& rotation) = 0;
    virtual void SetRoadsignFlags(std::uint8_t flags) = 0;
    virtual void SetRoadsignText(const std::string& text, std::uint8_t line) = 0;

    // Get
    virtual RwV2d& GetRoadsignSize() = 0;
    virtual RwV3d& GetRoadsignRotation() = 0;
    virtual std::uint16_t GetRoadsignFlags() const = 0;
    virtual std::string   GetRoadsignText() const = 0;

    // Escalator properties
    // Set
    virtual void SetEscalatorBottom(const RwV3d& bottom) = 0;
    virtual void SetEscalatorTop(const RwV3d& top) = 0;
    virtual void SetEscalatorEnd(const RwV3d& end) = 0;
    virtual void SetEscalatorDirection(std::uint8_t direction) = 0;

    // Get
    virtual RwV3d& GetEscalatorBottom() = 0;
    virtual RwV3d& GetEscalatorTop() = 0;
    virtual RwV3d& GetEscalatorEnd() = 0;
    virtual std::uint8_t GetEscalatorDirection() const = 0;

};
