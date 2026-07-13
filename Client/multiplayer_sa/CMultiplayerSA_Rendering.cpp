/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_Rendering.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/RenderWare.h>
extern CCoreInterface*           g_pCore;
GameEntityRenderHandler*         pGameEntityRenderHandler = nullptr;
PreRenderSkyHandler*             pPreRenderSkyHandlerHandler = nullptr;
RenderHeliLightHandler*          pRenderHeliLightHandler = nullptr;
RenderEverythingBarRoadsHandler* pRenderEverythingBarRoadsHandler = nullptr;

#define VAR_CCullZones_NumMirrorAttributeZones 0x0C87AC4  // int
#define VAR_CMirrors_d3dRestored               0x0C7C729  // uchar

namespace
{
    CEntitySAInterface* ms_Rendering = NULL;
    CEntitySAInterface* ms_RenderingOneNonRoad = NULL;
    bool                ms_bIsMinimizedAndNotConnected = false;
    int                 ms_iSavedNumMirrorZones = 0;
}  // namespace

// === Raise the visible-entity / visible-LOD render list capacity =============
// GTA's render lists (CRenderer::ms_aVisibleEntityPtrs @ 0xB75898,
// ms_aVisibleLodPtrs @ 0xB748F8) are fixed 1000-entry arrays; the two
// Check_NoOfVisible* hooks clamp the counters at 999. When more than ~1000
// entities are visible (dense custom maps once the object streaming limits are
// raised, see engineSetObjectStreamingLimits), the engine drops the excess and,
// because which entities get dropped shifts per frame, the world flickers.
// We relocate both arrays to larger buffers and raise the clamp. This is inert
// with the default limits (a scene rarely exceeds 1000 visible entities), so it
// does not change stock behaviour. Related: issues #869, #5054.
#define RENDERLIST_LIMIT 10000           // capacity (stock game: 1000)
static void* g_relocatedVisibleEntityPtrs[RENDERLIST_LIMIT] = {};
static void* g_relocatedVisibleLodPtrs[RENDERLIST_LIMIT] = {};

//////////////////////////////////////////////////////////////////////////////////////////
//
// CallIdle
//
// Profile call to function 'Idle'
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CallIdle_Pre(DWORD calledFrom)
{
    TIMING_CHECKPOINT("+CallIdle1");
}

void OnMY_CallIdle_Post(RwGeometry* pGeometry, DWORD calledFrom)
{
    TIMING_CHECKPOINT("-CallIdle2");
}

// Hook info
#define HOOKPOS_CallIdle  0x53ECBD
#define HOOKSIZE_CallIdle 5
DWORD                         RETURN_CallIdle = 0x53ECC2;
DWORD                         DO_CallIdle = 0x53E920;
static void __declspec(naked) HOOK_CallIdle()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        call    OnMY_CallIdle_Pre
        popad

        push    [esp+4*1]
        call    DO_CallIdle
        add     esp, 4*1

        pushad
        call    OnMY_CallIdle_Post
        popad

        jmp     RETURN_CallIdle
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// IsEntityRenderable
//
// Return false if should not render
//
//////////////////////////////////////////////////////////////////////////////////////////
bool IsEntityRenderable(CEntitySAInterface* pEntity)
{
    bool bIsPlaceable = pEntity->IsPlaceableVTBL();
    bool bHasRwObject = (pEntity->m_pRwObject != nullptr);
    if (bIsPlaceable || !bHasRwObject)
    {
        AddReportLog(8645, SString("Error in render list: IsPlaceable:%d HasRwObject:%d", bIsPlaceable, bHasRwObject));
    }
    return !bIsPlaceable && bHasRwObject;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CEntity::Render
//
// Detect entity rendering
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CEntity_Render_Pre(CEntitySAInterface* pEntity)
{
    ms_Rendering = pEntity;

    if (ms_Rendering)
        CallGameEntityRenderHandler(ms_Rendering);
}

void OnMY_CEntity_Render_Post()
{
    if (ms_Rendering)
    {
        ms_Rendering = NULL;
        CallGameEntityRenderHandler(ms_RenderingOneNonRoad);  // restore value set in RenderOneNonRoad
    }
}

// Hook info
#define HOOKPOS_CEntity_Render  0x534310
#define HOOKSIZE_CEntity_Render 6
DWORD                         RETURN_CEntity_Render = 0x534317;
static void __declspec(naked) HOOK_CEntity_Render()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    ecx
        call    OnMY_CEntity_Render_Pre
        add     esp, 4*1
        popad

        call inner

        pushad
        call    OnMY_CEntity_Render_Post
        popad
        retn

inner:
        push    ecx
        push    esi
        mov     esi,ecx
        mov     eax,dword ptr [esi+18h]
        jmp     RETURN_CEntity_Render
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CEntity::RenderOneNonRoad
//
// Detect entity rendering
//
//////////////////////////////////////////////////////////////////////////////////////////

// #425: the game sets ALPHAREF before the entity render call (140 in
// RenderEverythingBarRoads, 100 in CVisibilityPlugins::RenderEntity), alpha func is
// GREATER. MTA applies setElementAlpha by scaling material alpha during the render,
// so at element alpha A every pixel lands at (texel * A / 255) and the object drops
// below the ref once A <= 140 or A <= 100 depending on path. lowering the refs
// globally is what smeared the LV neon textures (#4996), so scale the ref for this
// entity only: ref' = ref * A / 255. both sides of the compare scale equally, cutouts
// keep their shape and ref' < A so the object never fully vanishes. restored in the
// Post handler. player peds, cars and bikes set ref 1 and restore it inside their
// own Render fns. boats and trains never did, so vehicles get the same scaling,
// for cars and bikes it's a no-op since their own ref write lands after ours.
static bool  ms_bAlphaTestRefOverridden = false;
static DWORD ms_dwSavedAlphaTestRef = 0;

static void OverrideAlphaTestRefForElementAlpha(CEntitySAInterface* pEntity)
{
    unsigned char ucAlpha = 255;

    if (pEntity->nType == ENTITY_TYPE_OBJECT)
    {
        SClientEntity<CObjectSA>* pObjectClientEntity = pGameInterface->GetPools()->GetObject((DWORD*)pEntity);
        if (pObjectClientEntity && pObjectClientEntity->pEntity)
            ucAlpha = pObjectClientEntity->pEntity->GetAlpha();
    }
    else if (pEntity->nType == ENTITY_TYPE_VEHICLE)
    {
        SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)pEntity);
        if (pVehicleClientEntity && pVehicleClientEntity->pEntity)
            ucAlpha = pVehicleClientEntity->pEntity->GetAlpha();
    }

    if (ucAlpha == 255)
        return;

    // RwEngineInstance->dOpenDevice.fpRenderStateGet/fpRenderStateSet
    DWORD engine = *(DWORD*)0xC97B24;
    auto  fpGet = reinterpret_cast<BOOL(__cdecl*)(DWORD, void*)>(*(DWORD*)(engine + 0x24));
    auto  fpSet = reinterpret_cast<BOOL(__cdecl*)(DWORD, void*)>(*(DWORD*)(engine + 0x20));

    fpGet(0x1e /*rwRENDERSTATEALPHATESTFUNCTIONREF*/, &ms_dwSavedAlphaTestRef);
    fpSet(0x1e /*rwRENDERSTATEALPHATESTFUNCTIONREF*/, reinterpret_cast<void*>(ms_dwSavedAlphaTestRef * ucAlpha / 255));
    ms_bAlphaTestRefOverridden = true;
}

static void RestoreAlphaTestRef()
{
    if (!ms_bAlphaTestRefOverridden)
        return;

    ms_bAlphaTestRefOverridden = false;

    DWORD engine = *(DWORD*)0xC97B24;
    auto  fpSet = reinterpret_cast<BOOL(__cdecl*)(DWORD, void*)>(*(DWORD*)(engine + 0x20));
    fpSet(0x1e /*rwRENDERSTATEALPHATESTFUNCTIONREF*/, reinterpret_cast<void*>(ms_dwSavedAlphaTestRef));
}

bool OnMY_CEntity_RenderOneNonRoad_Pre(CEntitySAInterface* pEntity)
{
    ms_RenderingOneNonRoad = pEntity;
    CallGameEntityRenderHandler(ms_RenderingOneNonRoad);

    OverrideAlphaTestRefForElementAlpha(pEntity);

    return IsEntityRenderable(pEntity);
}

void OnMY_CEntity_RenderOneNonRoad_Post(CEntitySAInterface* pEntity)
{
    RestoreAlphaTestRef();

    if (ms_RenderingOneNonRoad)
    {
        ms_RenderingOneNonRoad = NULL;
        CallGameEntityRenderHandler(ms_RenderingOneNonRoad);
    }
}

// Hook info
#define HOOKPOS_CEntity_RenderOneNonRoad  0x553260
#define HOOKSIZE_CEntity_RenderOneNonRoad 5
DWORD                         RETURN_CEntity_RenderOneNonRoad = 0x553265;
static void __declspec(naked) HOOK_CEntity_RenderOneNonRoad()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    [esp+32+4*1]
        call    OnMY_CEntity_RenderOneNonRoad_Pre
        add     esp, 4*1
        cmp     eax, 0
        popad

        jz      skip_render
        push    [esp+4*1]
        call inner
        add     esp, 4*1
skip_render:

        pushad
        push    [esp+32+4*1]
        call    OnMY_CEntity_RenderOneNonRoad_Post
        add     esp, 4*1
        popad
        retn

inner:
        push    esi
        mov     esi, [esp+08h]
        jmp     RETURN_CEntity_RenderOneNonRoad
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVisibilityPlugin::RenderWeaponPedsForPC_Mid
//
// Detect next ped weapon rendering
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CVisibilityPlugins_RenderWeaponPedsForPC_Mid(CPedSAInterface* pEntity)
{
    CallGameEntityRenderHandler(pEntity);
}

// Hook info
#define HOOKPOS_CVisibilityPlugins_RenderWeaponPedsForPC_Mid  0x733080
#define HOOKSIZE_CVisibilityPlugins_RenderWeaponPedsForPC_Mid 6
DWORD                         RETURN_CVisibilityPlugins_RenderWeaponPedsForPC_Mid = 0x733086;
static void __declspec(naked) HOOK_CVisibilityPlugins_RenderWeaponPedsForPC_Mid()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    ebx
        call    OnMY_CVisibilityPlugins_RenderWeaponPedsForPC_Mid
        add     esp, 4*1
        popad

        // Continue original code
        mov     ecx,dword ptr [ebx+4F4h]
        jmp     RETURN_CVisibilityPlugins_RenderWeaponPedsForPC_Mid
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVisibilityPlugin::RenderWeaponPedsForPC_End
//
// End of all ped weapon rendering
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CVisibilityPlugins_RenderWeaponPedsForPC_End()
{
    CallGameEntityRenderHandler(NULL);
}

// Hook info
#define HOOKPOS_CVisibilityPlugins_RenderWeaponPedsForPC_End  0x73314D
#define HOOKSIZE_CVisibilityPlugins_RenderWeaponPedsForPC_End 5
static void __declspec(naked) HOOK_CVisibilityPlugins_RenderWeaponPedsForPC_End()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        call    OnMY_CVisibilityPlugins_RenderWeaponPedsForPC_End
        popad

        // Continue original code
        pop         esi
        add         esp,0Ch
        ret
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Check_NoOfVisibleLods
//
// Apply render limits
//
//////////////////////////////////////////////////////////////////////////////////////////
// Hook info
#define HOOKPOS_Check_NoOfVisibleLods  0x5534F9
#define HOOKSIZE_Check_NoOfVisibleLods 6
DWORD                         RETURN_Check_NoOfVisibleLods = 0x5534FF;
static void __declspec(naked) HOOK_Check_NoOfVisibleLods()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        cmp     eax, RENDERLIST_LIMIT - 1           // clamp to the render-list capacity
        jge     limit
        inc     eax
limit:
        mov     dword ptr ds:[00B76840h],eax        // NoOfVisibleLods
        jmp     RETURN_Check_NoOfVisibleLods
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Check_NoOfVisibleEntities
//
// Apply render limits
//
//////////////////////////////////////////////////////////////////////////////////////////
// Hook info
#define HOOKPOS_Check_NoOfVisibleEntities  0x55352D
#define HOOKSIZE_Check_NoOfVisibleEntities 6
DWORD                         RETURN_Check_NoOfVisibleEntities = 0x553533;
static void __declspec(naked) HOOK_Check_NoOfVisibleEntities()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        cmp     eax, RENDERLIST_LIMIT - 1           // clamp to the render-list capacity
        jge     limit
        inc     eax
limit:
        mov     dword ptr ds:[00B76844h],eax        // NoOfVisibleEntities
        jmp     RETURN_Check_NoOfVisibleEntities
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// WinLoop
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_WinLoop()
{
    if (ms_bIsMinimizedAndNotConnected)
        Sleep(10);
}

// Hook info
#define HOOKPOS_WinLoop  0x748A93
#define HOOKSIZE_WinLoop 5
DWORD                         RETURN_WinLoop = 0x748A98;
static void __declspec(naked) HOOK_WinLoop()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        call    OnMY_WinLoop
        popad

        mov     eax, ds:0x0C8D4C0
        jmp     RETURN_WinLoop
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CTimer::Update
//
// Just before GTA calculates frame time deltas
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CTimer_Update  0x561B10
#define HOOKSIZE_CTimer_Update 6
static const DWORD            CONTINUE_CTimer_Update = 0x561B16;
static void __declspec(naked) HOOK_CTimer_Update()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
    }
    // clang-format on

    g_pCore->OnGameTimerUpdate();

    // clang-format off
    __asm
    {
        popad
        // Original code: mov ecx, ds:_timerFunction
        // GTA has its own NULL check at 0x561B19, so we just execute the original instruction
        mov     ecx, dword ptr ds:[0B7CB28h]
        jmp     CONTINUE_CTimer_Update
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CTimer::Suspend / CTimer::Resume
//
// Prevent crashes if _timerFunction is NULL during init
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CTimer_Suspend  0x5619E9
#define HOOKSIZE_CTimer_Suspend 6
static const DWORD           CONTINUE_CTimer_Suspend = 0x5619EF;
static void _declspec(naked) HOOK_CTimer_Suspend()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    _asm
    {
        // Check if _timerFunction is NULL
        mov     eax, dword ptr ds:[0B7CB28h]
        test    eax, eax
        jz      skip_suspend

             // Original code: call [_timerFunction]
        call    eax
        jmp     CONTINUE_CTimer_Suspend
        
    skip_suspend:
        // If NULL, return zero timestamp (EAX:EDX = 0) to avoid corrupting pause time
        xor     eax, eax
        xor     edx, edx
        jmp     CONTINUE_CTimer_Suspend
    }
    // clang-format on
}

#define HOOKPOS_CTimer_Resume  0x561A11
#define HOOKSIZE_CTimer_Resume 6
static const DWORD           CONTINUE_CTimer_Resume = 0x561A17;
static void _declspec(naked) HOOK_CTimer_Resume()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    _asm
    {
        // Check if _timerFunction is NULL
        mov     eax, dword ptr ds:[0B7CB28h]
        test    eax, eax
        jz      skip_resume

             // Original code: call [_timerFunction]
        call    eax
        jmp     CONTINUE_CTimer_Resume
        
    skip_resume:
        // If NULL, return zero timestamp (EAX:EDX = 0) to avoid corrupting resume time calculations
        xor     eax, eax
        xor     edx, edx
        jmp     CONTINUE_CTimer_Resume
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Photograph screen grab in windowed mode
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_psGrabScreen_GetRect(HWND hWnd, LPRECT pRect)
{
    // Get the window client area
    GetClientRect(hWnd, pRect);
    LPPOINT pPoints = (LPPOINT)pRect;
    ClientToScreen(hWnd, pPoints);
    ClientToScreen(hWnd, pPoints + 1);

    // Clip to desktop
    RECT desktopRect;
    GetWindowRect(GetDesktopWindow(), &desktopRect);
    pRect->left = std::max(pRect->left, desktopRect.left);
    pRect->top = std::max(pRect->top, desktopRect.top);
    pRect->right = std::min(pRect->right, desktopRect.right);
    pRect->bottom = std::min(pRect->bottom, desktopRect.bottom);

    // Ensure at least 1 pixel
    pRect->bottom = std::max(pRect->bottom, pRect->top + 1);
    pRect->right = std::max(pRect->right, pRect->left + 1);
}

bool OnMY_psGrabScreen_ShouldUseRect()
{
    bool bWindowed;
    g_pCore->GetCVars()->Get("display_windowed", bWindowed);
    return bWindowed;
}

// Hook info
#define HOOKPOS_psGrabScreen  0x7452FC
#define HOOKSIZE_psGrabScreen 5
DWORD                         RETURN_psGrabScreen_YesChange = 0x745311;
DWORD                         RETURN_psGrabScreen_NoChange = 0x745336;
static void __declspec(naked) HOOK_psGrabScreen()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        call    OnMY_psGrabScreen_ShouldUseRect
        test    al, al
        jnz     use_rect
        popad
        jmp     RETURN_psGrabScreen_NoChange

use_rect:
        popad
        mov     edx, [eax]      // hwnd
        lea     ecx, [esp+24h]  // pRect result
        pushad
        push    ecx
        push    edx
        call    OnMY_psGrabScreen_GetRect
        add     esp, 4*2
        popad

        jmp     RETURN_psGrabScreen_YesChange
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CClouds::RenderSkyPolys
//
// This is the first thing drawn by GTA
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CClouds_RenderSkyPolys()
{
    if (pPreRenderSkyHandlerHandler)
        pPreRenderSkyHandlerHandler();
}

// Hook info
#define HOOKCHECK_CClouds_RenderSkyPolys 0xA1
#define HOOKPOS_CClouds_RenderSkyPolys   0x714650
#define HOOKSIZE_CClouds_RenderSkyPolys  5
DWORD                         RETURN_CClouds_RenderSkyPolys = 0x714655;
static void __declspec(naked) HOOK_CClouds_RenderSkyPolys()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        call    OnMY_CClouds_RenderSkyPolys
        popad

        mov     eax, ds:0x0B6F03C
        jmp     RETURN_CClouds_RenderSkyPolys
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// RwCameraSetNearClipPlane
//
// Called multiple times, with varying distances from 2.0f down to 0.05f
//  - The lower values are used when near walls and peds
//  - The higher values are used to reduce z-flicker when flying etc
//
//////////////////////////////////////////////////////////////////////////////////////////
float OnMY_RwCameraSetNearClipPlane(DWORD dwCalledFrom, void* pUnknown, float fDistance)
{
    float fSetting = pMultiplayer->GetNearClipDistance();
    if (fSetting == DEFAULT_NEAR_CLIP_DISTANCE)
    {
        // Do nothing if setting is default value
        return fDistance;
    }

    // Don't process calls from RenderScene as they are restoring saved values (which have already been processed here)
    if (dwCalledFrom > 0x53DF40 && dwCalledFrom < 0x53E160)
    {
        return fDistance;
    }

    if (fSetting < DEFAULT_NEAR_CLIP_DISTANCE)
    {
        // If required setting is lower than default, ensure value used is not higher.
        return std::min(fSetting, fDistance);
    }
    else
    {
        // If required setting is higher than default, converge value towards it.
        float fAlpha = UnlerpClamped(DEFAULT_NEAR_CLIP_DISTANCE, fSetting, DEFAULT_NEAR_CLIP_DISTANCE * 3);
        return Lerp(fDistance, fAlpha, fSetting);
    }
}

// Hook info
#define HOOKCHECK_RwCameraSetNearClipPlane 0xD9
#define HOOKPOS_RwCameraSetNearClipPlane   0x7EE1D0
#define HOOKSIZE_RwCameraSetNearClipPlane  5
DWORD                         RETURN_RwCameraSetNearClipPlane = 0x7EE1D5;
static void __declspec(naked) HOOK_RwCameraSetNearClipPlane()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        call    OnMY_RwCameraSetNearClipPlane
        add     esp, 4*3
        popad

        // Result is on fp stack
        //fld     [esp+8]
        push    esi
        jmp     RETURN_RwCameraSetNearClipPlane
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Rendering heli search lights (in RenderEffects at 0x53E170)
//
// Render our own heli search lights (we have to do that here as
// it requires some render states
//
//////////////////////////////////////////////////////////////////////////////////////////
// Hook info
#define HOOKPOS_RenderEffects_HeliLight  0x53E1B9
#define HOOKSIZE_RenderEffects_HeliLight 5
DWORD                         RETURN_RenderEffects_HeliLight = 0x53E1BE;
static void __declspec(naked) HOOK_RenderEffects_HeliLight()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
    }
    // clang-format on

    // Call render handler
    if (pRenderHeliLightHandler) pRenderHeliLightHandler();

    // clang-format off
    __asm
    {
        popad
        mov     eax, ds:[0xC1C96C]
        jmp     RETURN_RenderEffects_HeliLight
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::SetGameEntityRenderHandler
//
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::SetGameEntityRenderHandler(GameEntityRenderHandler* pHandler)
{
    pGameEntityRenderHandler = pHandler;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::SetPreRenderSkyHandler
//
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::SetPreRenderSkyHandler(PreRenderSkyHandler* pHandler)
{
    pPreRenderSkyHandlerHandler = pHandler;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::SetRenderHeliLightHandler
//
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::SetRenderHeliLightHandler(RenderHeliLightHandler* pHandler)
{
    pRenderHeliLightHandler = pHandler;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::SetRenderEverythingBarRoadsHandler
//
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::SetRenderEverythingBarRoadsHandler(RenderEverythingBarRoadsHandler* pHandler)
{
    pRenderEverythingBarRoadsHandler = pHandler;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::SetIsMinimizedAndNotConnected
//
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::SetIsMinimizedAndNotConnected(bool bIsMinimizedAndNotConnected)
{
    ms_bIsMinimizedAndNotConnected = bIsMinimizedAndNotConnected;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::SetMirrorsEnabled
//
// Stop mirrors by skipping the frame setup and rendering code.
// Forces mirror render buffer recreation when enabling.
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::SetMirrorsEnabled(bool bEnabled)
{
    int&   iNumMirrorZones = *(int*)VAR_CCullZones_NumMirrorAttributeZones;
    uchar& bResetMirror = *(uchar*)VAR_CMirrors_d3dRestored;

    if (!bEnabled)
    {
        // Remove mirror zones
        ms_iSavedNumMirrorZones += iNumMirrorZones;
        iNumMirrorZones = 0;
    }
    else
    {
        if (ms_iSavedNumMirrorZones)
        {
            // Restore mirror zones
            iNumMirrorZones += ms_iSavedNumMirrorZones;
            ms_iSavedNumMirrorZones = 0;

            // Recreate render buffers
            bResetMirror = true;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVisibilityPlugins::RenderPedCB
//
// Ped atomic render callback
// Fix for screen flickering caused by corrupted matrices
//
//////////////////////////////////////////////////////////////////////////////////////////
bool IsMatrixValid(RwMatrix* pMatrix)
{
    constexpr float maxValue = 100000.0;
    constexpr float minValue = -100000.0;

    return pMatrix->at.x >= minValue && pMatrix->at.x <= maxValue && pMatrix->at.y >= minValue && pMatrix->at.y <= maxValue && pMatrix->at.z >= minValue &&
           pMatrix->at.y <= maxValue

           && pMatrix->right.x >= minValue && pMatrix->right.x <= maxValue && pMatrix->right.y >= minValue && pMatrix->right.y <= maxValue &&
           pMatrix->right.z >= minValue && pMatrix->right.y <= maxValue

           && pMatrix->up.x >= minValue && pMatrix->up.x <= maxValue && pMatrix->up.y >= minValue && pMatrix->up.y <= maxValue && pMatrix->up.z >= minValue &&
           pMatrix->up.y <= maxValue

           && pMatrix->pos.x >= minValue && pMatrix->pos.x <= maxValue && pMatrix->pos.y >= minValue && pMatrix->pos.y <= maxValue &&
           pMatrix->pos.z >= minValue && pMatrix->pos.y <= maxValue;
}

bool AreMatricesOfRpAtomicValid(RpAtomic* pAtomic)
{
    uint32            atomicSkinOffset = *(uint32*)0xC978A4;
    RpHAnimHierarchy* pSkinPluginData = *(RpHAnimHierarchy**)((char*)pAtomic + atomicSkinOffset);

    if (!pSkinPluginData)
        return true;

    unsigned __int32 count = pSkinPluginData->numNodes;
    RwMatrix*        pMatrixArray = pSkinPluginData->pMatrixArray;

    for (unsigned int i = 0; i < count; i++)
    {
        if (!IsMatrixValid(pMatrixArray + i))
            return false;
    }

    return true;
}

#define HOOKPOS_CVisibilityPlugins_RenderPedCB  0x7335B0
#define HOOKSIZE_CVisibilityPlugins_RenderPedCB 5
static void __declspec(naked) HOOK_CVisibilityPlugins_RenderPedCB()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push esi;
        push edi;
        mov edi, [esp + 0Ch]; // RpAtomic
        
        push edi;
        call AreMatricesOfRpAtomicValid;
        add esp, 4;
        
        test al, al;
        jz skipRender;
        
        push 0x7335B6; // Continue rendering
        retn;
        
    skipRender:
        pop edi;
        pop esi;
        retn;
    }
    // clang-format on
}

// Hook info
#define HOOKPOS_CRenderer_EverythingBarRoads  0x553C78
#define HOOKSIZE_CRenderer_EverythingBarRoads 5
DWORD                         RETURN_CRenderer_EverythingBarRoads = 0x553C7D;
DWORD                         DO_CRenderer_EverythingBarRoads = 0x7EE180;
static void __declspec(naked) HOOK_CRenderer_EverythingBarRoads()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
    }
    // clang-format on

    if (pRenderEverythingBarRoadsHandler) pRenderEverythingBarRoadsHandler();

    // clang-format off
    __asm
    {
        popad
        call DO_CRenderer_EverythingBarRoads
        jmp RETURN_CRenderer_EverythingBarRoads

    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_Rendering
//
// Setup hook
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Rendering()
{
    EZHookInstall(CallIdle);
    EZHookInstall(CEntity_Render);
    EZHookInstall(CEntity_RenderOneNonRoad);
    EZHookInstall(CVisibilityPlugins_RenderWeaponPedsForPC_Mid);
    EZHookInstall(CVisibilityPlugins_RenderWeaponPedsForPC_End);
    EZHookInstall(CVisibilityPlugins_RenderPedCB);
    EZHookInstall(Check_NoOfVisibleLods);
    EZHookInstall(Check_NoOfVisibleEntities);

    // Relocate the two render-list arrays to our larger buffers (see RENDERLIST_LIMIT).
    // Each site is a `[reg*4 + base]` access; we rewrite the 4-byte base immediate.
    // ms_aVisibleLodPtrs (0xB748F8): 1 write + 2 reads
    MemPut<DWORD>(0x5534F5, (DWORD)&g_relocatedVisibleLodPtrs[0]);
    MemPut<DWORD>(0x553923, (DWORD)&g_relocatedVisibleLodPtrs[0]);
    MemPut<DWORD>(0x553CB3, (DWORD)&g_relocatedVisibleLodPtrs[0]);
    // ms_aVisibleEntityPtrs (0xB75898): 1 write + 3 reads
    MemPut<DWORD>(0x553529, (DWORD)&g_relocatedVisibleEntityPtrs[0]);
    MemPut<DWORD>(0x553944, (DWORD)&g_relocatedVisibleEntityPtrs[0]);
    MemPut<DWORD>(0x553A53, (DWORD)&g_relocatedVisibleEntityPtrs[0]);
    MemPut<DWORD>(0x553B03, (DWORD)&g_relocatedVisibleEntityPtrs[0]);
    EZHookInstall(WinLoop);
    EZHookInstall(CTimer_Update);
    EZHookInstall(CTimer_Suspend);
    EZHookInstall(CTimer_Resume);
    EZHookInstall(psGrabScreen);
    EZHookInstallChecked(CClouds_RenderSkyPolys);
    EZHookInstallChecked(RwCameraSetNearClipPlane);
    EZHookInstall(RenderEffects_HeliLight);
    EZHookInstall(CRenderer_EverythingBarRoads);
}
