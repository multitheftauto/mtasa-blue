/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/C2DEffectSA.cpp
 *  PURPOSE:     2DFX static class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"
#include "C2DEffectSA.h"
#include "C2DEffectSAInterface.h"
#include "CGameSA.h"

extern CGameSA* pGame;

C2DEffectSA::C2DEffectSA(C2DEffectSAInterface* effectInterface, std::uint32_t modelID) : m_effectInterface(effectInterface), m_model(modelID)
{
    pGame->Get2DEffects()->AddToList(this);
}

void C2DEffectSA::Destroy() const
{
    CModelInfo* modelInfo = pGame->GetModelInfo(m_model);
    if (modelInfo)
        modelInfo->Remove2DFX(m_effectInterface, false);
}

void C2DEffectSA::SetPosition(const CVector& position)
{
    if (m_effectInterface)
        m_effectInterface->position = RwV3d{position.fX, position.fY, position.fZ};
}

CVector& C2DEffectSA::GetPosition() const
{
    if (m_effectInterface)
        return CVector(m_effectInterface->position.x, m_effectInterface->position.y, m_effectInterface->position.z);

    return CVector();
}

void C2DEffectSA::SetCoronaFarClip(float clip)
{
    if (IsValidLight())
        m_effectInterface->effect.light.coronaFarClip = clip;
}

void C2DEffectSA::SetCoronaPointLightRange(float range)
{
    if (IsValidLight())
        m_effectInterface->effect.light.pointLightRange = range;
}

void C2DEffectSA::SetCoronaSize(float size)
{
    if (IsValidLight())
        m_effectInterface->effect.light.coronaSize = size;
}

void C2DEffectSA::SetShadowSize(float size)
{
    if (IsValidLight())
        m_effectInterface->effect.light.shadowSize = size;
}

void C2DEffectSA::SetShadowMultiplier(std::uint8_t multiplier)
{
    if (IsValidLight())
        m_effectInterface->effect.light.shadowColorMultiplier = multiplier;
}

void C2DEffectSA::SetCoronaShowMode(e2dCoronaFlashType showMode)
{
    if (IsValidLight())
        m_effectInterface->effect.light.coronaFlashType = showMode;
}

void C2DEffectSA::SetCoronaReflectionsEnabled(bool enable)
{
    if (IsValidLight())
        m_effectInterface->effect.light.coronaEnableReflection = enable;
}

void C2DEffectSA::SetCoronaFlareType(std::uint8_t flareType)
{
    if (IsValidLight())
        m_effectInterface->effect.light.coronaFlareType = flareType;
}

void C2DEffectSA::SetLightFlags(std::uint16_t flags)
{
    if (IsValidLight())
        m_effectInterface->effect.light.flags = flags;
}

void C2DEffectSA::SetShadowDistance(std::int8_t distance)
{
    if (IsValidLight())
        m_effectInterface->effect.light.shadowZDistance = distance;
}

void C2DEffectSA::SetCoronaOffsets(const CVector& offsets)
{
    if (IsValidLight())
    {
        m_effectInterface->effect.light.offsetX = offsets.fX;
        m_effectInterface->effect.light.offsetY = offsets.fY;
        m_effectInterface->effect.light.offsetZ = offsets.fZ;
    }
}

void C2DEffectSA::SetCoronaColor(const RwColor& color)
{
    if (IsValidLight())
        m_effectInterface->effect.light.color = color;
}

void C2DEffectSA::SetCoronaTexture(const std::string& name)
{
    if (IsValidLight())
    {
        if (m_effectInterface->effect.light.coronaTex)
            RwTextureDestroy(m_effectInterface->effect.light.coronaTex);

        // Call CTxdStore::PushCurrentTxd
        ((void(__cdecl*)())FUNC_PushCurrentTxd)();
        // Call CTxdStore::FindTxdSlot
        int slot = ((int(__cdecl*)(const char*))FUNC_FindTxdSlot)("particle");
        // Call CTxdStore::SetCurrentTxd
        ((void(__cdecl*)(int))FUNC_SetCurrentTxd)(slot);

        m_effectInterface->effect.light.coronaTex = RwReadTexture(name.c_str(), nullptr);

        // Call CTxdStore::PopCurrentTxd
        ((void(__cdecl*)())FUNC_PopCurrentTxd)();
    }
}

void C2DEffectSA::SetShadowTexture(const std::string& name)
{
    if (IsValidLight())
    {
        if (m_effectInterface->effect.light.shadowTex)
            RwTextureDestroy(m_effectInterface->effect.light.shadowTex);

        // Call CTxdStore::PushCurrentTxd
        ((void(__cdecl*)())FUNC_PushCurrentTxd)();
        // Call CTxdStore::FindTxdSlot
        int slot = ((int(__cdecl*)(const char*))FUNC_FindTxdSlot)("particle");
        // Call CTxdStore::SetCurrentTxd
        ((void(__cdecl*)(int))FUNC_SetCurrentTxd)(slot);

        m_effectInterface->effect.light.shadowTex = RwReadTexture(name.c_str(), nullptr);

        // Call CTxdStore::PopCurrentTxd
        ((void(__cdecl*)())FUNC_PopCurrentTxd)();
    }
}

CVector C2DEffectSA::GetCoronaOffsets() const
{
    if (IsValidLight())
        return CVector(m_effectInterface->effect.light.offsetX, m_effectInterface->effect.light.offsetY, m_effectInterface->effect.light.offsetZ);

    return CVector();
}

void C2DEffectSA::SetParticleName(const std::string& name)
{
    if (m_effectInterface && m_effectInterface->type == e2dEffectType::PARTICLE)
        std::strncpy(m_effectInterface->effect.particle.szName, name.c_str(), 24);
}

void C2DEffectSA::SetRoadsignSize(const RwV2d& size)
{
    if (IsValidRoadsign())
        m_effectInterface->effect.roadsign.size = size;
}

void C2DEffectSA::SetRoadsignRotation(const RwV3d& rotation)
{
    if (IsValidRoadsign())
        m_effectInterface->effect.roadsign.rotation = rotation;
}

void C2DEffectSA::SetRoadsignFlags(std::uint8_t flags)
{
    if (IsValidRoadsign())
        m_effectInterface->effect.roadsign.flags = flags;
}

void C2DEffectSA::SetRoadsignText(const std::string& text, std::uint8_t line)
{
    if (IsValidRoadsign())
    {
        if (!m_effectInterface->effect.roadsign.text)
        {
            m_effectInterface->effect.roadsign.text = static_cast<char*>(std::malloc(64));
            MemSetFast(m_effectInterface->effect.roadsign.text, 0, 64);
        }

        if (!m_effectInterface->effect.roadsign.text)
            return;

        std::strncpy(m_effectInterface->effect.roadsign.text + 16 * (line - 1), text.c_str(), 16);
    }
}

RwV2d& C2DEffectSA::GetRoadsignSize()
{
    if (IsValidRoadsign())
        return m_effectInterface->effect.roadsign.size;

    static RwV2d dummySize{0, 0};
    return dummySize;
}

RwV3d& C2DEffectSA::GetRoadsignRotation()
{
    if (IsValidRoadsign())
        return m_effectInterface->effect.roadsign.rotation;

    static RwV3d dummyRotation{0, 0, 0};
    return dummyRotation;
}

std::string C2DEffectSA::GetRoadsignText() const
{
    if (IsValidRoadsign() && m_effectInterface->effect.roadsign.text)
        return std::string(m_effectInterface->effect.roadsign.text, 64);

    return "";
}

void C2DEffectSA::SetEscalatorBottom(const RwV3d& bottom)
{
    if (IsValidEscalator())
        m_effectInterface->effect.escalator.bottom = bottom;
}

void C2DEffectSA::SetEscalatorTop(const RwV3d& top)
{
    if (IsValidEscalator())
        m_effectInterface->effect.escalator.top = top;
}

void C2DEffectSA::SetEscalatorEnd(const RwV3d& end)
{
    if (IsValidEscalator())
        m_effectInterface->effect.escalator.end = end;
}

void C2DEffectSA::SetEscalatorDirection(std::uint8_t direction)
{
    if (IsValidEscalator())
        m_effectInterface->effect.escalator.direction = direction;
}

RwV3d& C2DEffectSA::GetEscalatorBottom()
{
    if (IsValidEscalator())
        return m_effectInterface->effect.escalator.bottom;

    static RwV3d dummyBottom{0, 0, 0};
    return dummyBottom;
}

RwV3d& C2DEffectSA::GetEscalatorTop()
{
    if (IsValidEscalator())
        return m_effectInterface->effect.escalator.top;

    static RwV3d dummyTop{0, 0, 0};
    return dummyTop;
}

RwV3d& C2DEffectSA::GetEscalatorEnd()
{
    if (IsValidEscalator())
        return m_effectInterface->effect.escalator.end;

    static RwV3d dummyEnd{0, 0, 0};
    return dummyEnd;
}

RpAtomic* C2DEffectSA::Roadsign_CreateAtomic(const RwV3d& position, const RwV3d& rotation, float sizeX, float sizeY, std::uint32_t numLines, char* line1, char* line2, char* line3, char* line4, std::uint32_t numLetters, std::uint8_t palleteID)
{
    // Call CCustomRoadsignMgr::CreateRoadsignAtomic
    RpAtomic* atomic = ((RpAtomic*(__cdecl*)(float, float, std::int32_t, char*, char*, char*, char*, std::int32_t, std::uint8_t))0x6FF2D0)(sizeX, sizeY, numLines, line1, line2, line3, line4, numLetters, palleteID);
    RwFrame* frame = RpAtomicGetFrame(atomic);
    RwFrameSetIdentity(frame);

    const RwV3d axis0{1.0f, 0.0f, 0.0f}, axis1{0.0f, 1.0f, 0.0f}, axis2{0.0f, 0.0f, 1.0f};
    RwFrameRotate(frame, &axis2, rotation.z, rwCOMBINEREPLACE);
    RwFrameRotate(frame, &axis0, rotation.x, rwCOMBINEPOSTCONCAT);
    RwFrameRotate(frame, &axis1, rotation.y, rwCOMBINEPOSTCONCAT);

    RwFrameTranslate(frame, &position, TRANSFORM_AFTER);
    RwFrameUpdateObjects(frame);

    return atomic;
}

std::uint32_t C2DEffectSA::Roadsign_GetPalleteIDFromFlags(std::uint8_t flags)
{
    std::uint32_t id = (flags >> 4) & 3;
    return id <= 3 ? id : 0;
}

std::uint32_t C2DEffectSA::Roadsign_GetNumLettersFromFlags(std::uint8_t flags)
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

std::uint32_t C2DEffectSA::Roadsign_GetNumLinesFromFlags(std::uint8_t flags)
{
    std::uint32_t lines = flags & 3;
    return (lines <= 3 && lines > 0) ? lines : 4;
}

void C2DEffectSA::Roadsign_DestroyAtomic(C2DEffectSAInterface* effect)
{
    if (!effect)
        return;

    t2dEffectRoadsign& roadsign = effect->effect.roadsign;
    if (roadsign.atomic)
    {
        RwFrame* frame = RpAtomicGetFrame(roadsign.atomic);
        if (frame)
        {
            RpAtomicSetFrame(roadsign.atomic, nullptr);
            RwFrameDestroy(frame);
        }

        RpAtomicDestroy(roadsign.atomic);
        roadsign.atomic = nullptr;
    }
}

C2DEffectSAInterface* C2DEffectSA::CreateCopy(C2DEffectSAInterface* effect)
{
    C2DEffectSAInterface* copy = new C2DEffectSAInterface();
    MemCpyFast(copy, effect, sizeof(C2DEffectSAInterface));

    // Create a copy of textures for the lights
    // We must to do this, because C2DEffect::Shutdown removes them
    if (copy->type == e2dEffectType::LIGHT)
    {
        if (effect->effect.light.coronaTex && effect->effect.light.shadowTex)
            C2DEffectSA::PrepareTexturesForLightEffect(copy->effect.light.coronaTex, copy->effect.light.shadowTex, effect->effect.light.coronaTex->name, effect->effect.light.shadowTex->name, false);
    }
    else if (copy->type == e2dEffectType::ROADSIGN)
    {
        // Create a copy of text and atomic for the roadsign
        // We must to do this, because C2DEffect::Shutdown removes them
        copy->effect.roadsign.text = static_cast<char*>(std::malloc(64));
        if (copy->effect.roadsign.text)
        {
            MemSetFast(copy->effect.roadsign.text, 0, 64);
            MemCpyFast(copy->effect.roadsign.text, effect->effect.roadsign.text, 64);

            copy->effect.roadsign.atomic = Roadsign_CreateAtomic(copy->position, copy->effect.roadsign.rotation, copy->effect.roadsign.size.x, copy->effect.roadsign.size.y, Roadsign_GetNumLinesFromFlags(copy->effect.roadsign.flags), &copy->effect.roadsign.text[0], &copy->effect.roadsign.text[16], &copy->effect.roadsign.text[32], &copy->effect.roadsign.text[48], Roadsign_GetNumLettersFromFlags(copy->effect.roadsign.flags), Roadsign_GetPalleteIDFromFlags(copy->effect.roadsign.flags));
        }
    }

    return copy;
}

void C2DEffectSA::Shutdown(C2DEffectSAInterface* effect)
{
    if (!effect)
        return;

    if (effect->type == e2dEffectType::ROADSIGN)
    {
        t2dEffectRoadsign& roadsign = effect->effect.roadsign;
        Roadsign_DestroyAtomic(effect);

        if (roadsign.text)
        {
            std::free(roadsign.text);
            roadsign.text = nullptr;
        }
    }
    else if (effect->type == e2dEffectType::LIGHT)
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
    }
}

void C2DEffectSA::SafeDelete2DFXEffect(C2DEffectSAInterface* effect)
{
    if (!effect)
        return;

    if (effect->type == e2dEffectType::ROADSIGN || effect->type == e2dEffectType::LIGHT)
        Shutdown(effect);

    delete effect;
    effect = nullptr;
}

void C2DEffectSA::PrepareTexturesForLightEffect(RwTexture*& coronaTex, RwTexture*& shadowTex, const char* coronaName, const char* shadowName, bool removeIfExist)
{
    // Call CTxdStore::PushCurrentTxd
    ((void(__cdecl*)())FUNC_PushCurrentTxd)();
    // Call CTxdStore::FindTxdSlot
    int slot = ((int(__cdecl*)(const char*))FUNC_FindTxdSlot)("particle");
    // Call CTxdStore::SetCurrentTxd
    ((void(__cdecl*)(int))FUNC_SetCurrentTxd)(slot);

    if (removeIfExist)
    {
        if (coronaTex && coronaName)
            RwTextureDestroy(coronaTex);
        if (shadowTex && shadowName)
            RwTextureDestroy(shadowTex);
    }

    if (coronaName)
        coronaTex = RwReadTexture(coronaName, nullptr);

    if (shadowName)
        shadowTex = RwReadTexture(shadowName, nullptr);

    // Call CTxdStore::PopCurrentTxd
    ((void(__cdecl*)())FUNC_PopCurrentTxd)();
}
