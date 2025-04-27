/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/C2DEffectsSA.h
 *  PURPOSE:     Header file for 2dfx effects handler class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once
#include <game/C2DEffects.h>
#include "C2DEffectSAInterface.h"

#define ARRAY_CEscalators  0xC6E9A8
#define NUM_MAX_ESCALATORS 32

static auto* aEscalators = reinterpret_cast<CEscalatorSAInterface*>(ARRAY_CEscalators);

class C2DEffectsSA : public C2DEffects
{
public:
    void InitEffect(C2DEffectSAInterface* effect, std::uint32_t model, CEntitySAInterface* entity = nullptr,
                    std::vector<CEntitySAInterface*>* affectedEntities = nullptr) override;
    void DeInitEffect(C2DEffectSAInterface* effect, std::uint32_t model, bool isCustomEffect = false, CEntitySAInterface* entity = nullptr) override;
    void ReInitEffect(C2DEffectSAInterface* effect, std::uint32_t model, bool isCustomEffect = false, CEntitySAInterface* entity = nullptr) override;
    void ReInitEffect(std::uint32_t model, std::uint32_t index) override;

    void    Set2DFXEffectPosition(std::uint32_t model, std::uint32_t index, const CVector& position) override;
    CVector Get2DFXEffectPosition(std::uint32_t model, std::uint32_t index) const override;

    e2dEffectType Get2DFXEffectType(C2DEffectSAInterface* effect) const override { return effect ? effect->type : e2dEffectType::NONE; }
    e2dEffectType Get2DFXEffectType(std::uint32_t model, std::uint32_t index) const override { return Get2DFXEffectType(GetEffect(model, index)); };
    S2DEffectData Get2DFXPropertiesData(std::uint32_t model, std::uint32_t index) const override;

    void Set2DFXProperties(C2DEffectSAInterface* effectInterface, const S2DEffectData& effectProperties) override;

    void Set2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, float value) override;
    void Set2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, bool value) override;
    void Set2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, std::string& value) override;
    void Set2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, e2dCoronaFlashType& value) override;
    void Set2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, const std::vector<float>& value) override;
    void Reset2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, const S2DEffectData& originalProperties) override;

    void Get2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, float& outValue) const override;
    void Get2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, bool& outValue) const override;
    void Get2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, std::string& outValue) const override;
    void Get2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, e2dCoronaFlashType& outValue) const override;
    void Get2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, SColor& outValue) const override;
    void Get2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, std::vector<float>& outValue) const override;

    // Roadsign functions
    void    SetRoadsignTextColor(C2DEffectSAInterface* effect, const RwColor& color);
    RwColor GetRoadsignTextColor(C2DEffectSAInterface* effect) const;

    void SetRoadsignText(C2DEffectSAInterface* effect, const std::string& text, std::uint8_t line);

    void UpdatePropertiesData(S2DEffectData& dst, const S2DEffectData& src, const e2dEffectProperty& property) const override;

private:
    C2DEffectSAInterface* GetEffect(std::uint32_t model, std::uint32_t index) const;

    // Roadsign functions
    RpAtomic* Roadsign_CreateAtomic(const CVector& position, const RwV3d& rotation, float sizeX, float sizeY, std::uint32_t numLines, char* line1, char* line2,
                                    char* line3, char* line4, std::uint32_t numLetters, std::uint8_t palleteID);
    std::uint32_t Roadsign_GetPalleteIDFromFlags(std::uint8_t flags) const noexcept;
    std::uint32_t Roadsign_GetNumLettersFromFlags(std::uint8_t flags) const noexcept;
    std::uint32_t Roadsign_GetNumLinesFromFlags(std::uint8_t flags) const noexcept;

    bool IsPropertyValueNumber(const e2dEffectProperty& property) const noexcept;
    bool IsPropertyValueString(const e2dEffectProperty& property) const noexcept;
    bool IsPropertyValueTable(const e2dEffectProperty& property) const noexcept;
};
