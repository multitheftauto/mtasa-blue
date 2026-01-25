/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPointLightsSA.cpp
 *  PURPOSE:     PointLights entity
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPointLightsSA.h"
#include "CEntitySA.h"

using CHeli_SearchLightCone_t = void(__cdecl*)(int handleId, CVector startPos, CVector endPos, float radius1, float unknownConstant, int unkown1,
                                               bool renderSpot, CVector* unkown3, CVector* unkown4, CVector* unknown5, int unknown6, float radius2);
using CHeli_PreSearchLightCone_t = int(__cdecl*)();
using CHeli_PostSearchLightCone_t = int(__cdecl*)();

static SColor searchLightColor{};

void CPointLightsSA::AddLight(int iMode, const CVector vecPosition, CVector vecDirection, float fRadius, SharedUtil::SColor color, unsigned char uc_8,
                              bool bCreatesShadow, CEntity* pAffected)
{
    DWORD dwEntityInterface = 0;
    if (pAffected)
        dwEntityInterface = (DWORD)pAffected->GetInterface();
    DWORD dwFunc = FUNC_CPointLights_AddLight;
    float fPosX = vecPosition.fX, fPosY = vecPosition.fY, fPosZ = vecPosition.fZ;
    float fDirX = vecDirection.fX, fDirY = vecDirection.fY, fDirZ = vecDirection.fZ;
    float fRed = (float)color.R / 255, fGreen = (float)color.G / 255, fBlue = (float)color.B / 255;
    // clang-format off
    __asm
    {
        push    dwEntityInterface
        push    bCreatesShadow
        push    uc_8
        push    fBlue
        push    fGreen
        push    fRed
        push    fRadius
        push    fDirZ
        push    fDirY
        push    fDirX
        push    fPosZ
        push    fPosY
        push    fPosX
        push    iMode
        call    dwFunc
        add     esp, 56
    }
    // clang-format on
}

void CPointLightsSA::PreRenderHeliLights()
{
    auto CHeli_PreSearchLightCone = (CHeli_PreSearchLightCone_t)FUNC_CHeli_Pre_SearchLightCone;
    CHeli_PreSearchLightCone();
}

void CPointLightsSA::PostRenderHeliLights()
{
    auto CHeli_PostSearchLightCone = (CHeli_PostSearchLightCone_t)FUNC_CHeli_Post_SearchLightCone;
    CHeli_PostSearchLightCone();
}

void CPointLightsSA::RenderHeliLight(const CVector& vecStart, const CVector& vecEnd, float startRadius, float endRadius, bool renderSpot, const SharedUtil::SColor& color)
{
    auto CHeli_SearchLightCone = (CHeli_SearchLightCone_t)FUNC_CHeli_SearchLightCone;

    // 3x3 translation matrix (initialised by the game)
    CVector mat[] = {CVector(), CVector(), CVector()};

    // Set color
    searchLightColor = color;

    // Set render states and render
    CHeli_SearchLightCone(0, vecStart, vecEnd, endRadius, 1.0f, 0, renderSpot, &mat[0], &mat[1], &mat[2], 1, startRadius);
}

struct RxObjSpace3DVertex
{
    CVector pos;
    CVector normal;
    int     color;
    int     u;
    int     v;
};

static void __cdecl PreRenderSearchLight3DVertexBuffer(RxObjSpace3DVertex* buffer, int numVerts, RwMatrix* ltm, int flags)
{
    // Light cone color
    if (buffer && numVerts > 0)
    {
        for (int i = 0; i < numVerts; i++)
        {
            std::uint8_t r = (buffer[i].color >> 16) & 0xFF;
            std::uint8_t g = (buffer[i].color >> 8) & 0xFF;
            std::uint8_t b = (buffer[i].color >> 0) & 0xFF;

            float intensity = (buffer[i].color & 0xFF) / 255.0f; // Get original intensity

            buffer[i].color = COLOR_RGBA(static_cast<std::uint8_t>(searchLightColor.R * intensity), static_cast<std::uint8_t>(searchLightColor.G * intensity), static_cast<std::uint8_t>(searchLightColor.B * intensity), 0);
        }
    }
}

static constexpr std::uintptr_t HOOK_CSearchLight_PreRender3DVertexBuffer = 0x6C6293;
static constexpr std::uintptr_t TansformFunc = 0x7EF450;
static void __declspec(naked) CSearchLight_PreRender3DVertexBuffer()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        call PreRenderSearchLight3DVertexBuffer
        call TansformFunc

        jmp HOOK_CSearchLight_PreRender3DVertexBuffer
    }
    // clang-format on
}

static void __cdecl RenderSearchLightShadow(char type, void* texture, CVector* pos, float x1, float y1, float x2, float y2, std::int16_t intensity, char r, char g, char b, float zDistance, bool drawOnWater, float scale, void* shadowData, bool drawOnBuildings)
{
    // Get original color intensity multiplier
    float colorIntensity = static_cast<float>(intensity) / 128.0f;

    // CShadows::StoreShadowToBeRendered
    ((void(__cdecl*)(char, void*, CVector*, float, float, float, float, std::int16_t, unsigned char, unsigned char, unsigned char, float, bool, float, void*, bool))0x707390)(
        type, texture, pos, x1, y1, x2, y2, intensity, static_cast<unsigned char>(searchLightColor.R * colorIntensity), static_cast<unsigned char>(searchLightColor.G * colorIntensity),
        static_cast<unsigned char>(searchLightColor.B * colorIntensity), zDistance, drawOnWater, scale, shadowData, drawOnBuildings);
}

static constexpr std::uintptr_t RETURN_CSearchLight_RenderShadow = 0x6C64FC;
static void __declspec(naked) CSearchLight_RenderShadow()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        call RenderSearchLightShadow
        jmp RETURN_CSearchLight_RenderShadow
    }
    // clang-format on
}

static void __cdecl RenderSearchLightCorona(int ID, void* entity, unsigned char r, unsigned char g, unsigned char b, unsigned char a, CVector* pos, float radius, float farClip, int type, char flareType, bool enableReflection, bool checkObstacles, int unknownUnused, float angle, bool longDistance, float nearClip, bool fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay)
{
    // CCoronas::RegisterCorona
    ((void(__cdecl*)(int, void*, unsigned char, unsigned char, unsigned char, unsigned char, CVector*, float, float, int, char, bool, bool, int, float, bool,
                     float, bool, float, bool, bool))0x6FC580)(ID, entity, searchLightColor.R, searchLightColor.G, searchLightColor.B, a, pos, radius, farClip,
                                                               type, flareType, enableReflection, checkObstacles, unknownUnused, angle, longDistance, nearClip,
                                                               fadeState, fadeSpeed, onlyFromBelow, reflectionDelay);
}

static constexpr std::uintptr_t RETURN_CSearchLight_RenderCorona = 0x6C5B24;
static void _declspec(naked)    CSearchLight_RenderCorona()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        call RenderSearchLightCorona
        jmp RETURN_CSearchLight_RenderCorona
    }
    // clang-format on
}

void CPointLightsSA::StaticSetHooks()
{
    HookInstall(0x6C628E, (DWORD)CSearchLight_PreRender3DVertexBuffer);
    HookInstall(0x6C64F7, (DWORD)CSearchLight_RenderShadow);
    HookInstall(0x6C5B1F, (DWORD)CSearchLight_RenderCorona);
}
