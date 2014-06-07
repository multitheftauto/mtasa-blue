/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CSettingsSA.cpp
*  PURPOSE:     Game settings
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

static const float MOUSE_SENSITIVITY_MIN     = 0.000312f;
static const float MOUSE_SENSITIVITY_DEFAULT = 0.0025f;
static const float MOUSE_SENSITIVITY_MAX     = MOUSE_SENSITIVITY_DEFAULT * 2 - MOUSE_SENSITIVITY_MIN;

unsigned long CSettingsSA::FUNC_GetNumVideoModes;
unsigned long CSettingsSA::FUNC_GetVideoModeInfo;
unsigned long CSettingsSA::FUNC_GetCurrentVideoMode;
unsigned long CSettingsSA::FUNC_SetCurrentVideoMode;
unsigned long CSettingsSA::FUNC_SetDrawDistance;
unsigned long CSettingsSA::FUNC_GetNumSubSystems;
unsigned long CSettingsSA::FUNC_GetCurrentSubSystem;
unsigned long CSettingsSA::FUNC_SetSubSystem;

#define VAR_CurVideoMode            (*((uint*)(0x08D6220)))
#define VAR_SavedVideoMode          (*((uint*)(0x0BA6820)))
#define VAR_CurAdapter              (*((uint*)(0x0C920F4)))

#define HOOKPOS_GetFxQuality                0x49EA50
void HOOK_GetFxQuality ();

#define HOOKPOS_StoreShadowForVehicle       0x70BDA0
DWORD RETURN_StoreShadowForVehicle =        0x70BDA9;
void HOOK_StoreShadowForVehicle ();

CSettingsSA::CSettingsSA ( void )
{
    m_pInterface = (CSettingsSAInterface *)CLASS_CMenuManager;
    m_pInterface->bFrameLimiter = false;
    m_bVolumetricShadowsEnabled = false;
    m_bVolumetricShadowsSuspended = false;
    SetAspectRatio ( ASPECT_RATIO_4_3 );
    HookInstall ( HOOKPOS_GetFxQuality, (DWORD)HOOK_GetFxQuality, 5 );
    HookInstall ( HOOKPOS_StoreShadowForVehicle, (DWORD)HOOK_StoreShadowForVehicle, 9 );
    m_iDesktopWidth = 0;
    m_iDesktopHeight = 0;
    MemPut < BYTE > ( 0x6FF420, 0xC3 );     // Truncate CalculateAspectRatio
}

bool CSettingsSA::IsWideScreenEnabled ( void )
{
    return m_pInterface->bUseWideScreen;
}

void CSettingsSA::SetWideScreenEnabled ( bool bEnabled )
{
    m_pInterface->bUseWideScreen = bEnabled;
}

unsigned int CSettingsSA::GetNumVideoModes ( void )
{
    unsigned int uiReturn = 0;
    _asm
    {
        call    FUNC_GetNumVideoModes
        mov     uiReturn, eax
    }
    return uiReturn;
}

VideoMode * CSettingsSA::GetVideoModeInfo ( VideoMode * modeInfo, unsigned int modeIndex )
{
    VideoMode* pReturn = NULL;
    _asm
    {
        push    modeIndex
        push    modeInfo
        call    FUNC_GetVideoModeInfo
        mov     pReturn, eax
        add     esp, 8
    }
    return pReturn;
}

unsigned int CSettingsSA::GetCurrentVideoMode ( void )
{
    unsigned int uiReturn = 0;
    _asm
    {
        call    FUNC_GetCurrentVideoMode
        mov     uiReturn, eax
    }
    return uiReturn;
}

void CSettingsSA::SetCurrentVideoMode ( unsigned int modeIndex, bool bOnRestart )
{
    if ( !bOnRestart )
    {
        _asm
        {
            push    modeIndex
            call    FUNC_SetCurrentVideoMode
            add     esp, 4
        }
    }
    // Only update settings variables for fullscreen modes
    if ( modeIndex )
        m_pInterface->dwVideoMode = modeIndex;
}

uint CSettingsSA::GetNumAdapters ( void )
{
    unsigned int uiReturn = 0;
    _asm
    {
        call    FUNC_GetNumSubSystems
        mov     uiReturn, eax
    }
    return uiReturn;
}

void CSettingsSA::SetAdapter ( unsigned int uiAdapterIndex )
{
    _asm
    {
        push    uiAdapterIndex
        call    FUNC_SetSubSystem
        add     esp, 4
    }
}

unsigned int CSettingsSA::GetCurrentAdapter ( void )
{
    unsigned int uiReturn = 0;
    _asm
    {
        call    FUNC_GetCurrentSubSystem
        mov     uiReturn, eax
    }
    return uiReturn;
}

unsigned char CSettingsSA::GetRadioVolume ( void )
{
    return m_pInterface->ucRadioVolume;
}

void CSettingsSA::SetRadioVolume ( unsigned char ucVolume )
{
    m_pInterface->ucRadioVolume = ucVolume;
    pGame->GetAudioEngine ()->SetMusicMasterVolume ( ucVolume );
}

unsigned char CSettingsSA::GetSFXVolume ( void )
{
    return m_pInterface->ucSfxVolume;
}

void CSettingsSA::SetSFXVolume ( unsigned char ucVolume )
{
    m_pInterface->ucSfxVolume = ucVolume;
    pGame->GetAudioEngine ()->SetEffectsMasterVolume ( ucVolume );
}

unsigned int CSettingsSA::GetUsertrackMode ( void )
{
    // 0 = radio, 1 = random, 2 = sequential
    return m_pInterface->ucUsertrackMode;
}

void CSettingsSA::SetUsertrackMode ( unsigned int uiMode )
{
    m_pInterface->ucUsertrackMode = uiMode;
}

bool CSettingsSA::IsUsertrackAutoScan ( void )
{
    // 1 = yes, 0 = no
    return m_pInterface->bUsertrackAutoScan;
}

void CSettingsSA::SetUsertrackAutoScan ( bool bEnable )
{
    m_pInterface->bUsertrackAutoScan = bEnable;
}

bool CSettingsSA::IsRadioEqualizerEnabled ( void )
{
    // 1 = on, 0 = off
    return m_pInterface->bRadioEqualizer;
}

void CSettingsSA::SetRadioEqualizerEnabled ( bool bEnable )
{
    m_pInterface->bRadioEqualizer = bEnable;
}

bool CSettingsSA::IsRadioAutotuneEnabled ( void )
{
    // 1 = on, 0 = off
    return m_pInterface->bRadioAutotune;
}

void CSettingsSA::SetRadioAutotuneEnabled ( bool bEnable )
{
    m_pInterface->bRadioAutotune = bEnable;
}

// Minimum is 0.925 and maximum is 1.8
float CSettingsSA::GetDrawDistance ( void )
{
    return m_pInterface->fDrawDistance;
}

void CSettingsSA::SetDrawDistance ( float fDistance )
{
    _asm
    {
        push    fDistance
        call    FUNC_SetDrawDistance
        add     esp, 4
    }
    m_pInterface->fDrawDistance = fDistance;
}

unsigned int CSettingsSA::GetBrightness ( )
{
    // up to 384
    return m_pInterface->dwBrightness;
}

void CSettingsSA::SetBrightness ( unsigned int uiBrightness )
{
    m_pInterface->dwBrightness = uiBrightness;
}

unsigned int CSettingsSA::GetFXQuality ( )
{
    // 0 = low, 1 = medium, 2 = high, 3 = very high
    return *(BYTE *)VAR_ucFxQuality;
}

void CSettingsSA::SetFXQuality ( unsigned int fxQualityId )
{
    MemPutFast < BYTE > ( VAR_ucFxQuality, fxQualityId );
}

float CSettingsSA::GetMouseSensitivity ( )
{
    float fRawValue = *(FLOAT *)VAR_fMouseSensitivity;
    return UnlerpClamped( MOUSE_SENSITIVITY_MIN, fRawValue, MOUSE_SENSITIVITY_MAX );    // Remap to 0-1
}

void CSettingsSA::SetMouseSensitivity ( float fSensitivity )
{
    float fRawValue = Lerp( MOUSE_SENSITIVITY_MIN, fSensitivity, MOUSE_SENSITIVITY_MAX );
    MemPutFast < FLOAT > ( VAR_fMouseSensitivity, fRawValue );
}

unsigned int CSettingsSA::GetAntiAliasing ( )
{
    // 1 = disabled, 2 = 1x, 3 = 2x, 4 = 3x
    return m_pInterface->dwAntiAliasing;
}

void CSettingsSA::SetAntiAliasing ( unsigned int uiAntiAliasing, bool bOnRestart )
{
    if ( !bOnRestart )
    {
        DWORD dwFunc = FUNC_SetAntiAliasing;
        _asm
        {
            push    uiAntiAliasing
            call    dwFunc
            add     esp, 4
        }
        SetCurrentVideoMode ( m_pInterface->dwVideoMode, false );
    }

    m_pInterface->dwAntiAliasing = uiAntiAliasing;
}

bool CSettingsSA::IsMipMappingEnabled ( void )
{
    return m_pInterface->bMipMapping;
}

void CSettingsSA::SetMipMappingEnabled ( bool bEnable )
{
    m_pInterface->bMipMapping = bEnable;
}

void CSettingsSA::Save ()
{
    _asm
    {
        mov ecx, CLASS_CMenuManager
        mov eax, FUNC_CMenuManager_Save
        call eax
    }
}

bool CSettingsSA::IsVolumetricShadowsEnabled ( void )
{
    return m_bVolumetricShadowsEnabled && !m_bVolumetricShadowsSuspended;
}

void CSettingsSA::SetVolumetricShadowsEnabled ( bool bEnable )
{
    m_bVolumetricShadowsEnabled = bEnable;
}

void CSettingsSA::SetVolumetricShadowsSuspended ( bool bSuspended )
{
    m_bVolumetricShadowsSuspended = bSuspended;
}

//
// Volumetric shadow hooks
//
DWORD dwFxQualityValue = 0;
WORD usCallingForVehicleModel = 0;

void _cdecl MaybeAlterFxQualityValue ( DWORD dwAddrCalledFrom )
{
    // Handle all calls from CVolumetricShadowMgr
    if ( dwAddrCalledFrom > 0x70F990 && dwAddrCalledFrom < 0x711EB0 )
    {
        // Force blob shadows if volumetric shadows are not enabled
        if ( !pGame->GetSettings ()->IsVolumetricShadowsEnabled () )
            dwFxQualityValue = 0;

        // These vehicles seem to have problems with volumetric shadows, so force blob shadows
        switch ( usCallingForVehicleModel )
        {
            case 460:   // Skimmer
            case 511:   // Beagle
            case 572:   // Mower
            case 590:   // Box Freight
            case 592:   // Andromada
                dwFxQualityValue = 0;
        }
        usCallingForVehicleModel = 0;
    }
    else
    // Handle all calls from CPed::PreRenderAfterTest
    if ( dwAddrCalledFrom > 0x5E65A0 && dwAddrCalledFrom < 0x5E7680 )
    {
        // Always use blob shadows for peds as realtime shadows are disabled in MTA (context switching issues)
        dwFxQualityValue = 0;
    }
}

// Hooked from 0x49EA50
void _declspec(naked) HOOK_GetFxQuality ()
{
    _asm
    {
        pushad
        mov     eax, [ecx+054h]         // Current FxQuality setting
        mov     dwFxQualityValue, eax

        mov     eax, [esp+32]           // Address GetFxQuality was called from
        push    eax                     
        call    MaybeAlterFxQualityValue
        add     esp, 4

        popad
        mov     eax, dwFxQualityValue
        retn
    }
}

// Hook to discover what vehicle will be calling GetFxQuality
void _declspec(naked) HOOK_StoreShadowForVehicle ()
{
    _asm
    {
        // Hooked from 0x70BDA0  5 bytes
        mov     eax, [esp+4]        // Get vehicle
        mov     ax, [eax+34]        // pEntity->m_nModelIndex
        mov     usCallingForVehicleModel, ax
        sub     esp, 44h 
        push    ebx
        mov     eax, 0x70F9B0       // CVolumetricShadowMgr::IsAvailable
        call    eax
        jmp     RETURN_StoreShadowForVehicle
    }
}


////////////////////////////////////////////////
//
// AspectRatio
//
////////////////////////////////////////////////
eAspectRatio CSettingsSA::GetAspectRatio ( void )
{
    return m_AspectRatio;
}

float CSettingsSA::GetAspectRatioValue ( void )
{
    return *(float*)0xC3EFA4;
}

void CSettingsSA::SetAspectRatio ( eAspectRatio aspectRatio, bool bAdjustmentEnabled )
{
    // Process change
    m_AspectRatio = aspectRatio;

    float fValue;
    if ( m_AspectRatio == ASPECT_RATIO_AUTO )
    {
        VideoMode modeInfo;
        pGame->GetSettings ()->GetVideoModeInfo ( &modeInfo, pGame->GetSettings ()->GetCurrentVideoMode () );
        fValue = modeInfo.width / (float)modeInfo.height;
    }
    else
    if ( m_AspectRatio == ASPECT_RATIO_4_3 )
    {
        fValue = 4 / 3.f;
    }
    else
    if ( m_AspectRatio == ASPECT_RATIO_16_10 )
    {
        fValue = 16 / 10.f;
    }
    else    // ASPECT_RATIO_16_9
    {
        fValue = 16 / 9.f;
    }

    MemPutFast < float > ( 0xC3EFA4, fValue );

    // Adjust position and size of our HUD components
    if ( bAdjustmentEnabled )
        pGame->GetHud ()->AdjustComponents ( fValue );
    else
        pGame->GetHud ()->ResetComponentAdjustment ();
}

////////////////////////////////////////////////
//
// Grass
//
////////////////////////////////////////////////
bool CSettingsSA::IsGrassEnabled ( void )
{
    return *(BYTE*)0x05DBAED == 0x85;
}

void CSettingsSA::SetGrassEnabled ( bool bEnable )
{
    MemPut < BYTE > ( 0x05DBAED, bEnable ? 0x85 : 0x33 );
}


////////////////////////////////////////////////
//
// CSettingsSA::HasUnsafeResolutions
//
// Return true if DirectX says we have resolutions available that are higher that the desktop
//
////////////////////////////////////////////////
bool CSettingsSA::HasUnsafeResolutions( void )
{
    uint numVidModes = GetNumVideoModes();
    for ( uint vidMode = 0; vidMode < numVidModes; vidMode++ )
    {
        VideoMode vidModeInfo;
        GetVideoModeInfo( &vidModeInfo, vidMode );

        if ( vidModeInfo.flags & rwVIDEOMODEEXCLUSIVE )
        {
            if ( IsUnsafeResolution( vidModeInfo.width, vidModeInfo.height ) )
                return true;
        }
    }
    return false;
}


////////////////////////////////////////////////
//
// CSettingsSA::IsUnsafeResolution
//
// Check if supplied resolution is higher than the desktop
//
////////////////////////////////////////////////
bool CSettingsSA::IsUnsafeResolution( int iWidth, int iHeight )
{
    // Check if we have gotten the desktop res yet
    if ( m_iDesktopWidth == 0 )
    {
        m_iDesktopWidth = 800;
        m_iDesktopHeight = 600;

        VideoMode currentModeInfo;
        if ( GetVideoModeInfo( &currentModeInfo, 0 ) )
        {
            m_iDesktopWidth = currentModeInfo.width;
            m_iDesktopHeight = currentModeInfo.height;
        }
    }
    return iWidth > m_iDesktopWidth || iHeight > m_iDesktopHeight;
}


////////////////////////////////////////////////
//
// CSettingsSA::FindVideoMode
//
// Find best matching video mode
//
////////////////////////////////////////////////
uint CSettingsSA::FindVideoMode( int iResX, int iResY, int iColorBits )
{
    int iBestMode, iBestScore = -1;

    uint numVidModes = GetNumVideoModes();
    for ( uint vidMode = 0; vidMode < numVidModes; vidMode++ )
    {
        VideoMode vidModeInfo;
        GetVideoModeInfo( &vidModeInfo, vidMode );

        // Remove resolutions that will make the gui unusable
        if ( vidModeInfo.width < 640 || vidModeInfo.height < 480 )
            continue;

        if ( vidModeInfo.flags & rwVIDEOMODEEXCLUSIVE )
        {
            // Rate my res
            int iScore = abs( iResX - vidModeInfo.width ) + abs( iResY - vidModeInfo.height );

            // Penalize matches with wrong bit depth
            if ( vidModeInfo.depth != iColorBits )
            {
                iScore += 100000;
            }

            // Penalize matches with higher than requested resolution
            if ( vidModeInfo.width > iResX || vidModeInfo.height > iResY )
            {
                iScore += 200000;
            }

            if ( iScore < iBestScore || iBestScore == -1 )
            {
                // Found a better match
                iBestScore = iScore;
                iBestMode = vidMode;
            }
        }
    }

    if ( iBestScore != -1 )
        return iBestMode;

    BrowseToSolution ( "no-find-res", EXIT_GAME_FIRST | ASK_GO_ONLINE, _( "Can't find valid screen resolution." ) );
    return 1;
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CSettingsSA::SetValidVideoMode
//
// Set/validate the required video mode
//
//////////////////////////////////////////////////////////////////////////////////////////
void CSettingsSA::SetValidVideoMode( void )
{
    bool bValid = false;
    int iWidth, iHeight, iColorBits, iAdapterIndex;
    bool bAllowUnsafeResolutions = false;

    // First, try to get MTA saved info
    if ( !bValid )
    {
        bValid = g_pCore->GetRequiredDisplayResolution( iWidth, iHeight, iColorBits, iAdapterIndex, bAllowUnsafeResolutions );
    }

    // Otherwise deduce from GTA saved video mode
    if ( !bValid )
    {
        SetAdapter( 0 );
        uint numVidModes = GetNumVideoModes();
        if ( VAR_SavedVideoMode > 0 && VAR_SavedVideoMode < numVidModes )
        {
            VideoMode modeInfo;
            if ( GetVideoModeInfo( &modeInfo, VAR_SavedVideoMode ) )
            {
                iWidth = modeInfo.width;
                iHeight = modeInfo.height;
                iColorBits = modeInfo.depth;
                iAdapterIndex = 0;
                bValid = true;        
            }
        }
    }

    // Finally use default
    if ( !bValid )
    {
        bValid = true;
        iWidth = 800;
        iHeight = 600;
        iColorBits = 32;
        iAdapterIndex = 0;
    }

    // Set adapter
    if ( (uint)iAdapterIndex >= GetNumAdapters() )
        iAdapterIndex = 0;
    SetAdapter( iAdapterIndex );

    // Save desktop resolution
    {
        m_iDesktopWidth = 800;
        m_iDesktopHeight = 600;

        VideoMode currentModeInfo;
        if ( GetVideoModeInfo( &currentModeInfo, GetCurrentVideoMode() ) )
        {
            m_iDesktopWidth = currentModeInfo.width;
            m_iDesktopHeight = currentModeInfo.height;
        }
    }

    // Handle 'unsafe' resolution stuff
    if ( IsUnsafeResolution( iWidth, iHeight ) )
    {
        if ( bAllowUnsafeResolutions )
        {
            // Confirm that res should be used
            SString strMessage = _("Are you sure you want to use this screen resolution?" );
            strMessage += SString( "\n\n%d x %d", iWidth, iHeight );
            if ( MessageBoxUTF8( NULL, strMessage, _("MTA: San Andreas"), MB_YESNO | MB_TOPMOST | MB_ICONQUESTION ) == IDNO )
                bAllowUnsafeResolutions = false;
        }

        if ( !bAllowUnsafeResolutions )
        {
            // Force down to desktop res if required
            iWidth = m_iDesktopWidth;
            iHeight = m_iDesktopHeight;
        }
    }

    // Ensure res is no smaller than 640 x 480
    iWidth = Max( 640, iWidth );
    iHeight = Max( 480, iHeight );

    // Find mode number which best matches required settings
    uint uiUseVideoMode = FindVideoMode( iWidth, iHeight, iColorBits );

    // Set for GTA to use
    VAR_CurVideoMode = uiUseVideoMode;
    VAR_SavedVideoMode = uiUseVideoMode;
    VAR_CurAdapter = iAdapterIndex;
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CSettingsSA::OnSelectDevice
//
// return 0 for single adapter
// return 1 for multi adapter hide dialog
// return 2 for multi adapter show dialog
//
//////////////////////////////////////////////////////////////////////////////////////////
int CSettingsSA::OnSelectDevice( void )
{
    if ( GetNumAdapters() > 1 && g_pCore->GetDeviceSelectionEnabled() )
    {
        // Show device selection
        return 1;
    }

    SetValidVideoMode();

    if ( GetNumAdapters() > 1 )
    {
        // Hide device selection
        return 2;
    }
    else
    {
        return 0;
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Hook psSelectDevice so we can:
//   * Set/validate the required video mode
//   * Choose whether to show the device selection dialog box
//
//////////////////////////////////////////////////////////////////////////////////////////
int OnMY_SelectDevice( void )
{
    CSettingsSA* gameSettings = (CSettingsSA*)pGame->GetSettings();
    return gameSettings->OnSelectDevice();
}

// Hook info
#define HOOKPOS_SelectDevice             0x0746219
#define HOOKSIZE_SelectDevice            6
DWORD RETURN_SelectDeviceSingle =        0x0746273;
DWORD RETURN_SelectDeviceMultiHide =     0x074622C;
DWORD RETURN_SelectDeviceMultiShow =     0x0746227;
void _declspec(naked) HOOK_SelectDevice ()
{
    _asm
    {
        pushad
        call    OnMY_SelectDevice
        cmp     eax, 1
        popad

        jl      single
        jz      multishow

        // multhide
        mov     eax, 1
        jmp     RETURN_SelectDeviceMultiHide

multishow:
        jmp     RETURN_SelectDeviceMultiShow

single:
        jmp     RETURN_SelectDeviceSingle
    }
}


////////////////////////////////////////////////
//
// Setup hooks
//
////////////////////////////////////////////////
void CSettingsSA::StaticSetHooks ( void )
{
    EZHookInstall( SelectDevice );
}
