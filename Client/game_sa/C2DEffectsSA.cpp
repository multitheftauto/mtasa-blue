/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/C2DEffectsSA.cpp
 *  PURPOSE:     2DFX effects handler class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "C2DEffectsSA.h"
#include "CGameSA.h"
#include "CFxSA.h"
#include "gamesa_renderware.h"

extern CGameSA* pGame;

C2DEffectSAInterface* C2DEffectsSA::GetEffect(std::uint32_t model, std::uint32_t index) const
{
    CModelInfo* modelInfo = pGame->GetModelInfo(model);
    if (!modelInfo)
        return nullptr;

    return modelInfo->Get2DFXEffectByIndex(index);
}

void C2DEffectsSA::InitEffect(C2DEffectSAInterface* effect, std::uint32_t model, CEntitySAInterface* entity, std::vector<CEntitySAInterface*>* affectedEntities)
{
    if (!effect)
        return;

    switch (effect->type)
    {
        case e2dEffectType::PARTICLE:
        {
            if (affectedEntities)
            {
                for (auto& particleEntity : *affectedEntities)
                    g_Fx->CreateEntityFx(particleEntity, effect->effect.particle.szName, &effect->position);
            }
            else if (entity)
                g_Fx->CreateEntityFx(entity, effect->effect.particle.szName, &effect->position);

            break;
        }
        case e2dEffectType::ROADSIGN:
        {
            t2dEffectRoadsign& roadsign = effect->effect.roadsign;

            std::uint32_t numLines = Roadsign_GetNumLinesFromFlags(roadsign.flags);
            std::uint32_t numLetters = Roadsign_GetNumLettersFromFlags(roadsign.flags);
            std::uint8_t  palleteID = Roadsign_GetPalleteIDFromFlags(roadsign.flags);
            roadsign.atomic = Roadsign_CreateAtomic(effect->position, roadsign.rotation, roadsign.size.x, roadsign.size.y, numLines, &roadsign.text[0],
                                                    &roadsign.text[16], &roadsign.text[32], &roadsign.text[48], numLetters, palleteID);

            CModelInfo* modelInfo = pGame->GetModelInfo(model);
            if (modelInfo)
                SetRoadsignTextColor(effect, modelInfo->GetStored2DFXRoadsignColor(modelInfo->Get2DFXEffectIndex(effect)));

            break;
        }
        case e2dEffectType::ESCALATOR:
        {
            if (!entity && affectedEntities)
                entity = affectedEntities->at(0);

            if (entity)
            {
                CVector vecStart{};
                CVector vecBottom{};
                CVector vecTop{};
                CVector vecEnd{};

                entity->TransformFromObjectSpace(&vecStart, &effect->position);
                entity->TransformFromObjectSpace(&vecBottom, reinterpret_cast<CVector*>(&effect->effect.escalator.bottom));
                entity->TransformFromObjectSpace(&vecTop, reinterpret_cast<CVector*>(&effect->effect.escalator.top));
                entity->TransformFromObjectSpace(&vecEnd, reinterpret_cast<CVector*>(&effect->effect.escalator.end));

                for (std::size_t i = 0; i < NUM_MAX_ESCALATORS; i++)
                {
                    if (aEscalators[i].exist)
                        continue;

                    aEscalators[i].AddThisOne(&vecStart, &vecBottom, &vecTop, &vecEnd, effect->effect.escalator.direction == 0, entity);
                    break;
                }
            }

            break;
        }
        default:
            break;
    }
}

void C2DEffectsSA::DeInitEffect(C2DEffectSAInterface* effect, std::uint32_t model, bool isCustomEffect, CEntitySAInterface* entity)
{
    if (!effect)
        return;

    switch (effect->type)
    {
        case e2dEffectType::LIGHT:
        {
            t2dEffectLight& light = effect->effect.light;

            if (light.coronaTex)
            {
                RwTextureDestroy(light.coronaTex);
                light.coronaTex = nullptr;
            }

            if (light.shadowTex)
            {
                RwTextureDestroy(light.shadowTex);
                light.shadowTex = nullptr;
            }
            break;
        }
        case e2dEffectType::PARTICLE:
        {
            if (!entity)
            {
                const auto& entities = CFxSA::GetEntitiesFromFx(model);
                for (auto* fxEntity : entities)
                    g_Fx->DestroyEntityFx(fxEntity);
            }
            else
                g_Fx->DestroyEntityFx(entity);

            break;
        }
        case e2dEffectType::ROADSIGN:
        {
            RpAtomic* atomic = effect->effect.roadsign.atomic;
            if (atomic)
            {
                RwFrame* frame = RpAtomicGetFrame(atomic);
                if (frame)
                {
                    RpAtomicSetFrame(atomic, nullptr);
                    RwFrameDestroy(frame);
                }

                RpAtomicDestroy(atomic);
                effect->effect.roadsign.atomic = nullptr;
            }

            // We only free memory for the custom effect.
            // The original effects are managed by CMemoryMgr in the RW plugin (EffectStreamRead)
            if (isCustomEffect && effect->effect.roadsign.text)
            {
                std::free(effect->effect.roadsign.text);
                effect->effect.roadsign.text = nullptr;
            }

            break;
        }
        case e2dEffectType::ESCALATOR:
        {
            for (std::size_t i = 0; i < NUM_MAX_ESCALATORS; i++)
            {
                if (!aEscalators[i].exist || !aEscalators[i].entity)
                    continue;

                if (entity && aEscalators[i].entity != entity)
                    continue;

                if (!entity && aEscalators[i].entity->m_nModelIndex != model)
                    continue;

                aEscalators[i].SwitchOff();
                aEscalators[i].exist = false;
                break;
            }

            break;
        }
        default:
            break;
    }
}

void C2DEffectsSA::ReInitEffect(C2DEffectSAInterface* effect, std::uint32_t model, bool isCustomEffect, CEntitySAInterface* entity)
{
    // We preserve the entities that have particle/escalator 2dfx effect,
    // so that during initialization we can add it back to the same entities
    std::vector<CEntitySAInterface*> affectedEntities{};
    if (effect->type == e2dEffectType::PARTICLE && !entity)
        affectedEntities = CFxSA::GetEntitiesFromFx(model);
    else if (effect->type == e2dEffectType::ESCALATOR && !entity)
    {
        for (std::size_t i = 0; i < NUM_MAX_ESCALATORS; i++)
        {
            if (aEscalators[i].exist && aEscalators[i].entity && aEscalators[i].entity->m_nModelIndex == model)
            {
                affectedEntities.push_back(aEscalators[i].entity);
                break;
            }
        }
    }

    DeInitEffect(effect, model, isCustomEffect, entity);
    InitEffect(effect, model, entity, affectedEntities.empty() ? nullptr : &affectedEntities);
}

void C2DEffectsSA::ReInitEffect(std::uint32_t model, std::uint32_t index)
{
    C2DEffectSAInterface* effect = GetEffect(model, index);
    if (!effect)
        return;

    ReInitEffect(effect, model, pGame->GetModelInfo(model)->IsCustom2DFXEffect(effect));
}

void C2DEffectsSA::Set2DFXEffectPosition(std::uint32_t model, std::uint32_t index, const CVector& position)
{
    C2DEffectSAInterface* effect = GetEffect(model, index);
    if (!effect)
        return;

    effect->position = position;
    if (effect->type != e2dEffectType::LIGHT)
        ReInitEffect(model, index);
}

CVector C2DEffectsSA::Get2DFXEffectPosition(std::uint32_t model, std::uint32_t index) const
{
    C2DEffectSAInterface* effect = GetEffect(model, index);
    return effect ? effect->position : CVector{};
}

void C2DEffectsSA::Set2DFXProperties(C2DEffectSAInterface* effectInterface, const S2DEffectData& effectProperties)
{
    if (!effectInterface)
        return;

    effectInterface->position = effectProperties.position;

    switch (effectInterface->type)
    {
        case e2dEffectType::LIGHT:
        {
            t2dEffectLight& light = effectInterface->effect.light;
            light.coronaFarClip = effectProperties.drawDistance;
            light.pointLightRange = effectProperties.lightRange;
            light.coronaSize = effectProperties.coronaSize;
            light.shadowSize = effectProperties.shadowSize;
            light.shadowColorMultiplier = effectProperties.shadowMult;
            light.shadowZDistance = effectProperties.shadowDist;
            light.coronaFlashType = effectProperties.coronaFlashType;
            light.coronaEnableReflection = effectProperties.coronaReflection;
            light.coronaFlareType = effectProperties.coronaFlareType;
            light.flags = effectProperties.flags;
            light.offsetX = effectProperties.offset.x;
            light.offsetY = effectProperties.offset.y;
            light.offsetZ = effectProperties.offset.z;
            light.color = RwColor{effectProperties.color.R, effectProperties.color.G, effectProperties.color.B, effectProperties.color.A};

            if (light.coronaTex)
                RwTextureDestroy(light.coronaTex);

            if (light.shadowTex)
                RwTextureDestroy(light.shadowTex);

            CTxdStore_PushCurrentTxd();
            SetTextureDict(CTxdStore_FindTxdSlot("particle"));
            light.coronaTex = RwReadTexture(effectProperties.coronaName.c_str(), nullptr);
            light.shadowTex = RwReadTexture(effectProperties.shadowName.c_str(), nullptr);
            CTxdStore_PopCurrentTxd();
            break;
        }
        case e2dEffectType::PARTICLE:
        {
            t2dEffectParticle& particle = effectInterface->effect.particle;
            std::strncpy(particle.szName, effectProperties.prt_name.c_str(), 24);
            break;
        }
        case e2dEffectType::ROADSIGN:
        {
            t2dEffectRoadsign& roadsign = effectInterface->effect.roadsign;
            roadsign.size = RwV2d{effectProperties.size.fX, effectProperties.size.fY};
            roadsign.rotation = RwV3d{effectProperties.rot.fX, effectProperties.rot.fY, effectProperties.rot.fZ};
            roadsign.flags = static_cast<std::uint8_t>(effectProperties.flags);

            if (!roadsign.text)
                roadsign.text = static_cast<char*>(std::malloc(64));

            if (roadsign.text)
            {
                ZeroMemory(roadsign.text, 64);

                SetRoadsignText(effectInterface, effectProperties.text_1, 1);
                SetRoadsignText(effectInterface, effectProperties.text_2, 2);
                SetRoadsignText(effectInterface, effectProperties.text_3, 3);
                SetRoadsignText(effectInterface, effectProperties.text_4, 4);
            }

            SetRoadsignTextColor(effectInterface,
                                 RwColor{effectProperties.color.R, effectProperties.color.G, effectProperties.color.B, effectProperties.color.A});
            break;
        }
        case e2dEffectType::ESCALATOR:
        {
            t2dEffectEscalator& escalator = effectInterface->effect.escalator;
            escalator.bottom = RwV3d{effectProperties.bottom.fX, effectProperties.bottom.fY, effectProperties.bottom.fZ};
            escalator.top = RwV3d{effectProperties.top.fX, effectProperties.top.fY, effectProperties.top.fZ};
            escalator.end = RwV3d{effectProperties.end.fX, effectProperties.end.fY, effectProperties.end.fZ};
            escalator.direction = effectProperties.direction;
            break;
        }
        default:
            break;
    }
}

void C2DEffectsSA::Set2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, float value)
{
    C2DEffectSAInterface* effect = GetEffect(model, index);
    if (!effect)
        return;

    switch (effect->type)
    {
        case e2dEffectType::LIGHT:
        {
            t2dEffectLight& light = effect->effect.light;

            switch (property)
            {
                case e2dEffectProperty::FAR_CLIP_DISTANCE:
                    light.coronaFarClip = value;
                    break;
                case e2dEffectProperty::LIGHT_RANGE:
                    light.pointLightRange = value;
                    break;
                case e2dEffectProperty::CORONA_SIZE:
                    light.coronaSize = value;
                    break;
                case e2dEffectProperty::SHADOW_SIZE:
                    light.shadowSize = value;
                    break;
                case e2dEffectProperty::SHADOW_MULT:
                    light.shadowColorMultiplier = static_cast<std::uint8_t>(value);
                    break;
                case e2dEffectProperty::FLARE_TYPE:
                    light.coronaFlareType = static_cast<std::uint8_t>(value);
                    break;
                case e2dEffectProperty::SHADOW_DISTANCE:
                    light.shadowZDistance = static_cast<std::int8_t>(value);
                    break;
                case e2dEffectProperty::FLAGS:
                    light.flags = static_cast<std::uint16_t>(value);
                    break;
                case e2dEffectProperty::COLOR:
                {
                    SColor color = TOCOLOR2SCOLOR(value);
                    light.color = RwColor{color.R, color.G, color.B, color.A};

                    break;
                }
                default:
                    break;
            }

            break;
        }
        case e2dEffectType::ROADSIGN:
        {
            t2dEffectRoadsign& roadsign = effect->effect.roadsign;

            switch (property)
            {
                case e2dEffectProperty::FLAGS:
                    roadsign.flags = static_cast<std::uint8_t>(value);
                    break;
                case e2dEffectProperty::COLOR:
                {
                    SColor  color = TOCOLOR2SCOLOR(value);
                    RwColor newColor = RwColor{color.R, color.G, color.B, color.A};

                    pGame->GetModelInfo(model)->Store2DFXRoadsignColor(index, newColor);
                    SetRoadsignTextColor(effect, newColor);
                    break;
                }
                default:
                    break;
            }

            break;
        }
        case e2dEffectType::ESCALATOR:
        {
            t2dEffectEscalator& escalator = effect->effect.escalator;
            if (property == e2dEffectProperty::DIRECTION)
                escalator.direction = static_cast<std::uint8_t>(value);

            break;
        }
        default:
            break;
    }
}

void C2DEffectsSA::Set2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, bool value)
{
    C2DEffectSAInterface* effect = GetEffect(model, index);
    if (!effect || effect->type != e2dEffectType::LIGHT || property != e2dEffectProperty::CORONA_REFLECTION)
        return;

    effect->effect.light.coronaEnableReflection = value;
}

void C2DEffectsSA::Set2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, std::string& value)
{
    C2DEffectSAInterface* effect = GetEffect(model, index);
    if (!effect)
        return;

    switch (effect->type)
    {
        case e2dEffectType::LIGHT:
        {
            t2dEffectLight light = effect->effect.light;

            switch (property)
            {
                case e2dEffectProperty::CORONA_NAME:
                {
                    if (light.coronaTex)
                        RwTextureDestroy(light.coronaTex);

                    CTxdStore_PushCurrentTxd();
                    SetTextureDict(CTxdStore_FindTxdSlot("particle"));
                    light.coronaTex = RwReadTexture(value.c_str(), nullptr);
                    CTxdStore_PopCurrentTxd();
                    break;
                }
                case e2dEffectProperty::SHADOW_NAME:
                {
                    if (light.shadowTex)
                        RwTextureDestroy(light.shadowTex);

                    CTxdStore_PushCurrentTxd();
                    SetTextureDict(CTxdStore_FindTxdSlot("particle"));
                    light.shadowTex = RwReadTexture(value.c_str(), nullptr);
                    CTxdStore_PopCurrentTxd();
                    break;
                }
                default:
                    break;
            }

            break;
        }
        case e2dEffectType::PARTICLE:
        {
            if (property == e2dEffectProperty::PRT_NAME)
                std::strncpy(effect->effect.particle.szName, value.c_str(), 24);

            break;
        }
        case e2dEffectType::ROADSIGN:
        {
            t2dEffectRoadsign& roadsign = effect->effect.roadsign;

            switch (property)
            {
                case e2dEffectProperty::TEXT_1:
                    std::strncpy(roadsign.text, value.c_str(), 16);
                    break;
                case e2dEffectProperty::TEXT_2:
                    std::strncpy(roadsign.text + 16, value.c_str(), 16);
                    break;
                case e2dEffectProperty::TEXT_3:
                    std::strncpy(roadsign.text + 32, value.c_str(), 16);
                    break;
                case e2dEffectProperty::TEXT_4:
                    std::strncpy(roadsign.text + 48, value.c_str(), 16);
                    break;
                default:
                    break;
            }
        }
        default:
            break;
    }
}

void C2DEffectsSA::Set2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, e2dCoronaFlashType& value)
{
    C2DEffectSAInterface* effect = GetEffect(model, index);
    if (!effect || effect->type != e2dEffectType::LIGHT || property != e2dEffectProperty::FLASH_TYPE)
        return;

    effect->effect.light.coronaFlashType = value;
}

void C2DEffectsSA::Set2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, const std::vector<float>& value)
{
    C2DEffectSAInterface* effect = GetEffect(model, index);
    if (!effect)
        return;

    switch (effect->type)
    {
        case e2dEffectType::LIGHT:
        {
            if (property == e2dEffectProperty::OFFSET)
            {
                effect->effect.light.offsetX = value[0];
                effect->effect.light.offsetY = value[1];
                effect->effect.light.offsetZ = value[2];
            }

            break;
        }
        case e2dEffectType::ROADSIGN:
        {
            switch (property)
            {
                case e2dEffectProperty::SIZE:
                    effect->effect.roadsign.size = RwV2d{value[0], value[1]};
                    break;
                case e2dEffectProperty::ROT:
                    effect->effect.roadsign.rotation = RwV3d{value[0], value[1], value[2]};
                    break;
                default:
                    break;
            }

            break;
        }
        case e2dEffectType::ESCALATOR:
        {
            switch (property)
            {
                case e2dEffectProperty::BOTTOM:
                    effect->effect.escalator.bottom = RwV3d{value[0], value[1], value[2]};
                    break;
                case e2dEffectProperty::TOP:
                    effect->effect.escalator.top = RwV3d{value[0], value[1], value[2]};
                    break;
                case e2dEffectProperty::END:
                    effect->effect.escalator.end = RwV3d{value[0], value[1], value[2]};
                    break;
                default:
                    break;
            }

            break;
        }
        default:
            break;
    }
}

void C2DEffectsSA::Reset2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, const S2DEffectData& originalProperties)
{
    C2DEffectSAInterface* effect = GetEffect(model, index);
    if (!effect)
        return;

    S2DEffectData& currProperties = Get2DFXPropertiesData(model, index);
    UpdatePropertiesData(currProperties, originalProperties, property);
    Set2DFXProperties(effect, currProperties);
}

S2DEffectData C2DEffectsSA::Get2DFXPropertiesData(std::uint32_t model, std::uint32_t index) const
{
    S2DEffectData         data{};
    C2DEffectSAInterface* effect = GetEffect(model, index);

    if (effect)
    {
        data.position = effect->position;

        switch (effect->type)
        {
            case e2dEffectType::LIGHT:
            {
                t2dEffectLight& light = effect->effect.light;
                data.drawDistance = light.coronaFarClip;
                data.lightRange = light.pointLightRange;
                data.coronaSize = light.coronaSize;
                data.shadowSize = light.shadowSize;
                data.shadowDist = light.shadowZDistance;
                data.shadowMult = light.shadowColorMultiplier;
                data.coronaFlashType = light.coronaFlashType;
                data.coronaFlareType = light.coronaFlareType;
                data.coronaReflection = light.coronaEnableReflection;
                data.offset = {light.offsetX, light.offsetY, light.offsetZ};
                data.coronaName = std::string(light.coronaTex->name, 32);
                data.shadowName = std::string(light.shadowTex->name, 32);
                data.flags = light.flags;

                SColor color;
                color.R = light.color.r;
                color.G = light.color.g;
                color.B = light.color.b;
                color.A = light.color.a;

                data.color = color;
                break;
            }
            case e2dEffectType::PARTICLE:
            {
                data.prt_name = std::string(effect->effect.particle.szName, strnlen(effect->effect.particle.szName, 24));
                break;
            }
            case e2dEffectType::ROADSIGN:
            {
                t2dEffectRoadsign& roadsign = effect->effect.roadsign;

                data.flags = roadsign.flags;
                data.size = CVector{roadsign.size.x, roadsign.size.y};
                data.rot = CVector{roadsign.rotation.x, roadsign.rotation.y, roadsign.rotation.z};

                data.text_1 = std::string(roadsign.text, strnlen(roadsign.text, 16));
                data.text_2 = std::string(roadsign.text + 16, strnlen(roadsign.text + 16, 16));
                data.text_3 = std::string(roadsign.text + 32, strnlen(roadsign.text + 32, 16));
                data.text_4 = std::string(roadsign.text + 48, strnlen(roadsign.text + 48, 16));

                pGame->Get2DFXEffects()->Get2DFXProperty(model, index, e2dEffectProperty::COLOR, data.color);
                break;
            }
            case e2dEffectType::ESCALATOR:
            {
                t2dEffectEscalator& escalator = effect->effect.escalator;

                data.bottom = CVector{escalator.bottom.x, escalator.bottom.y, escalator.bottom.z};
                data.top = CVector{escalator.top.x, escalator.top.y, escalator.top.z};
                data.end = CVector{escalator.end.x, escalator.end.y, escalator.end.z};
                data.direction = escalator.direction;
                break;
            }
        }
    }

    return data;
}

void C2DEffectsSA::Get2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, float& outValue) const
{
    C2DEffectSAInterface* effect = GetEffect(model, index);
    if (!effect)
        return;

    switch (effect->type)
    {
        case e2dEffectType::LIGHT:
        {
            t2dEffectLight& light = effect->effect.light;

            switch (property)
            {
                case e2dEffectProperty::FAR_CLIP_DISTANCE:
                    outValue = light.coronaFarClip;
                    break;
                case e2dEffectProperty::LIGHT_RANGE:
                    outValue = light.pointLightRange;
                    break;
                case e2dEffectProperty::CORONA_SIZE:
                    outValue = light.coronaSize;
                    break;
                case e2dEffectProperty::SHADOW_SIZE:
                    outValue = light.shadowSize;
                    break;
                case e2dEffectProperty::SHADOW_MULT:
                    outValue = light.shadowColorMultiplier;
                    break;
                case e2dEffectProperty::FLARE_TYPE:
                    outValue = light.coronaFlareType;
                    break;
                case e2dEffectProperty::SHADOW_DISTANCE:
                    outValue = light.shadowZDistance;
                    break;
                case e2dEffectProperty::FLAGS:
                    outValue = light.flags;
                    break;
                default:
                    break;
            }

            break;
        }
        case e2dEffectType::ROADSIGN:
        {
            t2dEffectRoadsign& roadsign = effect->effect.roadsign;

            switch (property)
            {
                case e2dEffectProperty::FLAGS:
                    outValue = roadsign.flags;
                    break;
                default:
                    break;
            }

            break;
        }
        case e2dEffectType::ESCALATOR:
        {
            t2dEffectEscalator& escalator = effect->effect.escalator;
            outValue = static_cast<float>(escalator.direction);

            break;
        }
        default:
            break;
    }
}

void C2DEffectsSA::Get2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, bool& outValue) const
{
    C2DEffectSAInterface* effect = GetEffect(model, index);
    if (!effect)
        return;

    if (effect->type == e2dEffectType::LIGHT && property == e2dEffectProperty::CORONA_REFLECTION)
        outValue = effect->effect.light.coronaEnableReflection;
}

void C2DEffectsSA::Get2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, std::string& outValue) const
{
    C2DEffectSAInterface* effect = GetEffect(model, index);
    if (!effect)
        return;

    switch (effect->type)
    {
        case e2dEffectType::LIGHT:
        {
            t2dEffectLight& light = effect->effect.light;

            switch (property)
            {
                case e2dEffectProperty::CORONA_NAME:
                    outValue = light.coronaTex ? std::string(light.coronaTex->name, strnlen(light.coronaTex->name, 32)) : "";
                    break;
                case e2dEffectProperty::SHADOW_NAME:
                    outValue = light.shadowTex ? std::string(light.shadowTex->name, strnlen(light.shadowTex->name, 32)) : "";
                    break;
                default:
                    break;
            }
        }
        case e2dEffectType::PARTICLE:
        {
            if (property == e2dEffectProperty::PRT_NAME)
                outValue = std::string(effect->effect.particle.szName, strnlen(effect->effect.particle.szName, 24));

            break;
        }
        case e2dEffectType::ROADSIGN:
        {
            t2dEffectRoadsign& roadsign = effect->effect.roadsign;

            switch (property)
            {
                case e2dEffectProperty::TEXT_1:
                    outValue = std::string(roadsign.text, strnlen(roadsign.text, 16));
                    break;
                case e2dEffectProperty::TEXT_2:
                    outValue = std::string(roadsign.text + 16, strnlen(roadsign.text + 16, 16));
                    break;
                case e2dEffectProperty::TEXT_3:
                    outValue = std::string(roadsign.text + 32, strnlen(roadsign.text + 32, 16));
                    break;
                case e2dEffectProperty::TEXT_4:
                    outValue = std::string(roadsign.text + 48, strnlen(roadsign.text + 48, 16));
                    break;
                default:
                    break;
            }

            break;
        }
        default:
            break;
    }
}

void C2DEffectsSA::Get2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, e2dCoronaFlashType& outValue) const
{
    C2DEffectSAInterface* effect = GetEffect(model, index);
    if (!effect)
        return;

    if (effect->type == e2dEffectType::LIGHT && property == e2dEffectProperty::FLASH_TYPE)
        outValue = effect->effect.light.coronaFlashType;
}

void C2DEffectsSA::Get2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, SColor& outValue) const
{
    C2DEffectSAInterface* effect = GetEffect(model, index);
    if (!effect)
        return;

    switch (effect->type)
    {
        case e2dEffectType::LIGHT:
        {
            t2dEffectLight& light = effect->effect.light;
            outValue.R = light.color.r;
            outValue.G = light.color.g;
            outValue.B = light.color.b;
            outValue.A = light.color.a;
            break;
        }
        case e2dEffectType::ROADSIGN:
        {
            RwColor& textColor = GetRoadsignTextColor(effect);
            outValue.R = textColor.r;
            outValue.G = textColor.g;
            outValue.B = textColor.b;
            outValue.A = textColor.a;
            break;
        }
        default:
            break;
    }
}

void C2DEffectsSA::Get2DFXProperty(std::uint32_t model, std::uint32_t index, const e2dEffectProperty& property, std::vector<float>& outValue) const
{
    C2DEffectSAInterface* effect = GetEffect(model, index);
    if (!effect)
        return;

    switch (effect->type)
    {
        case e2dEffectType::LIGHT:
        {
            t2dEffectLight& light = effect->effect.light;
            outValue = {static_cast<float>(light.offsetX), static_cast<float>(light.offsetY), static_cast<float>(light.offsetZ)};
            break;
        }
        case e2dEffectType::ROADSIGN:
        {
            t2dEffectRoadsign& roadsign = effect->effect.roadsign;

            switch (property)
            {
                case e2dEffectProperty::SIZE:
                    outValue = {roadsign.size.x, roadsign.size.y};
                    break;
                case e2dEffectProperty::ROT:
                    outValue = {roadsign.rotation.x, roadsign.rotation.y, roadsign.rotation.z};
                    break;
                default:
                    break;
            }
        }
        case e2dEffectType::ESCALATOR:
        {
            t2dEffectEscalator& escalator = effect->effect.escalator;

            switch (property)
            {
                case e2dEffectProperty::BOTTOM:
                    outValue = {escalator.bottom.x, escalator.bottom.y, escalator.bottom.z};
                    break;
                case e2dEffectProperty::TOP:
                    outValue = {escalator.top.x, escalator.top.y, escalator.top.z};
                    break;
                case e2dEffectProperty::END:
                    outValue = {escalator.end.x, escalator.end.y, escalator.end.z};
                    break;
                default:
                    break;
            }
        }
        default:
            break;
    }
}

void C2DEffectsSA::SetRoadsignTextColor(C2DEffectSAInterface* effect, const RwColor& color)
{
    if (!effect || effect->type != e2dEffectType::ROADSIGN)
        return;

    t2dEffectRoadsign& roadsign = effect->effect.roadsign;
    if (!roadsign.atomic)
        return;

    RpGeometry* geometry = roadsign.atomic->geometry;
    if (!geometry)
        return;

    RpMaterials materials = geometry->materials;
    for (std::size_t i = 0; i < materials.entries; i++)
    {
        RpMaterial* material = materials.materials[i];
        if (material)
            material->color = color;
    }
}

RwColor C2DEffectsSA::GetRoadsignTextColor(C2DEffectSAInterface* effect) const
{
    RwColor dummy = RwColor{0, 0, 0, 0};

    if (!effect || effect->type != e2dEffectType::ROADSIGN)
        return dummy;

    t2dEffectRoadsign& roadsign = effect->effect.roadsign;
    if (!roadsign.atomic)
        return dummy;

    RpGeometry* geometry = roadsign.atomic->geometry;
    if (!geometry)
        return dummy;

    RpMaterials materials = geometry->materials;
    if (materials.entries > 0)
        return materials.materials[0]->color;
    else
        return dummy;
}

void C2DEffectsSA::SetRoadsignText(C2DEffectSAInterface* effect, const std::string& text, std::uint8_t line)
{
    if (!effect || effect->type != e2dEffectType::ROADSIGN)
        return;

    if (!effect->effect.roadsign.text)
    {
        effect->effect.roadsign.text = static_cast<char*>(std::malloc(64));

        if (effect->effect.roadsign.text)
            ZeroMemory(effect->effect.roadsign.text, 64);
    }

    if (effect->effect.roadsign.text)
    {
        ZeroMemory(effect->effect.roadsign.text + 16 * (line - 1), 16);
        std::strncpy(effect->effect.roadsign.text + 16 * (line - 1), text.c_str(), 16);
    }
}

void C2DEffectsSA::UpdatePropertiesData(S2DEffectData& dst, const S2DEffectData& src, const e2dEffectProperty& property) const
{
    switch (property)
    {
        case e2dEffectProperty::FAR_CLIP_DISTANCE:
            dst.drawDistance = src.drawDistance;
            break;
        case e2dEffectProperty::LIGHT_RANGE:
            dst.lightRange = src.lightRange;
            break;
        case e2dEffectProperty::CORONA_SIZE:
            dst.coronaSize = src.coronaSize;
            break;
        case e2dEffectProperty::SHADOW_SIZE:
            dst.shadowSize = src.shadowSize;
            break;
        case e2dEffectProperty::SHADOW_MULT:
            dst.shadowMult = src.shadowMult;
            break;
        case e2dEffectProperty::FLASH_TYPE:
            dst.coronaFlashType = src.coronaFlashType;
            break;
        case e2dEffectProperty::CORONA_REFLECTION:
            dst.coronaReflection = src.coronaReflection;
            break;
        case e2dEffectProperty::FLARE_TYPE:
            dst.coronaFlareType = src.coronaFlareType;
            break;
        case e2dEffectProperty::SHADOW_DISTANCE:
            dst.shadowDist = src.shadowDist;
            break;
        case e2dEffectProperty::OFFSET:
            dst.offset = src.offset;
            break;
        case e2dEffectProperty::COLOR:
            dst.color = src.color;
            break;
        case e2dEffectProperty::CORONA_NAME:
            dst.coronaName = src.coronaName;
            break;
        case e2dEffectProperty::SHADOW_NAME:
            dst.shadowName = src.shadowName;
            break;
        case e2dEffectProperty::FLAGS:
            dst.flags = src.flags;
            break;
        case e2dEffectProperty::PRT_NAME:
            dst.prt_name = src.prt_name;
            break;
        case e2dEffectProperty::SIZE:
            dst.size = src.size;
            break;
        case e2dEffectProperty::ROT:
            dst.rot = src.rot;
            break;
        case e2dEffectProperty::TEXT_1:
            dst.text_1 = src.text_1;
            break;
        case e2dEffectProperty::TEXT_2:
            dst.text_2 = src.text_2;
            break;
        case e2dEffectProperty::TEXT_3:
            dst.text_3 = src.text_3;
            break;
        case e2dEffectProperty::TEXT_4:
            dst.text_4 = src.text_4;
            break;
        case e2dEffectProperty::BOTTOM:
            dst.bottom = src.bottom;
            break;
        case e2dEffectProperty::TOP:
            dst.top = src.top;
            break;
        case e2dEffectProperty::END:
            dst.end = src.end;
            break;
        case e2dEffectProperty::DIRECTION:
            dst.direction = src.direction;
            break;
        case e2dEffectProperty::POSITION:
            dst.position = src.position;
            break;
        default:
            break;
    }
}

RpAtomic* C2DEffectsSA::Roadsign_CreateAtomic(const CVector& position, const RwV3d& rotation, float sizeX, float sizeY, std::uint32_t numLines, char* line1,
                                              char* line2, char* line3, char* line4, std::uint32_t numLetters, std::uint8_t palleteID)
{
    // Call CCustomRoadsignMgr::CreateRoadsignAtomic
    RpAtomic* atomic = ((RpAtomic * (__cdecl*)(float, float, std::int32_t, char*, char*, char*, char*, std::int32_t, std::uint8_t))0x6FF2D0)(
        sizeX, sizeY, numLines, line1, line2, line3, line4, numLetters, palleteID);
    if (!atomic)
        return nullptr;

    RwFrame* frame = RpAtomicGetFrame(atomic);
    if (frame)
    {
        RwFrameSetIdentity(frame);

        const RwV3d axis0{1.0f, 0.0f, 0.0f}, axis1{0.0f, 1.0f, 0.0f}, axis2{0.0f, 0.0f, 1.0f};
        RwFrameRotate(frame, &axis2, rotation.z, rwCOMBINEREPLACE);
        RwFrameRotate(frame, &axis0, rotation.x, rwCOMBINEPOSTCONCAT);
        RwFrameRotate(frame, &axis1, rotation.y, rwCOMBINEPOSTCONCAT);

        RwV3d pos{position.fX, position.fY, position.fZ};
        RwFrameTranslate(frame, &pos, TRANSFORM_AFTER);
        RwFrameUpdateObjects(frame);
    }

    return atomic;
}

std::uint32_t C2DEffectsSA::Roadsign_GetPalleteIDFromFlags(std::uint8_t flags) const noexcept
{
    std::uint32_t id = (flags >> 4) & 3;
    return id <= 3 ? id : 0;
}

std::uint32_t C2DEffectsSA::Roadsign_GetNumLettersFromFlags(std::uint8_t flags) const noexcept
{
    std::uint32_t letters = (flags >> 2) & 3;
    switch (letters)
    {
        case 1u:
            return 2;
        case 2u:
            return 4;
        case 3u:
            return 8;
        default:
            return 16;
    }
}

std::uint32_t C2DEffectsSA::Roadsign_GetNumLinesFromFlags(std::uint8_t flags) const noexcept
{
    std::uint32_t lines = flags & 3;
    return (lines <= 3 && lines > 0) ? lines : 4;
}

bool C2DEffectsSA::IsPropertyValueNumber(const e2dEffectProperty& property) const noexcept
{
    switch (property)
    {
        case e2dEffectProperty::FAR_CLIP_DISTANCE:
        case e2dEffectProperty::LIGHT_RANGE:
        case e2dEffectProperty::CORONA_SIZE:
        case e2dEffectProperty::SHADOW_SIZE:
        case e2dEffectProperty::SHADOW_MULT:
        case e2dEffectProperty::FLARE_TYPE:
        case e2dEffectProperty::SHADOW_DISTANCE:
        case e2dEffectProperty::FLAGS:
        case e2dEffectProperty::DIRECTION:
            return true;
        default:
            return false;
    }
}

bool C2DEffectsSA::IsPropertyValueString(const e2dEffectProperty& property) const noexcept
{
    switch (property)
    {
        case e2dEffectProperty::CORONA_NAME:
        case e2dEffectProperty::SHADOW_NAME:
        case e2dEffectProperty::PRT_NAME:
        case e2dEffectProperty::TEXT_1:
        case e2dEffectProperty::TEXT_2:
        case e2dEffectProperty::TEXT_3:
        case e2dEffectProperty::TEXT_4:
            return true;
        default:
            return false;
    }
}

bool C2DEffectsSA::IsPropertyValueTable(const e2dEffectProperty& property) const noexcept
{
    switch (property)
    {
        case e2dEffectProperty::OFFSET:
        case e2dEffectProperty::SIZE:
        case e2dEffectProperty::ROT:
        case e2dEffectProperty::BOTTOM:
        case e2dEffectProperty::TOP:
        case e2dEffectProperty::END:
            return true;
        default:
            return false;
    }
}
