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
extern CCoreInterface*   g_pCore;
GameEntityRenderHandler* pGameEntityRenderHandler = nullptr;
PreRenderSkyHandler*     pPreRenderSkyHandlerHandler = nullptr;
RenderHeliLightHandler*  pRenderHeliLightHandler = nullptr;
RenderEverythingBarRoadsHandler*  pRenderEverythingBarRoadsHandler = nullptr;

#define VAR_CCullZones_NumMirrorAttributeZones  0x0C87AC4   // int
#define VAR_CMirrors_d3dRestored                0x0C7C729   // uchar

namespace
{
    CEntitySAInterface* ms_Rendering = NULL;
    CEntitySAInterface* ms_RenderingOneNonRoad = NULL;
    bool                ms_bIsMinimizedAndNotConnected = false;
    int                 ms_iSavedNumMirrorZones = 0;
}            // namespace

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
#define HOOKPOS_CallIdle                         0x53ECBD
#define HOOKSIZE_CallIdle                        5
DWORD RETURN_CallIdle = 0x53ECC2;
DWORD DO_CallIdle = 0x53E920;
void _declspec(naked) HOOK_CallIdle()
{
    _asm
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
        CallGameEntityRenderHandler(ms_RenderingOneNonRoad);            // restore value set in RenderOneNonRoad
    }
}

// Hook info
#define HOOKPOS_CEntity_Render                         0x534310
#define HOOKSIZE_CEntity_Render                        6
DWORD RETURN_CEntity_Render = 0x534317;
void _declspec(naked) HOOK_CEntity_Render()
{
    _asm
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
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CEntity::RenderOneNonRoad
//
// Detect entity rendering
//
//////////////////////////////////////////////////////////////////////////////////////////
bool OnMY_CEntity_RenderOneNonRoad_Pre(CEntitySAInterface* pEntity)
{
    ms_RenderingOneNonRoad = pEntity;
    CallGameEntityRenderHandler(ms_RenderingOneNonRoad);
    return IsEntityRenderable(pEntity);
}

void OnMY_CEntity_RenderOneNonRoad_Post(CEntitySAInterface* pEntity)
{
    if (ms_RenderingOneNonRoad)
    {
        ms_RenderingOneNonRoad = NULL;
        CallGameEntityRenderHandler(ms_RenderingOneNonRoad);
    }
}

// Hook info
#define HOOKPOS_CEntity_RenderOneNonRoad                         0x553260
#define HOOKSIZE_CEntity_RenderOneNonRoad                        5
DWORD RETURN_CEntity_RenderOneNonRoad = 0x553265;
void _declspec(naked) HOOK_CEntity_RenderOneNonRoad()
{
    _asm
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
#define HOOKPOS_CVisibilityPlugins_RenderWeaponPedsForPC_Mid                0x733080
#define HOOKSIZE_CVisibilityPlugins_RenderWeaponPedsForPC_Mid               6
DWORD RETURN_CVisibilityPlugins_RenderWeaponPedsForPC_Mid = 0x733086;
void _declspec(naked) HOOK_CVisibilityPlugins_RenderWeaponPedsForPC_Mid()
{
    _asm
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
#define HOOKPOS_CVisibilityPlugins_RenderWeaponPedsForPC_End                0x73314D
#define HOOKSIZE_CVisibilityPlugins_RenderWeaponPedsForPC_End               5
void _declspec(naked) HOOK_CVisibilityPlugins_RenderWeaponPedsForPC_End()
{
    _asm
    {
        pushad
        call    OnMY_CVisibilityPlugins_RenderWeaponPedsForPC_End
        popad

        // Continue original code
        pop         esi
        add         esp,0Ch
        ret
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Check_NoOfVisibleLods
//
// Apply render limits
//
//////////////////////////////////////////////////////////////////////////////////////////
// Hook info
#define HOOKPOS_Check_NoOfVisibleLods                         0x5534F9
#define HOOKSIZE_Check_NoOfVisibleLods                        6
DWORD RETURN_Check_NoOfVisibleLods = 0x5534FF;
void _declspec(naked) HOOK_Check_NoOfVisibleLods()
{
    _asm
    {
        cmp     eax, 999            // Array limit is 1000
        jge     limit
        inc     eax
limit:
        mov     dword ptr ds:[00B76840h],eax        // NoOfVisibleLods
        jmp     RETURN_Check_NoOfVisibleLods
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Check_NoOfVisibleEntities
//
// Apply render limits
//
//////////////////////////////////////////////////////////////////////////////////////////
// Hook info
#define HOOKPOS_Check_NoOfVisibleEntities                         0x55352D
#define HOOKSIZE_Check_NoOfVisibleEntities                        6
DWORD RETURN_Check_NoOfVisibleEntities = 0x553533;
void _declspec(naked) HOOK_Check_NoOfVisibleEntities()
{
    _asm
    {
        cmp     eax, 999        // Array limit is 1000
        jge     limit
        inc     eax
limit:
        mov     dword ptr ds:[00B76844h],eax        // NoOfVisibleEntities
        jmp     RETURN_Check_NoOfVisibleEntities
    }
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
#define HOOKPOS_WinLoop                            0x748A93
#define HOOKSIZE_WinLoop                           5
DWORD RETURN_WinLoop = 0x748A98;
void _declspec(naked) HOOK_WinLoop()
{
    _asm
    {
        pushad
        call    OnMY_WinLoop
        popad

        mov     eax, ds:0x0C8D4C0
        jmp     RETURN_WinLoop
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CTimer::Update
//
// Just before GTA calculates frame time deltas
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CTimer_Update               0x561B10
#define HOOKSIZE_CTimer_Update              6
static const DWORD CONTINUE_CTimer_Update = 0x561B16;
static void _declspec(naked) HOOK_CTimer_Update()
{
    _asm
    {
        pushad
    }

    g_pCore->OnGameTimerUpdate();

    _asm
    {
        popad
        mov     ecx,dword ptr ds:[0B7CB28h]
        jmp     CONTINUE_CTimer_Update
    }
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
#define HOOKPOS_psGrabScreen                        0x7452FC
#define HOOKSIZE_psGrabScreen                       5
DWORD RETURN_psGrabScreen_YesChange = 0x745311;
DWORD RETURN_psGrabScreen_NoChange = 0x745336;
void _declspec(naked) HOOK_psGrabScreen()
{
    _asm
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
#define HOOKCHECK_CClouds_RenderSkyPolys            0xA1
#define HOOKPOS_CClouds_RenderSkyPolys              0x714650
#define HOOKSIZE_CClouds_RenderSkyPolys             5
DWORD RETURN_CClouds_RenderSkyPolys = 0x714655;
void _declspec(naked) HOOK_CClouds_RenderSkyPolys()
{
    _asm
    {
        pushad
        call    OnMY_CClouds_RenderSkyPolys
        popad

        mov     eax, ds:0x0B6F03C
        jmp     RETURN_CClouds_RenderSkyPolys
    }
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
#define HOOKCHECK_RwCameraSetNearClipPlane          0xD9
#define HOOKPOS_RwCameraSetNearClipPlane            0x7EE1D0
#define HOOKSIZE_RwCameraSetNearClipPlane           5
DWORD RETURN_RwCameraSetNearClipPlane = 0x7EE1D5;
void _declspec(naked) HOOK_RwCameraSetNearClipPlane()
{
    _asm
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
#define HOOKPOS_RenderEffects_HeliLight                           0x53E1B9
#define HOOKSIZE_RenderEffects_HeliLight                          5
DWORD RETURN_RenderEffects_HeliLight = 0x53E1BE;
void _declspec(naked) HOOK_RenderEffects_HeliLight()
{
    _asm
    {
        pushad
    }

    // Call render handler
    if (pRenderHeliLightHandler) pRenderHeliLightHandler();

    _asm
    {
        popad
        mov     eax, ds:[0xC1C96C]
        jmp     RETURN_RenderEffects_HeliLight
    }
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

    return
        pMatrix->at.x >= minValue && pMatrix->at.x <= maxValue
        && pMatrix->at.y >= minValue && pMatrix->at.y <= maxValue
        && pMatrix->at.z >= minValue && pMatrix->at.y <= maxValue

        && pMatrix->right.x >= minValue && pMatrix->right.x <= maxValue
        && pMatrix->right.y >= minValue && pMatrix->right.y <= maxValue
        && pMatrix->right.z >= minValue && pMatrix->right.y <= maxValue

        && pMatrix->up.x >= minValue && pMatrix->up.x <= maxValue
        && pMatrix->up.y >= minValue && pMatrix->up.y <= maxValue
        && pMatrix->up.z >= minValue && pMatrix->up.y <= maxValue

        && pMatrix->pos.x >= minValue && pMatrix->pos.x <= maxValue
        && pMatrix->pos.y >= minValue && pMatrix->pos.y <= maxValue
        && pMatrix->pos.z >= minValue && pMatrix->pos.y <= maxValue;
}

bool AreMatricesOfRpAtomicValid(RpAtomic* pAtomic)
{
    uint32 atomicSkinOffset = *(uint32*)0xC978A4;
    RpHAnimHierarchy* pSkinPluginData = *(RpHAnimHierarchy**)((char*)pAtomic + atomicSkinOffset);

    if (!pSkinPluginData)
        return true;

    unsigned __int32 count = pSkinPluginData->numNodes;
    RwMatrix* pMatrixArray = pSkinPluginData->pMatrixArray;

    for (unsigned int i = 0; i < count; i++)
    {
        if (!IsMatrixValid(pMatrixArray + i))
            return false;
    }

    return true;
}

#define HOOKPOS_CVisibilityPlugins_RenderPedCB                        0x7335B0
#define HOOKSIZE_CVisibilityPlugins_RenderPedCB                       5
void _declspec(naked) HOOK_CVisibilityPlugins_RenderPedCB()
{
    _asm
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
}

// Hook info
#define HOOKPOS_CRenderer_EverythingBarRoads                         0x553C78
#define HOOKSIZE_CRenderer_EverythingBarRoads                        5
DWORD RETURN_CRenderer_EverythingBarRoads = 0x553C7D;
DWORD DO_CRenderer_EverythingBarRoads = 0x7EE180;
void _declspec(naked) HOOK_CRenderer_EverythingBarRoads()
{
    _asm
    {
        pushad
    }

    if (pRenderEverythingBarRoadsHandler)
        pRenderEverythingBarRoadsHandler();

    _asm
    {
        popad
        call DO_CRenderer_EverythingBarRoads
        jmp RETURN_CRenderer_EverythingBarRoads

    }
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
    EZHookInstall(WinLoop);
    EZHookInstall(CTimer_Update);
    EZHookInstall(psGrabScreen);
    EZHookInstallChecked(CClouds_RenderSkyPolys);
    EZHookInstallChecked(RwCameraSetNearClipPlane);
    EZHookInstall(RenderEffects_HeliLight);
    EZHookInstall(CRenderer_EverythingBarRoads);
}
