/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/C2DEffectSA.h
 *  PURPOSE:     Header file for 2dfx class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "game/C2DEffect.h"
#include "C2DEffectSAInterface.h"
#include <variant>

#define ARRAY_2DFXInfoStore 0xB4C2D8 // C2dfxInfoStore d2fxModels

#define FUNC_C2DEffect_Shutdown 0x4C57D0
#define FUNC_PushCurrentTxd     0x7316A0
#define FUNC_FindTxdSlot        0x731850
#define FUNC_SetCurrentTxd      0x7319C0
#define FUNC_PopCurrentTxd      0x7316B0

// Escalators stuff
#define ARRAY_CEscalators         0xC6E9A8
#define NUM_MAX_ESCALATORS        32
#define FUNC_CEscalator_SwitchOff 0x717860

// fx stuff
#define FUNC_Fx_c_DestroyEntityFx 0x4A1280
#define FUNC_Fx_c_CreateEntityFx  0x4A11E0
#define VAR_G_Fx                  0xA9AE00
#define OFFSET_FxSystem_Entities  0xC
#define OFFSET_FxSystem_Link_Prev 0x4

class C2DEffectSA : public C2DEffect
{
public:
    C2DEffectSA(C2DEffectSAInterface* effectInterface, std::uint32_t modelID);
    ~C2DEffectSA() = default;

    C2DEffectSAInterface* GetInterface() noexcept { return m_effectInterface; }
    e2dEffectType GetEffectType() override { return m_effectInterface ? m_effectInterface->type : e2dEffectType::NONE; }

    bool IsValidLight() const noexcept { return m_effectInterface && m_effectInterface->type == e2dEffectType::LIGHT; };
    bool IsValidRoadsign() const noexcept { return m_effectInterface && m_effectInterface->type == e2dEffectType::ROADSIGN; }
    bool IsValidEscalator() const noexcept { return m_effectInterface && m_effectInterface->type == e2dEffectType::ESCALATOR; }
    bool IsValidParticle() const noexcept { return m_effectInterface && m_effectInterface->type == e2dEffectType::PARTICLE; }

    void Destroy() const;

    void SetPosition(const CVector& position) override;
    CVector& GetPosition() const override;

    // Light properties
    // Set
    void SetCoronaFarClip(float clip) override;
    void SetCoronaPointLightRange(float range) override;
    void SetCoronaSize(float size) override;
    void SetShadowSize(float size) override;
    void SetShadowMultiplier(std::uint8_t multiplier) override;
    void SetCoronaShowMode(e2dCoronaFlashType showMode) override;
    void SetCoronaReflectionsEnabled(bool enable) override;
    void SetCoronaFlareType(std::uint8_t flareType) override;
    void SetLightFlags(std::uint16_t flags) override;
    void SetShadowDistance(std::int8_t distance) override;
    void SetCoronaOffsets(const CVector& offsets) override;
    void SetCoronaColor(const RwColor& color) override;
    void SetCoronaTexture(const std::string& name) override;
    void SetShadowTexture(const std::string& name) override;

    // Get
    float GetCoronaFarClip() const override { return IsValidLight() ? m_effectInterface->effect.light.coronaFarClip : 0.0f; }
    float GetCoronaPointLightRange() const override { return IsValidLight() ? m_effectInterface->effect.light.pointLightRange : 0.0f; }
    float GetCoronaSize() const override { return IsValidLight() ? m_effectInterface->effect.light.coronaSize : 0.0f; }
    float GetShadowSize() const override { return IsValidLight() ? m_effectInterface->effect.light.shadowSize : 0.0f; }
    std::uint8_t GetShadowMultiplier() const override { return IsValidLight() ? m_effectInterface->effect.light.shadowColorMultiplier : 0; }
    e2dCoronaFlashType GetCoronaShowMode() const override { return IsValidLight() ? m_effectInterface->effect.light.coronaFlashType : e2dCoronaFlashType::UNUSED; }
    bool GetCoronaReflectionsEnabled() const override { return IsValidLight() ? m_effectInterface->effect.light.coronaEnableReflection : false; }
    std::uint8_t       GetCoronaFlareType() const override { return IsValidLight() ? m_effectInterface->effect.light.coronaFlareType : 0; }
    std::uint16_t      GetLightFlags() const override { return IsValidLight() ? m_effectInterface->effect.light.flags : 0; }
    std::int8_t        GetShadowDistance() const override { return IsValidLight() ? m_effectInterface->effect.light.shadowZDistance : 0; }
    CVector             GetCoronaOffsets() const override;
    RwColor            GetCoronaColor() const override { return IsValidLight() ? m_effectInterface->effect.light.color : RwColor{0,0,0,0}; }
    std::string        GetCoronaTexture() const override { return IsValidLight() ? (m_effectInterface->effect.light.coronaTex ? m_effectInterface->effect.light.coronaTex->name : "") : ""; }
    std::string GetShadowTexture() const override { return IsValidLight() ? (m_effectInterface->effect.light.shadowTex ? m_effectInterface->effect.light.shadowTex->name : "") : ""; }

    // Particle properties
    // Set
    void SetParticleName(const std::string& name) override;

    // Get
    std::string GetParticleName() const override { return IsValidParticle() ? (m_effectInterface->effect.particle.szName ? m_effectInterface->effect.particle.szName : "") : ""; }

    // Roadsign properties
    // Set
    void SetRoadsignSize(const RwV2d& size) override;
    void SetRoadsignRotation(const RwV3d& rotation) override;
    void SetRoadsignFlags(std::uint8_t flags) override;
    void SetRoadsignText(const std::string& text, std::uint8_t line) override;

    // Get
    RwV2d&        GetRoadsignSize() override;
    RwV3d&        GetRoadsignRotation() override;
    std::uint16_t GetRoadsignFlags() const override { return IsValidRoadsign() ? m_effectInterface->effect.roadsign.flags : 0; }
    std::string   GetRoadsignText() const override;

    // Escalator properties
    // Set
    void SetEscalatorBottom(const RwV3d& bottom) override;
    void SetEscalatorTop(const RwV3d& top) override;
    void SetEscalatorEnd(const RwV3d& end) override;
    void SetEscalatorDirection(std::uint8_t direction) override;

    // Get
    RwV3d&       GetEscalatorBottom() override;
    RwV3d&       GetEscalatorTop() override;
    RwV3d&       GetEscalatorEnd() override;
    std::uint8_t GetEscalatorDirection() const override { return IsValidEscalator() ? m_effectInterface->effect.escalator.direction : 0; }

    static RpAtomic*     Roadsign_CreateAtomic(const RwV3d& position, const RwV3d& rotation, float sizeX, float sizeY, std::uint32_t numLines, char* line1, char* line2, char* line3, char* line4, std::uint32_t numLetters, std::uint8_t palleteID);
    static std::uint32_t Roadsign_GetPalleteIDFromFlags(std::uint8_t flags);
    static std::uint32_t Roadsign_GetNumLettersFromFlags(std::uint8_t flags);
    static std::uint32_t Roadsign_GetNumLinesFromFlags(std::uint8_t flags);
    static void Roadsign_DestroyAtomic(C2DEffectSAInterface* effect);

    static C2DEffectSAInterface* CreateCopy(C2DEffectSAInterface* effect);

    static void Shutdown(C2DEffectSAInterface* effect);
    static void SafeDelete2DFXEffect(C2DEffectSAInterface* effect);
    static void PrepareTexturesForLightEffect(RwTexture*& coronaTex, RwTexture*& shadowTex, const char* coronaName, const char* shadowName, bool removeIfExist);

public:
    static int effect2dPluginOffset;

private:
    C2DEffectSAInterface* m_effectInterface;
    std::uint32_t         m_model;

};
