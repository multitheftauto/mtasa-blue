/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFxSystemSA.cpp
 *  PURPOSE:     Game effects handling
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <optional>
#include <CMatrix.h>
#include <game/CVehicle.h>
#include "CFxSystemBPSA.h"
#include "CFxSystemSA.h"
#include "CGameSA.h"

extern CGameSA* pGame;

// CRC-32/JAMCRC of the uppercased blueprint name "NITRO" (see CKeyGen::GetUppercaseKey)
static constexpr uint32_t FX_BLUEPRINT_HASH_NITRO = 0x3D591CC6;

// One animated colour block ("FX_INFO_COLOUR_DATA"/"FX_INFO_COLOURBRIGHT_DATA"/
// "FX_INFO_COLOURRANGE_DATA") of the "nitro" blueprint's R/G/B(/Range) channels, plus their
// original (unmodified) keyframe values so a per-vehicle tint can be re-derived from scratch
// every time a different vehicle's nitro particles are about to be rendered.
struct SNitroColorBlock
{
    uint8_t nNumKeyframes;
    uint8_t nStride;  // 1 = single value per component, 2 = value + randomisation range per component

    // Indexed by [component (0=R, 1=G, 2=B)][stride slot (0=value, 1=range)]; null if the
    // blueprint has no such channel.
    uint16_t*             pChannelValues[3][2]{};
    std::vector<uint16_t> originalValues[3][2];
};

static std::vector<SNitroColorBlock> ms_NitroColorBlocks;
static bool                          ms_bNitroColorChannelsCached = false;
static CFxSystemSAInterface*         ms_pLastNitroFxSystem = nullptr;
static std::optional<SColor>         ms_LastAppliedNitroColor;

//////////////////////////////////////////////////////////////////////////////////////////
//
// CacheNitroColorChannels
//
// Collects every R/G/B(/Range) keyframe channel of the "nitro" blueprint's colour data, so
// that ApplyNitroColor can later re-tint or restore them on a per-vehicle basis without
// losing precision.
//
//////////////////////////////////////////////////////////////////////////////////////////
static void CacheNitroColorChannels(CFxSystemBPSAInterface* pBlueprint)
{
    if (ms_bNitroColorChannelsCached)
        return;

    for (uint8_t i = 0; i < (uint8_t)pBlueprint->cNumOfPrims; ++i)
    {
        CFxEmitterBPSAInterface* pEmitterBP = (CFxEmitterBPSAInterface*)pBlueprint->pPrims[i];
        for (uint32_t j = 0; j < pEmitterBP->m_infoManager.m_nNumInfos; ++j)
        {
            FxInfoSAInterface* pInfo = pEmitterBP->m_infoManager.m_pInfos[j];

            uint8_t stride = 0;
            if (pInfo->nType == FX_INFO_COLOUR_DATA || pInfo->nType == FX_INFO_COLOURBRIGHT_DATA)
                stride = 1;
            else if (pInfo->nType == FX_INFO_COLOURRANGE_DATA)
                stride = 2;

            if (stride == 0)
                continue;

            // Each colour component spans `stride` consecutive channels (e.g. ColourRange
            // stores R, RRange, G, GRange, B, BRange).
            FxInfoColorSAInterface* pColorInfo = (FxInfoColorSAInterface*)pInfo;

            SNitroColorBlock block;
            block.nNumKeyframes = (uint8_t)pColorInfo->nNumKeyframes;
            block.nStride = stride;

            for (int component = 0; component < 3; ++component)
            {
                for (int s = 0; s < stride; ++s)
                {
                    int channel = component * stride + s;
                    if (channel >= pColorInfo->nNumChannels)
                        break;

                    uint16_t* pValues = pColorInfo->ppChannelValues[channel];
                    block.pChannelValues[component][s] = pValues;
                    block.originalValues[component][s].assign(pValues, pValues + block.nNumKeyframes);
                }
            }
            ms_NitroColorBlocks.push_back(std::move(block));
        }
    }
    ms_bNitroColorChannelsCached = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// ApplyNitroColor
//
// If `color` has no value, restores every cached "nitro" colour channel to its original
// keyframe values (i.e. the game's default cyan effect).
//
// Otherwise, re-tints every cached channel using `color`. Rather than multiplying the
// original (cyan) values by the requested colour - which would mix the two hues together -
// the brightest of a block's original R/G/B values at each keyframe is used as a brightness
// envelope, and every channel is recoloured from scratch using that envelope. This allows
// any colour, including white, to fully replace the original hue.
//
// Always derived from the cached originals, so it can be called repeatedly for different
// vehicles without ever needing to restore the blueprint first.
//
//////////////////////////////////////////////////////////////////////////////////////////
static void ApplyNitroColor(const std::optional<SColor>& color)
{
    for (auto& block : ms_NitroColorBlocks)
    {
        if (!color.has_value())
        {
            for (int component = 0; component < 3; ++component)
            {
                for (int s = 0; s < block.nStride; ++s)
                {
                    if (uint16_t* pValues = block.pChannelValues[component][s])
                        std::copy(block.originalValues[component][s].begin(), block.originalValues[component][s].end(), pValues);
                }
            }
            continue;
        }

        const uint32_t components[3] = {color->R, color->G, color->B};

        for (uint8_t keyframe = 0; keyframe < block.nNumKeyframes; ++keyframe)
        {
            uint16_t brightness = 0;
            for (int component = 0; component < 3; ++component)
                brightness = std::max(brightness, block.originalValues[component][0][keyframe]);

            for (int component = 0; component < 3; ++component)
            {
                for (int s = 0; s < block.nStride; ++s)
                {
                    uint16_t* pValues = block.pChannelValues[component][s];
                    if (!pValues)
                        continue;

                    const uint16_t source = (s == 0) ? brightness : block.originalValues[component][s][keyframe];
                    pValues[keyframe] = (uint16_t)(source * components[component] / 255);
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// FxEmitterBP_c::Render
//
// Called once per particle while the "nitro" blueprint's particles are rendered.
// Re-tints the shared blueprint's colour data with the owning vehicle's nitro colour
// just before its colour keyframes are sampled.
//
//////////////////////////////////////////////////////////////////////////////////////////
__declspec(noinline) static void OnFxParticleProcessRenderInfo(CFxParticleSAInterface* pParticle)
{
    CFxSystemSAInterface* pFxSystem = pParticle->pSystem;
    if (!pFxSystem || !pFxSystem->pBlueprint || pFxSystem->pBlueprint->uiNameHash != FX_BLUEPRINT_HASH_NITRO)
        return;

    CVehicle*                   pVehicle = pGame->GetFxManagerSA()->GetVehicleFromNitroSystem(pFxSystem);
    const std::optional<SColor> color = pVehicle ? pVehicle->GetNitroColor() : std::nullopt;

    if (pFxSystem == ms_pLastNitroFxSystem && color == ms_LastAppliedNitroColor)
        return;

    CacheNitroColorChannels(pFxSystem->pBlueprint);
    ApplyNitroColor(color);

    ms_pLastNitroFxSystem = pFxSystem;
    ms_LastAppliedNitroColor = color;
}

// Hook info
// Redirects the CALL that initialises a particle's default render colour (at the start of
// FxEmitterBP_c::Render's per-particle loop) through our hook first.
// At this call site: EBX = current particle (CFxParticleSAInterface*, callee-saved).
#define HOOKPOS_FxEmitterBP_c_Render_ProcessRenderInfo 0x4A2E31
DWORD                         ORIGINAL_FxEmitterBP_c_Render_ProcessRenderInfo = 0x4A4A80;
static void __declspec(naked) HOOK_FxEmitterBP_c_Render_ProcessRenderInfo()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    ebx
        call    OnFxParticleProcessRenderInfo
        add     esp, 4
        popad

        jmp     ORIGINAL_FxEmitterBP_c_Render_ProcessRenderInfo
    }
    // clang-format on
}

// Variables used in the hooks
static CFxSystemSAInterface*  ms_pUsingFxSystemSAInterface = NULL;
static float                  ms_fUsingDrawDistance = 0;
static ushort                 ms_usFxSystemSavedCullDistance = 0;
static CFxEmitterSAInterface* ms_pUsingFxEmitterSAInterface = NULL;
static ushort                 ms_usFxEmitterSavedFadeFarDistance = 0;
static ushort                 ms_usFxEmitterSavedFadeNearDistance = 0;
static float                  ms_fFxSystemUpdateCullDistMultiplier = FX_SYSTEM_UPDATE_CULL_DIST_MULTIPLIER_DEFAULT;
static float                  ms_fFxCreateParticleCullDistMultiplier = FX_CREATE_PARTICLE_CULL_DIST_MULTIPLIER_DEFAULT;

CFxSystemSA::CFxSystemSA(CFxSystemSAInterface* pInterface)
{
    m_pInterface = pInterface;
    m_fDrawDistance = 0;
    pGame->GetFxManagerSA()->AddToList(m_pInterface, this);
}

CFxSystemSA::~CFxSystemSA()
{
    pGame->GetFxManagerSA()->RemoveFromList(this);
}

void CFxSystemSA::PlayAndKill()
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_FxSystem_c__PlayAndKill;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
    // clang-format on
}

void CFxSystemSA::GetMatrix(CMatrix& matrix)
{
    MemCpyFast(&matrix.vFront, &m_pInterface->matPosition.at, sizeof(CVector));
    MemCpyFast(&matrix.vPos, &m_pInterface->matPosition.pos, sizeof(CVector));
    MemCpyFast(&matrix.vUp, &m_pInterface->matPosition.up, sizeof(CVector));
    MemCpyFast(&matrix.vRight, &m_pInterface->matPosition.right, sizeof(CVector));
}

void CFxSystemSA::SetMatrix(const CMatrix& matrix)
{
    MemCpyFast(&m_pInterface->matPosition.at, &matrix.vFront, sizeof(CVector));
    MemCpyFast(&m_pInterface->matPosition.pos, &matrix.vPos, sizeof(CVector));
    MemCpyFast(&m_pInterface->matPosition.up, &matrix.vUp, sizeof(CVector));
    MemCpyFast(&m_pInterface->matPosition.right, &matrix.vRight, sizeof(CVector));
}

void CFxSystemSA::GetPosition(CVector& vecPos)
{
    vecPos.fX = m_pInterface->matPosition.pos.x;
    vecPos.fY = m_pInterface->matPosition.pos.y;
    vecPos.fZ = m_pInterface->matPosition.pos.z;
}

void CFxSystemSA::SetPosition(const CVector& vecPos)
{
    SetPosition(m_pInterface, vecPos);
}

void CFxSystemSA::SetPosition(CFxSystemSAInterface* fxSystem, const CVector& position)
{
    fxSystem->matPosition.pos.x = position.fX;
    fxSystem->matPosition.pos.y = position.fY;
    fxSystem->matPosition.pos.z = position.fZ;

    // Set the update flag(s)
    // this is what RwMatrixUpdate (@0x7F18E0) does
    fxSystem->matPosition.flags &= 0xFFFDFFFC;
}

float CFxSystemSA::GetEffectDensity()
{
    return m_pInterface->sRateMult / 1000.0f;
}

void CFxSystemSA::SetEffectDensity(float fDensity)
{
    m_pInterface->sRateMult = (short)(fDensity * 1000.0f);
}

float CFxSystemSA::GetEffectSpeed()
{
    return m_pInterface->sTimeMult / 1000.0f;
}

void CFxSystemSA::SetEffectSpeed(float fSpeed)
{
    m_pInterface->sTimeMult = (short)(fSpeed * 1000.0f);
}

void CFxSystemSA::SetDrawDistance(float fDrawDistance)
{
    m_fDrawDistance = Clamp(0.f, fDrawDistance, 8191.f);
}

float CFxSystemSA::GetDrawDistance()
{
    if (HasCustomDrawDistance())
        return m_fDrawDistance;
    else
        return m_pInterface->pBlueprint->usCullDistance / 256.f;
}

bool CFxSystemSA::HasCustomDrawDistance()
{
    return m_fDrawDistance != 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// FxSystem_c::Update_MidA
//
// Poke new draw distance for the FxSystem
//
//////////////////////////////////////////////////////////////////////////////////////////
__declspec(noinline) void OnMY_FxSystem_c_Update_MidA_Pre(CFxSystemSAInterface* pFxSystemSAInterface)
{
    CFxSystemSA* pFxSystemSA = pGame->GetFxManagerSA()->GetFxSystem(pFxSystemSAInterface);
    if (pFxSystemSA && pFxSystemSA->HasCustomDrawDistance())
    {
        ms_pUsingFxSystemSAInterface = pFxSystemSAInterface;
        ms_usFxSystemSavedCullDistance = pFxSystemSAInterface->pBlueprint->usCullDistance;
        ms_fUsingDrawDistance = pFxSystemSA->GetDrawDistance();
        pFxSystemSAInterface->pBlueprint->usCullDistance = (ushort)(ms_fUsingDrawDistance * 8);
        ms_fFxSystemUpdateCullDistMultiplier = 1 / 8.f;
    }
    else
        ms_fUsingDrawDistance = 0;
}

__declspec(noinline) void OnMY_FxSystem_c_Update_MidA_Post()
{
    if (ms_pUsingFxSystemSAInterface)
    {
        // Restore default settings
        ms_pUsingFxSystemSAInterface->pBlueprint->usCullDistance = ms_usFxSystemSavedCullDistance;
        ms_fUsingDrawDistance = 0;
        ms_pUsingFxSystemSAInterface = NULL;
        ms_fFxSystemUpdateCullDistMultiplier = FX_SYSTEM_UPDATE_CULL_DIST_MULTIPLIER_DEFAULT;
    }
}

// Hook info
#define HOOKCHECK_FxSystem_c_Update_MidA 0x83
#define HOOKPOS_FxSystem_c_Update_MidA   0x04AAF70
#define HOOKSIZE_FxSystem_c_Update_MidA  5
DWORD                         RETURN_FxSystem_c_Update_MidA = 0x04AAF75;
static void __declspec(naked) HOOK_FxSystem_c_Update_MidA()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    ecx
        call    OnMY_FxSystem_c_Update_MidA_Pre
        add     esp, 4*1
        popad

        push    [esp+4*2]
        push    [esp+4*2]
        call inner

        pushad
        call    OnMY_FxSystem_c_Update_MidA_Post
        popad
        retn    8

inner:
        sub     esp, 10h
        push    ebx
        push    ebp
        jmp     RETURN_FxSystem_c_Update_MidA
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// FxSystem_c::Update_MidB
//
// Poke new draw distances for each emitter
//
//////////////////////////////////////////////////////////////////////////////////////////
__declspec(noinline) void OnMY_FxSystem_c_Update_MidB_Pre(CFxEmitterSAInterface* pFxEmitterSAInterface)
{
    if (ms_fUsingDrawDistance)
    {
        dassert(pFxEmitterSAInterface->vtbl == (void*)0x085A7A4);
        ms_pUsingFxEmitterSAInterface = pFxEmitterSAInterface;

        ms_usFxEmitterSavedFadeFarDistance = pFxEmitterSAInterface->pBlueprint->m_nLodEnd;
        ms_usFxEmitterSavedFadeNearDistance = pFxEmitterSAInterface->pBlueprint->m_nLodStart;

        pFxEmitterSAInterface->pBlueprint->m_nLodEnd = (ushort)(ms_fUsingDrawDistance * 8);
        float fNearDistanceRatio = ms_usFxEmitterSavedFadeNearDistance / (float)ms_usFxEmitterSavedFadeFarDistance;
        pFxEmitterSAInterface->pBlueprint->m_nLodStart = (ushort)(ms_fUsingDrawDistance * fNearDistanceRatio * 8);
        ms_fFxCreateParticleCullDistMultiplier = 1 / 8.f;
    }
}

__declspec(noinline) void OnMY_FxSystem_c_Update_MidB_Post()
{
    if (ms_pUsingFxEmitterSAInterface)
    {
        // Restore default settings
        ms_pUsingFxEmitterSAInterface->pBlueprint->m_nLodEnd = ms_usFxEmitterSavedFadeFarDistance;
        ms_pUsingFxEmitterSAInterface->pBlueprint->m_nLodStart = ms_usFxEmitterSavedFadeNearDistance;
        ms_pUsingFxEmitterSAInterface = NULL;
        ms_fFxCreateParticleCullDistMultiplier = FX_CREATE_PARTICLE_CULL_DIST_MULTIPLIER_DEFAULT;
    }
}

// Hook info
#define HOOKCHECK_FxSystem_c_Update_MidB 0x83
#define HOOKPOS_FxSystem_c_Update_MidB   0x04AB21D
#define HOOKSIZE_FxSystem_c_Update_MidB  7
DWORD                         RETURN_FxSystem_c_Update_MidB = 0x04AB224;
static void __declspec(naked) HOOK_FxSystem_c_Update_MidB()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    ecx
        call    OnMY_FxSystem_c_Update_MidB_Pre
        add     esp, 4*1
        popad

        mov     eax, [ebp+54h]
        push    eax
        call    dword ptr [edx+8]   // FxEmitter_c::CreateParticles

        pushad
        call    OnMY_FxSystem_c_Update_MidB_Post
        popad

        jmp     RETURN_FxSystem_c_Update_MidB
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CFxSystemSA::StaticSetHooks
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CFxSystemSA::StaticSetHooks()
{
    EZHookInstall(FxSystem_c_Update_MidA);
    EZHookInstall(FxSystem_c_Update_MidB);

    // Redirect the per-particle render info CALL in FxEmitterBP_c::Render so we can apply
    // per-vehicle nitro colours just before they are sampled
    HookInstallCall(HOOKPOS_FxEmitterBP_c_Render_ProcessRenderInfo, (DWORD)HOOK_FxEmitterBP_c_Render_ProcessRenderInfo);

    // Redirect these constants so we can change them
    MemPut<float*>(VAR_FxSystemUpdateCullDistMultiplier, &ms_fFxSystemUpdateCullDistMultiplier);
    MemPut<float*>(VAR_FxCreateParticleCullDistMultiplierA, &ms_fFxCreateParticleCullDistMultiplier);
    MemPut<float*>(VAR_FxCreateParticleCullDistMultiplierB, &ms_fFxCreateParticleCullDistMultiplier);
}
