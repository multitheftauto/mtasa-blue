/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CClient2DFXManager.cpp
 *  PURPOSE:     Handling 2DFX effects manager
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClient2DFXManager.h"

#define mask(n) ((1 << (n)) - 1)
static constexpr std::size_t roadsignTextSize = 64;

CClient2DFXManager::CClient2DFXManager(CClientManager* mainManager) : m_mainManager(mainManager)
{
    // Init
    m_canRemoveFromList = true;
}

CClient2DFXManager::~CClient2DFXManager()
{
    // Delete all 2DFX's
    RemoveAll();
}

void CClient2DFXManager::RemoveAll()
{
    // Make sure they don't remove themselves from our list
    m_canRemoveFromList = false;

    // Run through our list deleting the 2DFX's
    for (auto* effect : m_effectsList)
        delete effect;

    // Allow list removal again
    m_canRemoveFromList = true;
}

void CClient2DFXManager::RemoveFromList(CClient2DFX* effect)
{
    if (m_canRemoveFromList)
        m_effectsList.remove(effect);
}

bool CClient2DFXManager::Exists(CClient2DFX* effect) const
{
    return std::find(m_effectsList.begin(), m_effectsList.end(), effect) != m_effectsList.end();
}

CClient2DFX* CClient2DFXManager::Add2DFX(std::uint32_t model, const CVector& position, const e2dEffectType& type, const effectDataMap& effectData)
{
    auto* effect = new CClient2DFX(m_mainManager, INVALID_ELEMENT_ID);
    if (!effect)
        return nullptr;

    if (!effect->Create(model, position, type, effectData))
    {
        delete effect;
        effect = nullptr;
    }

    return effect;
}

bool CClient2DFXManager::Set2DFXProperties(C2DEffect* effect, const effectDataMap& effectData)
{
    if (!effect)
        return false;

    bool success = true;

    for (const auto& [k, v] : effectData)
    {
        e2dEffectProperty property;
        if (!StringToEnum(k, property))
        {
            success = false;
            continue;
        }

        if (!Set2DFXProperty(effect, property, v))
            success = false;
    }

    return success;
}

effectDataMap CClient2DFXManager::Get2DFXProperties(C2DEffect* effect) const
{
    auto properties = effectDataMap();
    if (!effect)
        return properties;

    // Get properties
    switch (effect->GetEffectType())
    {
        case e2dEffectType::LIGHT:
        {
            MapSet(properties, "drawDistance", effect->GetCoronaFarClip());
            MapSet(properties, "lightRange", effect->GetCoronaPointLightRange());
            MapSet(properties, "coronaSize", effect->GetCoronaSize());
            MapSet(properties, "shadowSize", effect->GetShadowSize());
            MapSet(properties, "shadowMultiplier", static_cast<float>(effect->GetShadowMultiplier()));
            MapSet(properties, "showMode", EnumToString(effect->GetCoronaShowMode()));
            MapSet(properties, "coronaReflection", effect->GetCoronaReflectionsEnabled());
            MapSet(properties, "flareType", static_cast<float>(effect->GetCoronaFlareType()));
            MapSet(properties, "flags", static_cast<float>(effect->GetLightFlags()));
            MapSet(properties, "shadowDistance", static_cast<float>(effect->GetShadowDistance()));

            const CVector& offsets = effect->GetCoronaOffsets();
            MapSet(properties, "offsetX", offsets.fX);
            MapSet(properties, "offsetY", offsets.fY);
            MapSet(properties, "offsetZ", offsets.fZ);

            RwColor& color = effect->GetCoronaColor();
            int colorValue = (static_cast<int>(color.a) << 24) | (static_cast<int>(color.r) << 16) | (static_cast<int>(color.g) << 8) | static_cast<int>(color.b);
            MapSet(properties, "color", static_cast<float>(colorValue));

            MapSet(properties, "coronaName", effect->GetCoronaTexture());
            MapSet(properties, "shadowName", effect->GetShadowTexture());

            break;
        }
        case e2dEffectType::PARTICLE:
        {
            MapSet(properties, "name", effect->GetParticleName());
            break;
        }
        case e2dEffectType::ROADSIGN:
        {
            const RwV2d& size = effect->GetRoadsignSize();
            const RwV3d& rot = effect->GetRoadsignRotation();

            MapSet(properties, "sizeX", size.x);
            MapSet(properties, "sizeY", size.y);
            MapSet(properties, "rotX", rot.x);
            MapSet(properties, "rotY", rot.y);
            MapSet(properties, "rotZ", rot.z);
            MapSet(properties, "flags", static_cast<float>(effect->GetRoadsignFlags()));
            MapSet(properties, "text", effect->GetRoadsignText());

            break;
        }
        case e2dEffectType::ESCALATOR:
        {
            const RwV3d& bottom = effect->GetEscalatorBottom();
            const RwV3d& top = effect->GetEscalatorTop();
            const RwV3d& end = effect->GetEscalatorEnd();

            MapSet(properties, "bottomX", bottom.x);
            MapSet(properties, "bottomY", bottom.y);
            MapSet(properties, "bottomZ", bottom.z);
            MapSet(properties, "topX", top.x);
            MapSet(properties, "topY", top.y);
            MapSet(properties, "topZ", top.z);
            MapSet(properties, "endX", end.x);
            MapSet(properties, "endY", end.y);
            MapSet(properties, "endZ", end.z);
            MapSet(properties, "direction", static_cast<float>(effect->GetEscalatorDirection()));

            break;
        }
    }

    return properties;
}

bool CClient2DFXManager::Set2DFXProperty(C2DEffect* effect, const e2dEffectProperty& property, const std::variant<bool, float, std::string>& propertyValue)
{
    if (!effect)
        return false;

    switch (effect->GetEffectType())
    {
        case e2dEffectType::LIGHT:
        {
            switch (property)
            {
                case e2dEffectProperty::FAR_CLIP_DISTANCE:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        effect->SetCoronaFarClip(std::get<float>(propertyValue));
                        return true;
                    }
                    break;
                }
                case e2dEffectProperty::LIGHT_RANGE:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        effect->SetCoronaPointLightRange(std::get<float>(propertyValue));
                        return true;
                    }
                    break;
                }
                case e2dEffectProperty::CORONA_SIZE:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        effect->SetCoronaSize(std::get<float>(propertyValue));
                        return true;
                    }
                    break;
                }
                case e2dEffectProperty::SHADOW_SIZE:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        effect->SetShadowSize(std::get<float>(propertyValue));
                        return true;
                    }
                    break;
                }
                case e2dEffectProperty::SHADOW_MULT:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        effect->SetShadowMultiplier(std::get<float>(propertyValue));
                        return true;
                    }
                    break;
                }
                case e2dEffectProperty::FLASH_TYPE:
                {
                    if (std::holds_alternative<std::string>(propertyValue))
                    {
                        e2dCoronaFlashType showMode;
                        if (StringToEnum(std::get<std::string>(propertyValue), showMode))
                        {
                            effect->SetCoronaShowMode(showMode);
                            return true;
                        }
                    }

                    break;
                }
                case e2dEffectProperty::CORONA_REFLECTION:
                {
                    if (std::holds_alternative<bool>(propertyValue))
                    {
                        effect->SetCoronaReflectionsEnabled(std::get<bool>(propertyValue));
                        return true;
                    }
                    break;
                }
                case e2dEffectProperty::FLARE_TYPE:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        effect->SetCoronaFlareType(static_cast<std::uint8_t>(std::get<float>(propertyValue)));
                        return true;
                    }
                    break;
                }
                case e2dEffectProperty::FLAGS:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        effect->SetLightFlags(static_cast<std::uint16_t>(std::get<float>(propertyValue)));
                        return true;
                    }
                    break;
                }
                case e2dEffectProperty::SHADOW_DISTANCE:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        effect->SetShadowDistance(static_cast<std::int8_t>(std::get<float>(propertyValue)));
                        return true;
                    }
                    break;
                }
                case e2dEffectProperty::OFFSET_X:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        CVector offsets = effect->GetCoronaOffsets();
                        offsets.fX = static_cast<std::int8_t>(std::get<float>(propertyValue));

                        effect->SetCoronaOffsets(offsets);
                        return true;
                    }
                    break;
                }
                case e2dEffectProperty::OFFSET_Y:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        CVector offsets = effect->GetCoronaOffsets();
                        offsets.fY = static_cast<std::int8_t>(std::get<float>(propertyValue));

                        effect->SetCoronaOffsets(offsets);
                        return true;
                    }
                    break;
                }
                case e2dEffectProperty::OFFSET_Z:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        CVector offsets = effect->GetCoronaOffsets();
                        offsets.fZ = static_cast<std::int8_t>(std::get<float>(propertyValue));

                        effect->SetCoronaOffsets(offsets);
                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::COLOR:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        std::uint32_t colorValue = static_cast<std::uint32_t>(std::get<float>(propertyValue));
                        effect->SetCoronaColor(RwColor{static_cast<std::uint8_t>((colorValue >> 16) & mask(8)), static_cast<std::uint8_t>((colorValue >> 8) & mask(8)), static_cast<std::uint8_t>((colorValue >> 0) & mask(8)), static_cast<std::uint8_t>((colorValue >> 24) & mask(8))});

                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::CORONA_NAME:
                {
                    if (std::holds_alternative<std::string>(propertyValue))
                    {
                        e2dEffectTextureName coronaName;
                        if (StringToEnum(std::get<std::string>(propertyValue), coronaName))
                        {
                            effect->SetCoronaTexture(std::get<std::string>(propertyValue));
                            return true;
                        }
                    }

                    break;
                }
                case e2dEffectProperty::SHADOW_NAME:
                {
                    if (std::holds_alternative<std::string>(propertyValue))
                    {
                        e2dEffectTextureName shadowName;
                        if (StringToEnum(std::get<std::string>(propertyValue), shadowName))
                        {
                            effect->SetShadowTexture(std::get<std::string>(propertyValue));
                            return true;
                        }
                    }

                    break;
                }
            }

            break;
        }
        case e2dEffectType::PARTICLE:
        {
            if (property == e2dEffectProperty::PRT_NAME)
            {
                if (std::holds_alternative<std::string>(propertyValue))
                {
                    effect->SetParticleName(std::get<std::string>(propertyValue));
                    return true;
                }
            }

            break;
        }
        case e2dEffectType::ROADSIGN:
        {
            switch (property)
            {
                case e2dEffectProperty::SIZE_X:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        RwV2d& size = effect->GetRoadsignSize();
                        size.x = std::get<float>(propertyValue);

                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::SIZE_Y:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        RwV2d& size = effect->GetRoadsignSize();
                        size.y = std::get<float>(propertyValue);

                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::ROT_X:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        RwV3d& rotation = effect->GetRoadsignRotation();
                        rotation.x = std::get<float>(propertyValue);

                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::ROT_Y:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        RwV3d& rotation = effect->GetRoadsignRotation();
                        rotation.y = std::get<float>(propertyValue);

                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::ROT_Z:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        RwV3d& rotation = effect->GetRoadsignRotation();
                        rotation.z = std::get<float>(propertyValue);

                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::FLAGS:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        effect->SetRoadsignFlags(static_cast<std::uint8_t>(std::get<float>(propertyValue)));
                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::TEXT:
                {
                    if (std::holds_alternative<std::string>(propertyValue))
                    {
                        effect->SetRoadsignText(std::get<std::string>(propertyValue), 1);
                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::TEXT_2:
                {
                    if (std::holds_alternative<std::string>(propertyValue))
                    {
                        effect->SetRoadsignText(std::get<std::string>(propertyValue), 2);
                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::TEXT_3:
                {
                    if (std::holds_alternative<std::string>(propertyValue))
                    {
                        effect->SetRoadsignText(std::get<std::string>(propertyValue), 3);
                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::TEXT_4:
                {
                    if (std::holds_alternative<std::string>(propertyValue))
                    {
                        effect->SetRoadsignText(std::get<std::string>(propertyValue), 4);
                        return true;
                    }

                    break;
                }
            }
            break;
        }
        case e2dEffectType::ESCALATOR:
        {
            switch (property)
            {
                case e2dEffectProperty::BOTTOM_X:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        RwV3d& bottom = effect->GetEscalatorBottom();
                        bottom.x = std::get<float>(propertyValue);

                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::BOTTOM_Y:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        RwV3d& bottom = effect->GetEscalatorBottom();
                        bottom.y = std::get<float>(propertyValue);

                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::BOTTOM_Z:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        RwV3d& bottom = effect->GetEscalatorBottom();
                        bottom.z = std::get<float>(propertyValue);

                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::TOP_X:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        RwV3d& top = effect->GetEscalatorTop();
                        top.x = std::get<float>(propertyValue);

                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::TOP_Y:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        RwV3d& top = effect->GetEscalatorTop();
                        top.y = std::get<float>(propertyValue);

                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::TOP_Z:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        RwV3d& top = effect->GetEscalatorTop();
                        top.z = std::get<float>(propertyValue);

                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::END_X:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        RwV3d& end = effect->GetEscalatorEnd();
                        end.x = std::get<float>(propertyValue);

                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::END_Y:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        RwV3d& end = effect->GetEscalatorEnd();
                        end.y = std::get<float>(propertyValue);

                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::END_Z:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        RwV3d& end = effect->GetEscalatorEnd();
                        end.z = std::get<float>(propertyValue);

                        return true;
                    }

                    break;
                }
                case e2dEffectProperty::DIRECTION:
                {
                    if (std::holds_alternative<float>(propertyValue))
                    {
                        effect->SetEscalatorDirection(static_cast<std::uint8_t>(std::get<float>(propertyValue)));
                        return true;
                    }

                    break;
                }
            }

            break;
        }
    }

    return false;
}

std::variant<float, bool, std::string> CClient2DFXManager::Get2DFXProperty(C2DEffect* effect, const e2dEffectProperty& property)
{
    if (!effect)
        return false;

    switch (effect->GetEffectType())
    {
        case e2dEffectType::LIGHT:
        {
            switch (property)
            {
                case e2dEffectProperty::FAR_CLIP_DISTANCE:
                    return effect->GetCoronaFarClip();
                case e2dEffectProperty::LIGHT_RANGE:
                    return effect->GetCoronaPointLightRange();
                case e2dEffectProperty::CORONA_SIZE:
                    return effect->GetCoronaSize();
                case e2dEffectProperty::SHADOW_SIZE:
                    return effect->GetShadowSize();
                case e2dEffectProperty::SHADOW_MULT:
                    return static_cast<float>(effect->GetShadowMultiplier());
                case e2dEffectProperty::FLASH_TYPE:
                    return EnumToString(effect->GetCoronaShowMode());
                case e2dEffectProperty::CORONA_REFLECTION:
                    return effect->GetCoronaReflectionsEnabled();
                case e2dEffectProperty::FLARE_TYPE:
                    return static_cast<float>(effect->GetCoronaFlareType());
                case e2dEffectProperty::FLAGS:
                    return static_cast<float>(effect->GetLightFlags());
                case e2dEffectProperty::SHADOW_DISTANCE:
                    return static_cast<float>(effect->GetShadowDistance());
                case e2dEffectProperty::OFFSET_X:
                    return static_cast<float>(effect->GetCoronaOffsets().fX);
                case e2dEffectProperty::OFFSET_Y:
                    return static_cast<float>(effect->GetCoronaOffsets().fY);
                case e2dEffectProperty::OFFSET_Z:
                    return static_cast<float>(effect->GetCoronaOffsets().fZ);
                case e2dEffectProperty::COLOR:
                {
                    RwColor& color = effect->GetCoronaColor();
                    int colorValue = (static_cast<int>(color.a) << 24) | (static_cast<int>(color.r) << 16) | (static_cast<int>(color.g) << 8) | static_cast<int>(color.b);

                    return static_cast<float>(colorValue);
                }
                case e2dEffectProperty::CORONA_NAME:
                    return effect->GetCoronaTexture();
                case e2dEffectProperty::SHADOW_NAME:
                    return effect->GetShadowTexture();
            }

            break;
        }
        case e2dEffectType::PARTICLE:
        {
            if (property == e2dEffectProperty::PRT_NAME)
                return effect->GetParticleName();

            break;
        }
        case e2dEffectType::ROADSIGN:
        {
            switch (property)
            {
                case e2dEffectProperty::SIZE_X:
                    return effect->GetRoadsignSize().x;
                case e2dEffectProperty::SIZE_Y:
                    return effect->GetRoadsignSize().y;
                case e2dEffectProperty::ROT_X:
                    return effect->GetRoadsignRotation().x;
                case e2dEffectProperty::ROT_Y:
                    return effect->GetRoadsignRotation().y;
                case e2dEffectProperty::ROT_Z:
                    return effect->GetRoadsignRotation().z;
                case e2dEffectProperty::FLAGS:
                    return static_cast<float>(effect->GetRoadsignFlags());
                case e2dEffectProperty::TEXT:
                case e2dEffectProperty::TEXT_2:
                case e2dEffectProperty::TEXT_3:
                case e2dEffectProperty::TEXT_4:
                    return effect->GetRoadsignText();
            }

            break;
        }
        case e2dEffectType::ESCALATOR:
        {
            switch (property)
            {
                case e2dEffectProperty::BOTTOM_X:
                    return effect->GetEscalatorBottom().x;
                case e2dEffectProperty::BOTTOM_Y:
                    return effect->GetEscalatorBottom().y;
                case e2dEffectProperty::BOTTOM_Z:
                    return effect->GetEscalatorBottom().z;
                case e2dEffectProperty::TOP_X:
                    return effect->GetEscalatorTop().x;
                case e2dEffectProperty::TOP_Y:
                    return effect->GetEscalatorTop().y;
                case e2dEffectProperty::TOP_Z:
                    return effect->GetEscalatorTop().z;
                case e2dEffectProperty::END_X:
                    return effect->GetEscalatorEnd().x;
                case e2dEffectProperty::END_Y:
                    return effect->GetEscalatorEnd().y;
                case e2dEffectProperty::END_Z:
                    return effect->GetEscalatorEnd().z;
                case e2dEffectProperty::DIRECTION:
                    return static_cast<float>(effect->GetEscalatorDirection());
            }

            break;
        }
    }

    return false;
}

void CClient2DFXManager::Set2DFXPosition(C2DEffect* effect, const CVector& position)
{
    if (!effect)
        return;

    effect->SetPosition(position);
}

CVector* CClient2DFXManager::Get2DFXPosition(C2DEffect* effect) const
{
    if (!effect)
        return nullptr;

    return &effect->GetPosition();
}

const char* CClient2DFXManager::IsValidEffectData(const e2dEffectType& effectType, const effectDataMap& effectData)
{
    // Check if keys & values are ok!
    switch (effectType)
    {
        case e2dEffectType::LIGHT:
        {
            // corona far clip
            auto* drawDistance = MapFind(effectData, "drawDistance");
            if (!drawDistance || !std::holds_alternative<float>(*drawDistance))
                return "Invalid \"drawDistance\" value";

            auto* lightRange = MapFind(effectData, "lightRange");
            if (!lightRange || !std::holds_alternative<float>(*lightRange))
                return "Invalid \"lightRange\" value";

            auto* coronaSize = MapFind(effectData, "coronaSize");
            if (!coronaSize || !std::holds_alternative<float>(*coronaSize))
                return "Invalid \"coronaSize\" value";

            auto* shadowSize = MapFind(effectData, "shadowSize");
            if (!shadowSize || !std::holds_alternative<float>(*shadowSize))
                return "Invalid \"shadowSize\" value";

            auto* shadowMultiplier = MapFind(effectData, "shadowMultiplier");
            if (!shadowMultiplier || !std::holds_alternative<float>(*shadowMultiplier) || std::get<float>(*shadowMultiplier) < 0.0f)
                return "Invalid \"shadowMultiplier\" value";

            auto* showMode = MapFind(effectData, "showMode");
            e2dCoronaFlashType tempType;
            if (!showMode || !std::holds_alternative<std::string>(*showMode) || !StringToEnum(std::get<std::string>(*showMode), tempType))
                return "Invalid \"showMode\" value";

            auto* coronaReflection = MapFind(effectData, "coronaReflection");
            if (!coronaReflection || !std::holds_alternative<bool>(*coronaReflection))
                return "Invalid \"coronaReflection\" value";

            auto* coronaFlareType = MapFind(effectData, "flareType");
            if (!coronaFlareType || !std::holds_alternative<float>(*coronaFlareType) || (std::get<float>(*coronaFlareType) < 0.0f || std::get<float>(*coronaFlareType) > 1.0f))
                return "Invalid \"flareType\" value";

            auto* flags = MapFind(effectData, "flags");
            if (!flags || !std::holds_alternative<float>(*flags) || std::get<float>(*flags) < 0.0f)
                return "Invalid \"flags\" value";

            auto* shadowZDistance = MapFind(effectData, "shadowDistance");
            if (!shadowZDistance || (!std::holds_alternative<float>(*shadowZDistance)))
                return "Invalid \"shadowDistance\" value";

            auto* offsetX = MapFind(effectData, "offsetX");
            if (!offsetX || !std::holds_alternative<float>(*offsetX))
                return "Invalid \"offsetX\" value";

            auto* offsetY = MapFind(effectData, "offsetY");
            if (!offsetY || !std::holds_alternative<float>(*offsetY))
                return "Invalid \"offsetY\" value";

            auto* offsetZ = MapFind(effectData, "offsetZ");
            if (!offsetZ || !std::holds_alternative<float>(*offsetZ))
                return "Invalid \"offsetZ\" value";

            auto* color = MapFind(effectData, "color");
            if (!color || !std::holds_alternative<float>(*color))
                return "Invalid \"color\" value";

            auto* coronaTexture = MapFind(effectData, "coronaName");
            e2dEffectTextureName tempName;
            if (!coronaTexture || !std::holds_alternative<std::string>(*coronaTexture) || !StringToEnum(std::get<std::string>(*coronaTexture), tempName))
                return "Invalid \"coronaName\" value";

            auto* shadowTexture = MapFind(effectData, "shadowName");
            e2dEffectTextureName shadowName;
            if (!shadowTexture || !std::holds_alternative<std::string>(*shadowTexture) || !StringToEnum(std::get<std::string>(*shadowTexture), shadowName))
                return "Invalid \"shadowName\" value";

            break;
        }
        case e2dEffectType::PARTICLE:
        {
            auto* particleName = MapFind(effectData, "name");
            if (!particleName || !std::holds_alternative<std::string>(*particleName) || std::get<std::string>(*particleName).length() > 24)
                return "Invalid \"particle name\" value";

            break;
        }
        case e2dEffectType::ROADSIGN:
        {
            auto* sizeX = MapFind(effectData, "sizeX");
            if (!sizeX || !std::holds_alternative<float>(*sizeX))
                return "Invalid \"sizeX\" value";

            auto* sizeY = MapFind(effectData, "sizeY");
            if (!sizeY || !std::holds_alternative<float>(*sizeY))
                return "Invalid \"sizeY\" value";

            auto* rotX = MapFind(effectData, "rotX");
            if (!rotX || !std::holds_alternative<float>(*rotX))
                return "Invalid \"rotX\" value";

            auto* rotY = MapFind(effectData, "rotY");
            if (!rotY || !std::holds_alternative<float>(*rotY))
                return "Invalid \"rotY\" value";

            auto* rotZ = MapFind(effectData, "rotZ");
            if (!rotZ || !std::holds_alternative<float>(*rotZ))
                return "Invalid \"rotZ\" value";

            auto* flags = MapFind(effectData, "flags");
            if (!flags || !std::holds_alternative<float>(*flags) || std::get<float>(*flags) < 0.0f)
                return "Invalid \"flags\" value";

            auto* text = MapFind(effectData, "text1");
            if (!text || !std::holds_alternative<std::string>(*text))
                return "Invalid \"text1\" value";

            auto* text2 = MapFind(effectData, "text2");
            if (!text2 || !std::holds_alternative<std::string>(*text2))
                return "Invalid \"text2\" value";

            auto* text3 = MapFind(effectData, "text3");
            if (!text3 || !std::holds_alternative<std::string>(*text3))
                return "Invalid \"text3\" value";

            auto* text4 = MapFind(effectData, "text4");
            if (!text4 || !std::holds_alternative<std::string>(*text4))
                return "Invalid \"text4\" value";

            break;
        }
        case e2dEffectType::ESCALATOR:
        {
            auto* bottomX = MapFind(effectData, "bottomX");
            if (!bottomX || !std::holds_alternative<float>(*bottomX))
                return "Invalid \"bottomX\" value";

            auto* bottomY = MapFind(effectData, "bottomY");
            if (!bottomY || !std::holds_alternative<float>(*bottomY))
                return "Invalid \"bottomY\" value";

            auto* bottomZ = MapFind(effectData, "bottomZ");
            if (!bottomZ || !std::holds_alternative<float>(*bottomZ))
                return "Invalid \"bottomZ\" value";

            auto* topX = MapFind(effectData, "topX");
            if (!topX || !std::holds_alternative<float>(*topX))
                return "Invalid \"topX\" value";

            auto* topY = MapFind(effectData, "topY");
            if (!topY || !std::holds_alternative<float>(*topY))
                return "Invalid \"topY\" value";

            auto* topZ = MapFind(effectData, "topZ");
            if (!topX || !std::holds_alternative<float>(*topZ))
                return "Invalid \"topZ\" value";

            auto* endX = MapFind(effectData, "endX");
            if (!endX || !std::holds_alternative<float>(*endX))
                return "Invalid \"endX\" value";

            auto* endY = MapFind(effectData, "endY");
            if (!endY || !std::holds_alternative<float>(*endY))
                return "Invalid \"endY\" value";

            auto* endZ = MapFind(effectData, "endZ");
            if (!endZ || !std::holds_alternative<float>(*endZ))
                return "Invalid \"endZ\" value";

            auto* direction = MapFind(effectData, "direction");
            if (!direction || !std::holds_alternative<float>(*direction) || (std::get<float>(*direction) < 0.0f || std::get<float>(*direction) > 1.0f))
                return "Invalid \"direction\" value";

            break;
        }
        case e2dEffectType::SUN_GLARE:
            // It has no properties, it only uses position and type fields
            break;
        case e2dEffectType::ATTRACTOR:
            // Unnecessary in MTA
            break;
        case e2dEffectType::FURNITURE:
            // Unnecessary in MTA (Probably unused even in SA)
            break;
        case e2dEffectType::ENEX:
            // Unnecessary in MTA
            break;
        case e2dEffectType::TRIGGER_POINT:
        {
            // Unnecessary in MTA
            break;
        }
        case e2dEffectType::COVER_POINT:
            // Unnecessary in MTA
            break;
    }

    return nullptr;
}

bool CClient2DFXManager::TryDestroyCustom2DFXEffect(std::uint32_t modelID, std::int8_t index)
{
    bool removed = false;

    if (index == -1)
    {
        for (auto it = m_effectsList.begin(); it != m_effectsList.end();)
        {
            if ((*it)->GetModelID() == modelID)
            {
                delete *it;
                it = m_effectsList.erase(it);

                removed = true;
            }
            else
                ++it;
        }
    }
    else
    {
        CModelInfo* modelInfo = g_pGame->GetModelInfo(modelID);
        if (!modelInfo)
            return false;

        C2DEffect* effect = reinterpret_cast<C2DEffect*>(modelInfo->Get2DFXFromIndex(static_cast<std::uint32_t>(index)));
        if (!effect)
            return false;

        for (auto* clientEffect : m_effectsList)
        {
            if (clientEffect->Get2DFX() == effect)
            {
                RemoveFromList(clientEffect);
                delete clientEffect;

                removed = true;
                break;
            }
        }
    }

    return removed;
}

bool CClient2DFXManager::IsValidModel(std::uint32_t model) noexcept
{
    return CClientObjectManager::IsValidModel(model) || CClientBuildingManager::IsValidModel(model) || CClientVehicleManager::IsValidModel(model);
}
