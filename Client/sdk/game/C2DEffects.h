/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/C2DEffects.h
 *  PURPOSE:     2DFX effect functions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/ \
#pragma once
#include "Common.h"
#include <CVector.h>
#include <CVector2D.h>
#include <cstdint>
#include <string>

class C2DEffectSAInterface;
class CEntitySAInterface;

struct S2DEffectData
{
    CVector position{};

    // light properties
    float              drawDistance{};
    float              lightRange{};
    float              coronaSize{};
    float              shadowSize{};
    float              shadowMult{};
    float              shadowDist{};
    e2dCoronaFlashType coronaFlashType{e2dCoronaFlashType::DEFAULT};
    bool               coronaReflection{false};
    float              coronaFlareType{};

    struct
    {
        std::int8_t x{};
        std::int8_t y{};
        std::int8_t z{};
    } offset;

    std::string coronaName{};
    std::string shadowName{};

    // light & roadsign
    std::uint16_t flags{};
    SColor        color{};

    // particle
    std::string prt_name{};

    // roadsign
    CVector2D   size{};
    CVector     rot{};
    std::string text_1{};
    std::string text_2{};
    std::string text_3{};
    std::string text_4{};

    // escalator
    CVector bottom{};
    CVector top{};
    CVector end{};

    std::uint8_t direction{};

    bool operator==(const S2DEffectData& other) const
    {
        return position == other.position && drawDistance == other.drawDistance && lightRange == other.lightRange && coronaSize == other.coronaSize &&
               shadowSize == other.shadowSize && shadowMult == other.shadowMult && shadowDist == other.shadowDist && coronaFlashType == other.coronaFlashType &&
               coronaReflection == other.coronaReflection && coronaFlareType == other.coronaFlareType && offset.x == other.offset.x &&
               offset.y == other.offset.y && offset.z == other.offset.z && coronaName == other.coronaName && shadowName == other.shadowName &&
               flags == other.flags && color == other.color && prt_name == other.prt_name && size == other.size && rot == other.rot && text_1 == other.text_1 &&
               text_2 == other.text_2 && text_3 == other.text_3 && text_4 == other.text_4 && bottom == other.bottom && top == other.top && end == other.end &&
               direction == other.direction;
    }
};

class C2DEffects
{
public:
    virtual void InitEffect(C2DEffectSAInterface* effect, std::uint32_t model, CEntitySAInterface* entity = nullptr,
                            std::vector<CEntitySAInterface*>* affectedEntities = nullptr) = 0;
    virtual void DeInitEffect(C2DEffectSAInterface* effect, std::uint32_t model, bool isCustomEffect = false, CEntitySAInterface* entity = nullptr) = 0;
    virtual void ReInitEffect(C2DEffectSAInterface* effect, std::uint32_t model, bool isCustomEffect = false, CEntitySAInterface* entity = nullptr) = 0;
    virtual void ReInitEffect(std::uint32_t model, std::uint32_t index) = 0;

    virtual void    Set2DFXEffectPosition(std::uint32_t model, std::uint32_t index, const CVector& position) = 0;
    virtual CVector Get2DFXEffectPosition(std::uint32_t model, std::uint32_t index) const = 0;

    virtual e2dEffectType Get2DFXEffectType(C2DEffectSAInterface* effect) const = 0;
    virtual e2dEffectType Get2DFXEffectType(std::uint32_t model, std::uint32_t index) const = 0;

    virtual void Set2DFXProperties(C2DEffectSAInterface* effectInterface, const S2DEffectData& effectProperties) = 0;

    virtual void Set2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, float value) = 0;
    virtual void Set2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, bool value) = 0;
    virtual void Set2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, std::string& value) = 0;
    virtual void Set2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, e2dCoronaFlashType& value) = 0;
    virtual void Set2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, const std::vector<float>& value) = 0;
    virtual void Reset2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, const S2DEffectData& originalProperties) = 0;

    virtual S2DEffectData Get2DFXPropertiesData(std::uint32_t model, std::uint32_t index) const = 0;

    virtual void Get2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, float& outValue) const = 0;
    virtual void Get2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, bool& outValue) const = 0;
    virtual void Get2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, std::string& outValue) const = 0;
    virtual void Get2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, e2dCoronaFlashType& outValue) const = 0;
    virtual void Get2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, SColor& outValue) const = 0;
    virtual void Get2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, std::vector<float>& outValue) const = 0;

    virtual void UpdatePropertiesData(S2DEffectData& dst, const S2DEffectData& src, const e2dEffectProperty& property) const = 0;

    virtual bool IsPropertyValueNumber(const e2dEffectProperty& property) const noexcept = 0;
    virtual bool IsPropertyValueString(const e2dEffectProperty& property) const noexcept = 0;
    virtual bool IsPropertyValueTable(const e2dEffectProperty& property) const noexcept = 0;
};
