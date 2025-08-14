#include "StdInc.h"
#include "CClient2DFXManager.h"
#include "game/C2DEffects.h"

bool CClient2DFXManager::InitFlagsFromTable(const e2dEffectType& effectType, const effectFlagsTable& tbl, std::uint16_t& flags)
{
    switch (effectType)
    {
        case e2dEffectType::LIGHT:
        {
            auto GetBoolValue = [&tbl](const e2dEffectFlags& key) -> bool
            {
                auto it = tbl.find(key);
                if (it == tbl.end())
                    return false;

                if (auto val = std::get_if<bool>(&(it->second)))
                    return *val;

                return false;
            };

            if (GetBoolValue(e2dEffectFlags::CHECK_OBSTACLES))
                flags |= (1 << 0);
            if (GetBoolValue(e2dEffectFlags::FOG_TYPE))
                flags |= (1 << 1);
            if (GetBoolValue(e2dEffectFlags::FOG_TYPE2))
                flags |= (1 << 2);
            if (GetBoolValue(e2dEffectFlags::WITHOUT_CORONA))
                flags |= (1 << 3);
            if (GetBoolValue(e2dEffectFlags::ONLY_LONG_DISTANCE))
                flags |= (1 << 4);
            if (GetBoolValue(e2dEffectFlags::AT_DAY))
                flags |= (1 << 5);
            if (GetBoolValue(e2dEffectFlags::AT_NIGHT))
                flags |= (1 << 6);
            if (GetBoolValue(e2dEffectFlags::BLINKING1))
                flags |= (1 << 7);

            if (GetBoolValue(e2dEffectFlags::ONLY_FROM_BELOW))
                flags |= (1 << 8);
            if (GetBoolValue(e2dEffectFlags::BLINKING2))
                flags |= (1 << 9);
            if (GetBoolValue(e2dEffectFlags::UPDATE_HEIGHT_ABOVE_GROUND))
                flags |= (1 << 10);
            if (GetBoolValue(e2dEffectFlags::CHECK_DIRECTION))
                flags |= (1 << 11);
            if (GetBoolValue(e2dEffectFlags::BLINKING3))
                flags |= (1 << 12);

            break;
        }
        case e2dEffectType::ROADSIGN:
        {
            auto GetIntValue = [&tbl](const e2dEffectFlags& key, int defaultValue = 0) -> int
            {
                auto it = tbl.find(key);
                if (it == tbl.end())
                    return defaultValue;

                if (auto val = std::get_if<int>(&it->second))
                    return *val;

                return defaultValue;
            };

            int lines = GetIntValue(e2dEffectFlags::LINES_NUM);
            int linesBits = lines == 4 ? 0 : lines;
            flags |= (linesBits & 0b11);

            int characters = GetIntValue(e2dEffectFlags::CHARACTERS_PER_LINE);
            int charactersBits = 0;

            if (lines < 1 || lines > 4)
                return false;

            if (characters != 2 && characters != 4 && characters != 8 && characters != 16)
                return false;

            if (characters == 16)
                charactersBits = 0;
            else if (characters == 8)
                charactersBits = 3;
            else if (characters == 4)
                charactersBits = 2;
            else if (characters == 2)
                charactersBits = 1;

            flags |= (charactersBits & 0b11) << 2;
            break;
        }

        default:
            break;
    }

    return true;
}

effectFlagsTable CClient2DFXManager::InitFlagsTable(const e2dEffectType& effectType, std::uint16_t flags)
{
    effectFlagsTable tbl;

    switch (effectType)
    {
        case e2dEffectType::LIGHT:
        {
            auto SetFlag = [&tbl, flags](const e2dEffectFlags& key, int bit)
            {
                if (flags & (1 << bit))
                    tbl[key] = true;
            };

            SetFlag(e2dEffectFlags::CHECK_OBSTACLES, 0);
            SetFlag(e2dEffectFlags::FOG_TYPE, 1);
            SetFlag(e2dEffectFlags::FOG_TYPE2, 2);
            SetFlag(e2dEffectFlags::WITHOUT_CORONA, 3);
            SetFlag(e2dEffectFlags::ONLY_LONG_DISTANCE, 4);
            SetFlag(e2dEffectFlags::AT_DAY, 5);
            SetFlag(e2dEffectFlags::AT_NIGHT, 6);
            SetFlag(e2dEffectFlags::BLINKING1, 7);

            SetFlag(e2dEffectFlags::ONLY_FROM_BELOW, 8);
            SetFlag(e2dEffectFlags::BLINKING2, 9);
            SetFlag(e2dEffectFlags::UPDATE_HEIGHT_ABOVE_GROUND, 10);
            SetFlag(e2dEffectFlags::CHECK_DIRECTION, 11);
            SetFlag(e2dEffectFlags::BLINKING3, 12);

            break;
        }
        case e2dEffectType::ROADSIGN:
        {
            int linesBits = flags & 0b11;
            int lines = linesBits == 0 ? 4 : linesBits;
            tbl[e2dEffectFlags::LINES_NUM] = lines;

            int charactersBits = (flags >> 2) & 0b11;
            int characters = 16;

            if (charactersBits == 0)
                characters = 16;
            else if (charactersBits == 3)
                characters = 8;
            else if (charactersBits == 2)
                characters = 4;
            else if (charactersBits == 1)
                characters = 2;

            tbl[e2dEffectFlags::CHARACTERS_PER_LINE] = characters;

            break;
        }

        default:
            break;
    }

    return tbl;
}

S2DEffectData CClient2DFXManager::InitProperties(const e2dEffectType& effectType, const effectPropertiesMap& properties)
{
    S2DEffectData data;

    switch (effectType)
    {
        case e2dEffectType::LIGHT:
        {
            e2dCoronaFlashType showMode = e2dCoronaFlashType::DEFAULT;
            StringToEnum(std::get<std::string>(*MapFind(properties, "showMode")), showMode);

            data.drawDistance = std::get<float>(*MapFind(properties, "drawDistance"));
            data.lightRange = std::get<float>(*MapFind(properties, "lightRange"));
            data.coronaSize = std::get<float>(*MapFind(properties, "coronaSize"));
            data.shadowSize = std::get<float>(*MapFind(properties, "shadowSize"));
            data.shadowMult = std::get<float>(*MapFind(properties, "shadowMultiplier"));
            data.coronaFlashType = showMode;
            data.coronaReflection = std::get<bool>(*MapFind(properties, "coronaReflection"));
            data.coronaFlareType = std::get<float>(*MapFind(properties, "flareType"));
            data.color = TOCOLOR2SCOLOR(static_cast<std::uint32_t>(std::get<float>(*MapFind(properties, "color"))));
            data.shadowDist = std::get<float>(*MapFind(properties, "shadowDistance"));

            auto* flags = MapFind(properties, "flags");
            if (std::holds_alternative<float>(*flags))
                data.flags = static_cast<std::uint16_t>(std::get<float>(*flags));
            else if (std::holds_alternative<effectFlagsTable>(*flags))
            {
                if (!InitFlagsFromTable(effectType, std::get<effectFlagsTable>(*flags), data.flags))
                    data.flags = 0;
            }

            auto& offset = std::get<std::vector<float>>(*MapFind(properties, "offset"));
            data.offset.x = static_cast<std::int8_t>(offset[0]);
            data.offset.y = static_cast<std::int8_t>(offset[1]);
            data.offset.z = static_cast<std::int8_t>(offset[2]);

            data.coronaName = std::get<std::string>(*MapFind(properties, "coronaName"));
            data.shadowName = std::get<std::string>(*MapFind(properties, "shadowName"));
            break;
        }
        case e2dEffectType::PARTICLE:
        {
            data.prt_name = std::get<std::string>(*MapFind(properties, "name"));
            break;
        }
        case e2dEffectType::ROADSIGN:
        {
            auto& size = std::get<std::vector<float>>(*MapFind(properties, "size"));
            data.size = CVector2D(size[0], size[1]);

            auto& rot = std::get<std::vector<float>>(*MapFind(properties, "rotation"));
            data.rot = CVector(rot[0], rot[1], rot[2]);

            data.flags = std::get<float>(*MapFind(properties, "flags"));
            data.color = TOCOLOR2SCOLOR(static_cast<std::uint32_t>(std::get<float>(*MapFind(properties, "color"))));
            data.text_1 = std::get<std::string>(*MapFind(properties, "text1"));
            data.text_2 = std::get<std::string>(*MapFind(properties, "text2"));
            data.text_3 = std::get<std::string>(*MapFind(properties, "text3"));
            data.text_4 = std::get<std::string>(*MapFind(properties, "text4"));
            break;
        }
        case e2dEffectType::ESCALATOR:
        {
            auto& bottom = std::get<std::vector<float>>(*MapFind(properties, "bottom"));
            data.bottom = CVector(bottom[0], bottom[1], bottom[2]);

            auto& top = std::get<std::vector<float>>(*MapFind(properties, "top"));
            data.top = CVector(top[0], top[1], top[2]);

            auto& end = std::get<std::vector<float>>(*MapFind(properties, "end"));
            data.end = CVector(end[0], end[1], end[2]);

            data.direction = std::get<float>(*MapFind(properties, "direction"));
            break;
        }
        default:
            break;
    }

    return data;
}

effectPropertiesMap CClient2DFXManager::GetProperties(std::uint32_t model, std::uint32_t index)
{
    auto        tbl = effectPropertiesMap();
    C2DEffects* effects = g_pGame->Get2DFXEffects();

    switch (effects->Get2DFXEffectType(model, index))
    {
        case e2dEffectType::LIGHT:
        {
            float drawDist = 0.0f;
            float lightRange = 0.0f;
            float coronaSize = 0.0f;
            float shadowSize = 0.0f;
            float shadowMult = 0.0f;
            float shadowDist = 0.0f;
            float flareType = 0.0f;
            float flags = 0;

            std::vector<float> offset;
            offset.reserve(3);

            e2dCoronaFlashType flashType = e2dCoronaFlashType::DEFAULT;

            bool        coronaReflection = false;
            std::string coronaName{};
            std::string shadowName{};
            SColor      color{};

            effects->Get2DFXProperty(model, index, e2dEffectProperty::FAR_CLIP_DISTANCE, drawDist);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::LIGHT_RANGE, lightRange);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::CORONA_SIZE, coronaSize);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::SHADOW_SIZE, shadowSize);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::SHADOW_MULT, shadowMult);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::SHADOW_DISTANCE, shadowDist);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::FLARE_TYPE, flareType);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::FLAGS, flags);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::FLASH_TYPE, flashType);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::CORONA_REFLECTION, coronaReflection);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::CORONA_NAME, coronaName);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::SHADOW_NAME, shadowName);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::COLOR, color);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::OFFSET, offset);

            tbl["drawDistance"] = drawDist;
            tbl["lightRange"] = lightRange;
            tbl["coronaSize"] = coronaSize;
            tbl["shadowSize"] = shadowSize;
            tbl["shadowMultiplier"] = shadowMult;
            tbl["shadowDistance"] = shadowDist;
            tbl["flareType"] = flareType;
            tbl["showMode"] = EnumToString(flashType);
            tbl["coronaReflection"] = coronaReflection;
            tbl["flags"] = flags;
            tbl["offset"] = offset;
            tbl["coronaName"] = coronaName;
            tbl["shadowName"] = shadowName;
            tbl["color"] =
                std::vector<float>{static_cast<float>(color.R), static_cast<float>(color.G), static_cast<float>(color.B), static_cast<float>(color.A)};

            break;
        }
        case e2dEffectType::PARTICLE:
        {
            std::string name{};
            effects->Get2DFXProperty(model, index, e2dEffectProperty::PRT_NAME, name);

            tbl["name"] = name;
            break;
        }
        case e2dEffectType::ROADSIGN:
        {
            std::vector<float> size;
            std::vector<float> rot;
            size.reserve(2);
            rot.reserve(3);

            float       flags = 0;
            std::string text_1{};
            std::string text_2{};
            std::string text_3{};
            std::string text_4{};
            SColor      color{};

            effects->Get2DFXProperty(model, index, e2dEffectProperty::SIZE, size);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::ROT, rot);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::FLAGS, flags);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::TEXT_1, text_1);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::TEXT_2, text_2);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::TEXT_3, text_3);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::TEXT_4, text_4);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::COLOR, color);

            tbl["size"] = size;
            tbl["rotation"] = rot;
            tbl["flags"] = flags;
            tbl["text1"] = text_1;
            tbl["text2"] = text_2;
            tbl["text3"] = text_3;
            tbl["text4"] = text_4;
            tbl["color"] =
                std::vector<float>{static_cast<float>(color.R), static_cast<float>(color.G), static_cast<float>(color.B), static_cast<float>(color.A)};

            break;
        }
        case e2dEffectType::ESCALATOR:
        {
            std::vector<float> bottom;
            std::vector<float> top;
            std::vector<float> end;
            bottom.reserve(3);
            top.reserve(3);
            end.reserve(3);

            float direction{};

            effects->Get2DFXProperty(model, index, e2dEffectProperty::BOTTOM, bottom);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::TOP, top);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::END, end);
            effects->Get2DFXProperty(model, index, e2dEffectProperty::DIRECTION, direction);

            tbl["bottom"] = bottom;
            tbl["top"] = top;
            tbl["end"] = end;
            tbl["direction"] = direction;

            break;
        }
        default:
            break;
    }

    return tbl;
}

const char* CClient2DFXManager::IsValidEffectData(const e2dEffectType& effectType, const effectPropertiesMap& effectData)
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

            auto*              showMode = MapFind(effectData, "showMode");
            e2dCoronaFlashType tempType;
            if (!showMode || !std::holds_alternative<std::string>(*showMode) || !StringToEnum(std::get<std::string>(*showMode), tempType))
                return "Invalid \"showMode\" value";

            auto* coronaReflection = MapFind(effectData, "coronaReflection");
            if (!coronaReflection || !std::holds_alternative<bool>(*coronaReflection))
                return "Invalid \"coronaReflection\" value";

            auto* coronaFlareType = MapFind(effectData, "flareType");
            if (!coronaFlareType || !std::holds_alternative<float>(*coronaFlareType) ||
                (std::get<float>(*coronaFlareType) < 0.0f || std::get<float>(*coronaFlareType) > 1.0f))
                return "Invalid \"flareType\" value";

            auto* flags = MapFind(effectData, "flags");
            bool  flt = (std::holds_alternative<float>(*flags) && std::get<float>(*flags) >= 0);
            bool  tb = std::holds_alternative<effectFlagsTable>(*flags);
            bool  validFlags = (std::holds_alternative<float>(*flags) && std::get<float>(*flags) >= 0) || std::holds_alternative<effectFlagsTable>(*flags);
            if (!flags || !validFlags)
                return "Invalid \"flags\" value";

            auto* shadowZDistance = MapFind(effectData, "shadowDistance");
            if (!shadowZDistance || (!std::holds_alternative<float>(*shadowZDistance)))
                return "Invalid \"shadowDistance\" value";

            auto* offset = MapFind(effectData, "offset");
            if (!offset || !std::holds_alternative<std::vector<float>>(*offset) || std::get<std::vector<float>>(*offset).size() < 3)
                return "Invalid \"offset\" value";

            auto* color = MapFind(effectData, "color");
            if (!color || !std::holds_alternative<float>(*color))
                return "Invalid \"color\" value";

            auto*                coronaTexture = MapFind(effectData, "coronaName");
            e2dEffectTextureName tempName;
            if (!coronaTexture || !std::holds_alternative<std::string>(*coronaTexture) || !StringToEnum(std::get<std::string>(*coronaTexture), tempName))
                return "Invalid \"coronaName\" value";

            auto*                shadowTexture = MapFind(effectData, "shadowName");
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
            auto* size = MapFind(effectData, "size");
            if (!size || !std::holds_alternative<std::vector<float>>(*size) || std::get<std::vector<float>>(*size).size() < 2)
                return "Invalid \"size\" value";

            auto* rot = MapFind(effectData, "rotation");
            if (!rot || !std::holds_alternative<std::vector<float>>(*rot) || std::get<std::vector<float>>(*rot).size() < 3)
                return "Invalid \"rotation\" value";

            auto* flags = MapFind(effectData, "flags");
            bool  validFlags = (std::holds_alternative<float>(*flags) && std::get<float>(*flags) >= 0) || std::holds_alternative<effectFlagsTable>(*flags);
            if (!flags || !validFlags)
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

            auto* color = MapFind(effectData, "color");
            if (!color || !std::holds_alternative<float>(*color))
                return "Invalid \"color\" value";

            break;
        }
        case e2dEffectType::ESCALATOR:
        {
            auto* bottom = MapFind(effectData, "bottom");
            if (!bottom || !std::holds_alternative<std::vector<float>>(*bottom) || std::get<std::vector<float>>(*bottom).size() < 3)
                return "Invalid \"bottom\" value";

            auto* top = MapFind(effectData, "top");
            if (!top || !std::holds_alternative<std::vector<float>>(*top) || std::get<std::vector<float>>(*top).size() < 3)
                return "Invalid \"top\" value";

            auto* end = MapFind(effectData, "end");
            if (!end || !std::holds_alternative<std::vector<float>>(*end) || std::get<std::vector<float>>(*end).size() < 3)
                return "Invalid \"end\" value";

            auto* direction = MapFind(effectData, "direction");
            if (!direction || !std::holds_alternative<float>(*direction) || (std::get<float>(*direction) < 0.0f || std::get<float>(*direction) > 1.0f))
                return "Invalid \"direction\" value";

            break;
        }
        case e2dEffectType::SUN_GLARE:            // It has no properties
            break;
        // Unnecessary in MTA
        case e2dEffectType::ATTRACTOR:
        case e2dEffectType::FURNITURE:
        case e2dEffectType::ENEX:
        case e2dEffectType::TRIGGER_POINT:
        case e2dEffectType::COVER_POINT:
            break;
    }

    return nullptr;
}

bool CClient2DFXManager::IsValidPropertyValue(const e2dEffectType& effectType, const e2dEffectProperty& property,
                                              std::variant<bool, float, std::string, std::vector<float>, effectFlagsTable> value)
{
    bool isValid = false;
    switch (property)
    {
        case e2dEffectProperty::FAR_CLIP_DISTANCE:
        case e2dEffectProperty::LIGHT_RANGE:
        case e2dEffectProperty::CORONA_SIZE:
        case e2dEffectProperty::SHADOW_SIZE:
        case e2dEffectProperty::SHADOW_MULT:
        case e2dEffectProperty::SHADOW_DISTANCE:
        case e2dEffectProperty::FLARE_TYPE:
        case e2dEffectProperty::COLOR:
        case e2dEffectProperty::DIRECTION:
        {
            isValid = std::holds_alternative<float>(value);
            if (isValid)
            {
                float val = std::get<float>(value);
                if (val < 0.0f && property != e2dEffectProperty::COLOR)
                    isValid = false;

                if (val > 1.0f && (property == e2dEffectProperty::FLARE_TYPE || property == e2dEffectProperty::DIRECTION))
                    isValid = false;
            }
            break;
        }
        case e2dEffectProperty::OFFSET:
        case e2dEffectProperty::SIZE:
        case e2dEffectProperty::ROT:
        case e2dEffectProperty::BOTTOM:
        case e2dEffectProperty::TOP:
        case e2dEffectProperty::END:
        {
            isValid = std::holds_alternative<std::vector<float>>(value);
            if (isValid)
            {
                auto& val = std::get<std::vector<float>>(value);
                if ((property == e2dEffectProperty::SIZE && val.size() < 2) || (property != e2dEffectProperty::SIZE && val.size() < 3))
                    isValid = false;
            }

            break;
        }
        case e2dEffectProperty::CORONA_REFLECTION:
        {
            isValid = std::holds_alternative<bool>(value);
            break;
        }
        case e2dEffectProperty::FLASH_TYPE:
        {
            if (std::holds_alternative<std::string>(value))
            {
                e2dCoronaFlashType tempVal;
                isValid = StringToEnum(std::get<std::string>(value), tempVal);
            }
            break;
        }
        case e2dEffectProperty::CORONA_NAME:
        case e2dEffectProperty::SHADOW_NAME:
        {
            if (std::holds_alternative<std::string>(value))
            {
                e2dEffectTextureName tempVal;
                isValid = StringToEnum(std::get<std::string>(value), tempVal);
            }
            break;
        }
        case e2dEffectProperty::PRT_NAME:
        case e2dEffectProperty::TEXT_1:
        case e2dEffectProperty::TEXT_2:
        case e2dEffectProperty::TEXT_3:
        case e2dEffectProperty::TEXT_4:
        {
            isValid = std::holds_alternative<std::string>(value);
            break;
        }
        case e2dEffectProperty::FLAGS:
        {
            isValid = std::holds_alternative<float>(value) || std::holds_alternative<effectFlagsTable>(value);
            if (isValid)
            {
                if (std::holds_alternative<float>(value) && std::get<float>(value) < 0)
                    isValid = false;

                if (std::holds_alternative<effectFlagsTable>(value))
                {
                    effectFlagsTable tbl = std::get<effectFlagsTable>(value);
                    if (tbl.empty())
                        isValid = false;

                    std::uint16_t temp = 0;
                    if (!InitFlagsFromTable(effectType, tbl, temp))
                        isValid = false;
                }
            }

            break;
        }
    }

    if (isValid)
        isValid = IsValidProperty(effectType, property);

    return isValid;
}

bool CClient2DFXManager::IsValidProperty(const e2dEffectType& effectType, const e2dEffectProperty& property)
{
    switch (property)
    {
        case e2dEffectProperty::FAR_CLIP_DISTANCE:
        case e2dEffectProperty::LIGHT_RANGE:
        case e2dEffectProperty::CORONA_SIZE:
        case e2dEffectProperty::SHADOW_SIZE:
        case e2dEffectProperty::SHADOW_MULT:
        case e2dEffectProperty::FLASH_TYPE:
        case e2dEffectProperty::CORONA_REFLECTION:
        case e2dEffectProperty::FLARE_TYPE:
        case e2dEffectProperty::SHADOW_DISTANCE:
        case e2dEffectProperty::OFFSET:
        case e2dEffectProperty::CORONA_NAME:
        case e2dEffectProperty::SHADOW_NAME:
            return (effectType == e2dEffectType::LIGHT);
        case e2dEffectProperty::COLOR:
        case e2dEffectProperty::FLAGS:
            return (effectType == e2dEffectType::LIGHT || effectType == e2dEffectType::ROADSIGN);
        case e2dEffectProperty::PRT_NAME:
            return (effectType == e2dEffectType::PARTICLE);
        case e2dEffectProperty::SIZE:
        case e2dEffectProperty::ROT:
        case e2dEffectProperty::TEXT_1:
        case e2dEffectProperty::TEXT_2:
        case e2dEffectProperty::TEXT_3:
        case e2dEffectProperty::TEXT_4:
            return (effectType == e2dEffectType::ROADSIGN);
        case e2dEffectProperty::BOTTOM:
        case e2dEffectProperty::TOP:
        case e2dEffectProperty::END:
        case e2dEffectProperty::DIRECTION:
            return (effectType == e2dEffectType::ESCALATOR);
        default:
            return false;
    }
}

bool CClient2DFXManager::IsValidModel(std::uint32_t model)
{
    return CClientObjectManager::IsValidModel(model) || CClientBuildingManager::IsValidModel(model) || CClientVehicleManager::IsValidModel(model) ||
           CClientPlayerManager::IsValidModel(model);
}
