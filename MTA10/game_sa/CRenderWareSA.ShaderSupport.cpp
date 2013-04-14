/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*  PURPOSE:
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#include "StdInc.h"
#include "CRenderWareSA.ShaderMatching.h"

#define ADDR_CCustomCarPlateMgr_CreatePlateTexture_TextureSetName        0x06FDF40
#define ADDR_CCustomRoadsignMgr_CreateRoadsignTexture_TextureSetName     0x06FED49
#define ADDR_CClothesBuilder_ConstructTextures_Start                     0x05A6040
#define ADDR_CClothesBuilder_ConstructTextures_End                       0x05A6520
#define ADDR_CVehicle_DoHeadLightBeam_RenderPrimitive                    0x06E13CD
#define ADDR_CHeli_SearchLightCone_RenderPrimitive                       0x06C62AD
#define ADDR_CWaterCannon_Render_RenderPrimitive                         0x072956B

enum
{
    RT_NONE,
    RT_2DI,
    RT_2DNI,
    RT_3DI,
    RT_3DNI,
};

int CRenderWareSA::ms_iRenderingType = 0;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// The hooks
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

// Hooks for creating txd create and destroy events
#define HOOKPOS_CTxdStore_SetupTxdParent       0x731D55
DWORD RETURN_CTxdStore_SetupTxdParent =        0x731D5B;
#define HOOKPOS_CTxdStore_RemoveTxd         0x731E90
DWORD RETURN_CTxdStore_RemoveTxd =          0x731E96;

//
// STxdStreamEvent stores the txd create and destroy events for the current frame
//
struct STxdStreamEvent
{
    STxdStreamEvent ( bool bAdded, ushort usTxdId ) : bAdded ( bAdded ), usTxdId ( usTxdId ) {}

    bool operator < ( const STxdStreamEvent& other ) const
    {
        return usTxdId < other.usTxdId || ( usTxdId == other.usTxdId && bAdded == false && other.bAdded == true );
    }
    bool operator == ( const STxdStreamEvent& other ) const
    {
        return usTxdId == other.usTxdId && bAdded == other.bAdded;
    }

    bool bAdded;
    ushort usTxdId;
};

static CMappedArray < STxdStreamEvent > ms_txdStreamEventList;


////////////////////////////////////////////////////////////////
// Txd created
////////////////////////////////////////////////////////////////
void _cdecl OnStreamingAddedTxd ( DWORD dwTxdId )
{
    ushort usTxdId = (ushort)dwTxdId;
    // Ensure there are no previous events for this txd
    ms_txdStreamEventList.remove ( STxdStreamEvent ( false, usTxdId ) );
    ms_txdStreamEventList.remove ( STxdStreamEvent ( true, usTxdId ) );
    // Append 'added'
    ms_txdStreamEventList.push_back ( STxdStreamEvent ( true, usTxdId ) );
}

// called from streaming on TXD create
void _declspec(naked) HOOK_CTxdStore_SetupTxdParent ()
{
    _asm
    {
        // Hooked from 731D55  6 bytes

        // eax - txd id
        pushad
        push eax
        call OnStreamingAddedTxd
        add esp, 4
        popad

        // orig
        mov     esi, ds:00C8800Ch 
        jmp     RETURN_CTxdStore_SetupTxdParent  // 731D5B
    }
}


////////////////////////////////////////////////////////////////
// Txd remove
////////////////////////////////////////////////////////////////
void _cdecl OnStreamingRemoveTxd ( DWORD dwTxdId )
{
    ushort usTxdId = (ushort)dwTxdId - 20000;
    // Ensure there are no previous events for this txd
    ms_txdStreamEventList.remove ( STxdStreamEvent ( true, usTxdId ) );
    ms_txdStreamEventList.remove ( STxdStreamEvent ( false, usTxdId ) );
    // Append 'removed'
    ms_txdStreamEventList.push_back ( STxdStreamEvent ( false, usTxdId ) );
}

// called from streaming on TXD destroy
void _declspec(naked) HOOK_CTxdStore_RemoveTxd ()
{
    _asm
    {
        // Hooked from 731E90  6 bytes

        // esi - txd id + 20000
        pushad
        push esi
        call OnStreamingRemoveTxd
        add esp, 4
        popad

        // orig
        mov     ecx, ds:00C8800Ch
        jmp     RETURN_CTxdStore_RemoveTxd      // 731E96
    }
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::InitTextureWatchHooks
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::InitTextureWatchHooks ( void )
{
    HookInstall ( HOOKPOS_CTxdStore_SetupTxdParent, (DWORD)HOOK_CTxdStore_SetupTxdParent, 6 );
    HookInstall ( HOOKPOS_CTxdStore_RemoveTxd, (DWORD)HOOK_CTxdStore_RemoveTxd, 6 );
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// Process results of hooks
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::PulseWorldTextureWatch
//
// Process ms_txdStreamEventList
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::PulseWorldTextureWatch ( void )
{
    UpdateModuleTickCount64();
    UpdateDisableGTAVertexShadersTimer();

    TIMING_CHECKPOINT( "+TextureWatch" );

    // Go through ms_txdStreamEventList
    for ( std::vector < STxdStreamEvent >::const_iterator iter = ms_txdStreamEventList.begin () ; iter != ms_txdStreamEventList.end () ; ++iter )
    {
        const STxdStreamEvent& action = *iter;
        if ( action.bAdded )
        {
            //
            // New txd has been loaded
            //

            // Get list of texture names and data to add

            // Note: If txd has been unloaded since, textureList will be empty
            std::vector < RwTexture* > textureList;
            GetTxdTextures ( textureList, action.usTxdId );

            for ( std::vector < RwTexture* > ::iterator iter = textureList.begin () ; iter != textureList.end () ; iter++ )
            {
                RwTexture* texture = *iter;
                const char* szTextureName = texture->name;
                CD3DDUMMY* pD3DData = texture->raster ? (CD3DDUMMY*)texture->raster->renderResource : NULL;
                if ( !MapContains( m_SpecialTextures, texture ) )
                    StreamingAddedTexture ( action.usTxdId, szTextureName, pD3DData );
            }
        }
        else
        {
            //
            // Txd has been unloaded
            //

            StreamingRemovedTxd ( action.usTxdId );
        }
    }

    ms_txdStreamEventList.clear ();
    TIMING_CHECKPOINT( "-TextureWatch" );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::StreamingAddedTexture
//
// Called when a TXD is loaded.
// Create a texinfo for the texture
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::StreamingAddedTexture ( ushort usTxdId, const SString& strTextureName, CD3DDUMMY* pD3DData )
{
    STexInfo* pTexInfo = CreateTexInfo ( usTxdId, strTextureName, pD3DData );
    OnTextureStreamIn ( pTexInfo );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::StreamingRemovedTxd
//
// Called when a TXD is being unloaded.
// Delete texinfos which came from that TXD
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::StreamingRemovedTxd ( ushort usTxdId )
{
    TIMING_CHECKPOINT( "+StreamingRemovedTxd" );

    typedef std::multimap < ushort, STexInfo* > ::const_iterator ConstIterType;
    std::pair < ConstIterType, ConstIterType > range = m_TexInfoMap.equal_range ( usTxdId );
    for ( ConstIterType iter = range.first ; iter != range.second ; )
    {
        STexInfo* pTexInfo = iter->second;
        if ( pTexInfo->texTag == usTxdId )
        {
            OnTextureStreamOut ( pTexInfo );
            DestroyTexInfo ( pTexInfo );
            m_TexInfoMap.erase ( iter++ );
        }
        else
            ++iter;

    }

    TIMING_CHECKPOINT( "-StreamingRemovedTxd" );
}



////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ScriptAddedTxd
//
// Called when a TXD is loaded outside of streaming
// Create texinfos for the textures
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::ScriptAddedTxd ( RwTexDictionary *pTxd )
{
    TIMING_CHECKPOINT( "+ScriptAddedTxd" );
    std::vector < RwTexture* > textureList;
    GetTxdTextures ( textureList, pTxd );
    for ( std::vector < RwTexture* > ::iterator iter = textureList.begin () ; iter != textureList.end () ; iter++ )
    {
        RwTexture* texture = *iter;
        const char* szTextureName = texture->name;
        CD3DDUMMY* pD3DData = texture->raster ? (CD3DDUMMY*)texture->raster->renderResource : NULL;

        // Added texture
        STexInfo* pTexInfo = CreateTexInfo ( texture, szTextureName, pD3DData );
        OnTextureStreamIn ( pTexInfo );
    }
    TIMING_CHECKPOINT( "-ScriptAddedTxd" );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ScriptRemovedTexture
//
// Called when a texture is destroyed outside of streaming
// Delete texinfo for that texture
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::ScriptRemovedTexture ( RwTexture* pTex )
{
    TIMING_CHECKPOINT( "+ScriptRemovedTexture" );
    // Find TexInfo for this script added texture
    for ( std::multimap < ushort, STexInfo* >::iterator iter = m_TexInfoMap.begin () ; iter != m_TexInfoMap.end () ; )
    {
        STexInfo* pTexInfo = iter->second;
        if ( pTexInfo->texTag == pTex )
        {
            OnTextureStreamOut ( pTexInfo );
            DestroyTexInfo ( pTexInfo );
            m_TexInfoMap.erase ( iter++ );
        }
        else
            ++iter;
    }
    TIMING_CHECKPOINT( "-ScriptRemovedTexture" );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::SpecialAddedTexture
//
// For game textures that are created (not loaded)
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::SpecialAddedTexture ( RwTexture* texture, const char* szTextureName )
{
    if ( !szTextureName )
        szTextureName = texture->name;

    OutputDebug( SString( "Adding special texture %s", szTextureName ) );

    CD3DDUMMY* pD3DData = texture->raster ? (CD3DDUMMY*)texture->raster->renderResource : NULL;

    // Added texture
    STexInfo* pTexInfo = CreateTexInfo ( texture, szTextureName, pD3DData );
    OnTextureStreamIn ( pTexInfo );

    MapInsert( m_SpecialTextures, texture );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::SpecialRemovedTexture
//
// For game textures that are created (not loaded)
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::SpecialRemovedTexture ( RwTexture* pTex )
{
    if ( !MapContains( m_SpecialTextures, pTex ) )
        return;

    OutputDebug( SString( "Removing special texture (%s)", pTex->name ) );

    MapRemove( m_SpecialTextures, pTex );

    // Find TexInfo for this special texture
    for ( std::multimap < ushort, STexInfo* >::iterator iter = m_TexInfoMap.begin () ; iter != m_TexInfoMap.end () ; )
    {
        STexInfo* pTexInfo = iter->second;
        if ( pTexInfo->texTag == pTex )
        {
            OutputDebug( SString( "     %s", *pTexInfo->strTextureName ) );
            OnTextureStreamOut ( pTexInfo );
            DestroyTexInfo ( pTexInfo );
            m_TexInfoMap.erase ( iter++ );
        }
        else
            ++iter;
    }
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// Texture watch processing
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::CreateTexInfo
//
//
//
////////////////////////////////////////////////////////////////
STexInfo* CRenderWareSA::CreateTexInfo ( const STexTag& texTag, const SString& strTextureName, CD3DDUMMY* pD3DData )
{
    // Create texinfo
    STexInfo* pTexInfo = new STexInfo ( texTag, strTextureName, pD3DData );

    // Add to map
    MapInsert ( m_TexInfoMap, pTexInfo->texTag.m_usTxdId, pTexInfo );

    // Add to D3DData lookup map
    MapSet ( m_D3DDataTexInfoMap, pTexInfo->pD3DData, pTexInfo );
    return pTexInfo;
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::DestroyTexInfo
//
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::DestroyTexInfo ( STexInfo* pTexInfo )
{
    // Remove from D3DData lookup map
    if ( MapFindRef ( m_D3DDataTexInfoMap, pTexInfo->pD3DData ) == pTexInfo )
        MapRemove ( m_D3DDataTexInfoMap, pTexInfo->pD3DData );

    delete pTexInfo;
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::SetRenderingClientEntity
//
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::SetRenderingClientEntity ( CClientEntityBase* pClientEntity, ushort usModelId, int iTypeMask )
{
    m_pRenderingClientEntity = pClientEntity;
    m_usRenderingEntityModelId = usModelId;
    m_iRenderingEntityType = iTypeMask;
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetAppliedShaderForD3DData
//
//
//
////////////////////////////////////////////////////////////////
SShaderItemLayers* CRenderWareSA::GetAppliedShaderForD3DData ( CD3DDUMMY* pD3DData )
{
    m_uiReplacementRequestCounter++;

    // If rendering with no texture, and doing an 3d model like render, use the 'unnamed' texinfo
    if ( pD3DData == NULL && CRenderWareSA::ms_iRenderingType == RT_NONE )
        pD3DData = FAKE_D3DTEXTURE_NO_TEXTURE;

    STexInfo* pTexInfo = MapFindRef ( m_D3DDataTexInfoMap, pD3DData );

    if ( !pTexInfo )
        return NULL;

    SShaderInfoLayers* pInfoLayers = m_pMatchChannelManager->GetShaderForTexAndEntity ( pTexInfo, m_pRenderingClientEntity, m_iRenderingEntityType );

    if ( !pInfoLayers->output.pBase && pInfoLayers->output.layerList.empty () )
        return NULL;

    if ( m_iRenderingEntityType == TYPE_MASK_PED && pInfoLayers->output.bUsesVertexShader )
    {
        // If a possible conflict is detected, make sure GTA vertex shaders are disabled (effective from the next ped rendered)
        DisableGTAVertexShadersForAWhile();
    }

    m_uiReplacementMatchCounter++;

    return &pInfoLayers->output;
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::AppendAdditiveMatch
//
// Add additive match for a shader+entity combo
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::AppendAdditiveMatch ( CSHADERDUMMY* pShaderData, CClientEntityBase* pClientEntity, const char* szTextureNameMatch, float fShaderPriority, bool bShaderLayered, int iTypeMask, uint uiShaderCreateTime, bool bShaderUsesVertexShader, bool bAppendLayers )
{
    TIMING_CHECKPOINT( "+AppendAddMatch" );

    // Make previous versions usage of "CJ" work with new way
    SString strTextureNameMatch = szTextureNameMatch;
    if ( strTextureNameMatch.CompareI( "cj" ) )
        strTextureNameMatch = "cj_ped_*";

    m_pMatchChannelManager->AppendAdditiveMatch ( pShaderData, pClientEntity, strTextureNameMatch, fShaderPriority, bShaderLayered, iTypeMask, uiShaderCreateTime, bShaderUsesVertexShader, bAppendLayers );
    TIMING_CHECKPOINT( "-AppendAddMatch" );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::AppendSubtractiveMatch
//
// Add subtractive match for a shader+entity combo
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::AppendSubtractiveMatch ( CSHADERDUMMY* pShaderData, CClientEntityBase* pClientEntity, const char* szTextureNameMatch )
{
    TIMING_CHECKPOINT( "+AppendSubMatch" );

    // Make previous versions usage of "CJ" work with new way
    SString strTextureNameMatch = szTextureNameMatch;
    if ( strTextureNameMatch.CompareI( "cj" ) )
        strTextureNameMatch = "cj_ped_*";

    m_pMatchChannelManager->AppendSubtractiveMatch ( pShaderData, pClientEntity, strTextureNameMatch );
    TIMING_CHECKPOINT( "-AppendSubMatch" );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::OnTextureStreamIn
//
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::OnTextureStreamIn ( STexInfo* pTexInfo )
{
    // Insert into all channels that match the name
    m_pMatchChannelManager->InsertTexture ( pTexInfo );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::OnTextureStreamOut
//
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::OnTextureStreamOut ( STexInfo* pTexInfo )
{
    m_pMatchChannelManager->RemoveTexture ( pTexInfo );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::RemoveClientEntityRefs
//
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::RemoveClientEntityRefs ( CClientEntityBase* pClientEntity )
{
    TIMING_CHECKPOINT( "+RemoveEntityRefs" );
    m_pMatchChannelManager->RemoveClientEntityRefs ( pClientEntity );
    TIMING_CHECKPOINT( "-RemoveEntityRefs" );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::RemoveShaderRefs
//
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::RemoveShaderRefs ( CSHADERDUMMY* pShaderItem )
{
    TIMING_CHECKPOINT( "+RemoveShaderRefs" );
    m_pMatchChannelManager->RemoveShaderRefs ( pShaderItem );
    TIMING_CHECKPOINT( "-RemoveShaderRefs" );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ResetStats
//
// Save/reset counters
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::ResetStats ( void )
{
    m_uiNumReplacementRequests = m_uiReplacementRequestCounter;
    m_uiNumReplacementMatches = m_uiReplacementMatchCounter;
    m_uiReplacementRequestCounter = 0;
    m_uiReplacementMatchCounter = 0;
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetShaderReplacementStats
//
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::GetShaderReplacementStats ( SShaderReplacementStats& outStats )
{
    outStats.uiNumReplacementRequests = m_uiNumReplacementRequests;
    outStats.uiNumReplacementMatches = m_uiNumReplacementMatches;
    m_pMatchChannelManager->GetShaderReplacementStats ( outStats );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::Initialize
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::Initialize ( void )
{
    if ( !MapContains( m_D3DDataTexInfoMap, FAKE_D3DTEXTURE_NO_TEXTURE ) )
    {
        // Make a fake texinfo to handle all non-textures
        STexInfo* pTexInfo = CreateTexInfo( FAKE_RWTEXTURE_NO_TEXTURE, FAKE_NAME_NO_TEXTURE, FAKE_D3DTEXTURE_NO_TEXTURE );
        OnTextureStreamIn( pTexInfo );
    }
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::DisableGTAVertexShadersForAWhile
//
// Disable GTA vertex shaders for the next 10 seconds
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::DisableGTAVertexShadersForAWhile( void )
{
    m_GTAVertexShadersDisabledTimer.Reset();
    SetGTAVertexShadersEnabled( false );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::UpdateDisableGTAVertexShadersTimer
//
// Update countdown before automatically re enabling GTA vertex shaders
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::UpdateDisableGTAVertexShadersTimer( void )
{
#if MTA_DEBUG
    if ( m_GTAVertexShadersDisabledTimer.Get() > 1 * 1000 )
#else
    if ( m_GTAVertexShadersDisabledTimer.Get() > 10 * 1000 )
#endif
    {
        SetGTAVertexShadersEnabled( true );
    }
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::SetGTAVertexShadersEnabled
//
// Set to false to disable GTA vertex shaders, so we can use our own ones
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::SetGTAVertexShadersEnabled( bool bEnable )
{
    if ( m_bGTAVertexShadersEnabled == bEnable )
        return;

    m_bGTAVertexShadersEnabled = bEnable;
 
    DWORD pSkinAtomic = 0x07C7CD0;
    if ( pGame->GetGameVersion () != VERSION_US_10 )
        pSkinAtomic = 0x07C7D10;

    if ( bEnable )
    {
        // Allow GTA vertex shaders (default)
        MemPut < BYTE > ( pSkinAtomic + 0, 0x8B );  // mov  eax, [edi+20h]
        MemPut < BYTE > ( pSkinAtomic + 1, 0x47 );
        MemPut < BYTE > ( pSkinAtomic + 2, 0x20 );
        MemPut < BYTE > ( pSkinAtomic + 3, 0x85 );  // test eax, eax
        MemPut < BYTE > ( pSkinAtomic + 4, 0xC0 );
    }
    else
    {
        // Disallow GTA vertex shaders
        // This forces the current skin buffer to use software blending from now on
        MemPut < BYTE > ( pSkinAtomic + 0, 0x33 );  // xor  eax, eax
        MemPut < BYTE > ( pSkinAtomic + 1, 0xC0 );
        MemPut < BYTE > ( pSkinAtomic + 2, 0x89 );  // mov  dword ptr [edi+20h], eax 
        MemPut < BYTE > ( pSkinAtomic + 3, 0x47 );
        MemPut < BYTE > ( pSkinAtomic + 4, 0x20 );
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// RwTextureSetName
//
// Check each created texture, so we can add special ones to the shader system
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_RwTextureSetName( DWORD dwAddrCalledFrom, RwTexture* pTexture, const char* szName )
{
    SString strReplacementName;
    if ( dwAddrCalledFrom == ADDR_CCustomCarPlateMgr_CreatePlateTexture_TextureSetName )
        strReplacementName = "custom_car_plate";
    else
    if ( dwAddrCalledFrom == ADDR_CCustomRoadsignMgr_CreateRoadsignTexture_TextureSetName )
        strReplacementName = "custom_roadsign_text";
    else
    if ( dwAddrCalledFrom > ADDR_CClothesBuilder_ConstructTextures_Start && dwAddrCalledFrom < ADDR_CClothesBuilder_ConstructTextures_End )
        strReplacementName = SString( "cj_ped_%s", szName );

    if ( !strReplacementName.empty() )
        pGame->GetRenderWareSA()->SpecialAddedTexture( pTexture, strReplacementName );
}

// Hook info
#define HOOKPOS_RwTextureSetName_US     0x7F38A0
#define HOOKSIZE_RwTextureSetName_US    9
#define HOOKPOS_RwTextureSetName_EU     0x7F38D0
#define HOOKSIZE_RwTextureSetName_EU    9
DWORD RETURN_RwTextureSetName_US =      0x7F38A9;
DWORD RETURN_RwTextureSetName_EU =      0x7F38D9;
DWORD RETURN_RwTextureSetName_BOTH =    0;
void _declspec(naked) HOOK_RwTextureSetName ()
{
    _asm
    {
        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        call    OnMY_RwTextureSetName
        add     esp, 4*3
        popad

        sub     esp, 8
        mov     ecx, ds:0x0C97B24
        jmp     RETURN_RwTextureSetName_BOTH
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// RwTextureDestroy_Mid
//
// Check each destroyed texture, so we can remove special ones from the shader system
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_RwTextureDestroy_Mid( RwTexture* pTexture )
{
    pGame->GetRenderWareSA()->SpecialRemovedTexture( pTexture );
}

// Hook info
#define HOOKPOS_RwTextureDestroy_Mid_US     0x07F3834
#define HOOKSIZE_RwTextureDestroy_Mid_US    5
#define HOOKPOS_RwTextureDestroy_Mid_EU     0x07F3874
#define HOOKSIZE_RwTextureDestroy_Mid_EU    5
DWORD RETURN_RwTextureDestroy_Mid_US =      0x07F3839;
DWORD RETURN_RwTextureDestroy_Mid_EU =      0x07F3879;
DWORD RETURN_RwTextureDestroy_Mid_BOTH =    0;
void _declspec(naked) HOOK_RwTextureDestroy_Mid ()
{
    _asm
    {
        pushad
        push    esi
        call    OnMY_RwTextureDestroy_Mid
        add     esp, 4*1
        popad

        push    0x08E23CC
        jmp     RETURN_RwTextureDestroy_Mid_BOTH
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// RwIm3DRenderIndexedPrimitive
//
// Classify what is going on here
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_RwIm3DRenderIndexedPrimitive_Pre( DWORD dwAddrCalledFrom )
{
    if ( dwAddrCalledFrom == ADDR_CVehicle_DoHeadLightBeam_RenderPrimitive
        || dwAddrCalledFrom == ADDR_CHeli_SearchLightCone_RenderPrimitive
        || dwAddrCalledFrom == ADDR_CWaterCannon_Render_RenderPrimitive )
    {
        CRenderWareSA::ms_iRenderingType = RT_NONE; // Treat these items like world models
    }
    else
    {
        CRenderWareSA::ms_iRenderingType = RT_3DI;
    }
}

void OnMY_RwIm3DRenderIndexedPrimitive_Post( DWORD dwAddrCalledFrom )
{
    CRenderWareSA::ms_iRenderingType = RT_NONE;
}

// Hook info
#define HOOKPOS_RwIm3DRenderIndexedPrimitive_US     0x07EF550
#define HOOKSIZE_RwIm3DRenderIndexedPrimitive_US    5
#define HOOKPOS_RwIm3DRenderIndexedPrimitive_EU     0x07EF590
#define HOOKSIZE_RwIm3DRenderIndexedPrimitive_EU    5
DWORD RETURN_RwIm3DRenderIndexedPrimitive_US =      0x07EF555;
DWORD RETURN_RwIm3DRenderIndexedPrimitive_EU =      0x07EF595;
DWORD RETURN_RwIm3DRenderIndexedPrimitive_BOTH =    0;
void _declspec(naked) HOOK_RwIm3DRenderIndexedPrimitive ()
{
    _asm
    {
        pushad
        push    [esp+32+4*0]
        call    OnMY_RwIm3DRenderIndexedPrimitive_Pre
        add     esp, 4*1
        popad

        push    [esp+4*3]
        push    [esp+4*3]
        push    [esp+4*3]
        call inner
        add     esp, 4*3

        pushad
        push    [esp+32+4*0]
        call    OnMY_RwIm3DRenderIndexedPrimitive_Post
        add     esp, 4*1
        popad
        retn
inner:
        mov     eax, ds:0x0C9C078
        jmp     RETURN_RwIm3DRenderIndexedPrimitive_BOTH
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// RwIm3DRenderPrimitive
//
// Note that RwIm3DRenderPrimitive is being called to render something
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_RwIm3DRenderPrimitive_Pre( DWORD dwAddrCalledFrom )
{
    CRenderWareSA::ms_iRenderingType = RT_3DNI;
}

void OnMY_RwIm3DRenderPrimitive_Post( DWORD dwAddrCalledFrom )
{
    CRenderWareSA::ms_iRenderingType = RT_NONE;
}

// Hook info
#define HOOKPOS_RwIm3DRenderPrimitive_US    0x07EF6B0
#define HOOKSIZE_RwIm3DRenderPrimitive_US   6
#define HOOKPOS_RwIm3DRenderPrimitive_EU    0x07EF6F0
#define HOOKSIZE_RwIm3DRenderPrimitive_EU   6
DWORD RETURN_RwIm3DRenderPrimitive_US =     0x07EF6B6;
DWORD RETURN_RwIm3DRenderPrimitive_EU =     0x07EF6F6;
DWORD RETURN_RwIm3DRenderPrimitive_BOTH =   0;
void _declspec(naked) HOOK_RwIm3DRenderPrimitive ()
{
    _asm
    {
        pushad
        push    [esp+32+4*0]
        call    OnMY_RwIm3DRenderPrimitive_Pre
        add     esp, 4*1
        popad

        push    [esp+4*3]
        push    [esp+4*3]
        push    [esp+4*3]
        call inner
        add     esp, 4*3

        pushad
        push    [esp+32+4*0]
        call    OnMY_RwIm3DRenderPrimitive_Post
        add     esp, 4*1
        popad
        retn
inner:
        mov     ecx, ds:0x0C97B24
        jmp     RETURN_RwIm3DRenderPrimitive_BOTH
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// RwIm2DRenderIndexedPrimitive
//
// Note that RwIm2DRenderIndexedPrimitive is being called to render something
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_RwIm2DRenderIndexedPrimitive_Pre( DWORD dwAddrCalledFrom )
{
    CRenderWareSA::ms_iRenderingType = RT_2DI;
}

void OnMY_RwIm2DRenderIndexedPrimitive_Post( DWORD dwAddrCalledFrom )
{
    CRenderWareSA::ms_iRenderingType = RT_NONE;
}

// Hook info
#define HOOKPOS_RwIm2DRenderIndexedPrimitive_US     0x0734EA1
#define HOOKSIZE_RwIm2DRenderIndexedPrimitive_US    5
#define HOOKPOS_RwIm2DRenderIndexedPrimitive_EU     0x0734EA1
#define HOOKSIZE_RwIm2DRenderIndexedPrimitive_EU    5
DWORD RETURN_RwIm2DRenderIndexedPrimitive_US =      0x0403927;
DWORD RETURN_RwIm2DRenderIndexedPrimitive_EU =      0x0403937;
DWORD RETURN_RwIm2DRenderIndexedPrimitive_BOTH =    0;
void _declspec(naked) HOOK_RwIm2DRenderIndexedPrimitive ()
{
    _asm
    {
        pushad
        push    [esp+32+4*0]
        call    OnMY_RwIm2DRenderIndexedPrimitive_Pre
        add     esp, 4*1
        popad

        push    [esp+4*5]
        push    [esp+4*5]
        push    [esp+4*5]
        push    [esp+4*5]
        push    [esp+4*5]
        call inner
        add     esp, 4*5

        pushad
        push    [esp+32+4*0]
        call    OnMY_RwIm2DRenderIndexedPrimitive_Post
        add     esp, 4*1
        popad
        retn

inner:
        jmp     RETURN_RwIm2DRenderIndexedPrimitive_BOTH
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// RwIm2DRenderPrimitive
//
// Note that RwIm2DRenderPrimitive is being called to render something
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_RwIm2DRenderPrimitive_Pre( DWORD dwAddrCalledFrom )
{
    CRenderWareSA::ms_iRenderingType = RT_2DNI;
}

void OnMY_RwIm2DRenderPrimitive_Post( DWORD dwAddrCalledFrom )
{
    CRenderWareSA::ms_iRenderingType = RT_NONE;
}

// Hook info
#define HOOKPOS_RwIm2DRenderPrimitive                0x0734E90
#define HOOKSIZE_RwIm2DRenderPrimitive               5
DWORD RETURN_RwIm2DRenderPrimitive =                 0x0734E95;
void _declspec(naked) HOOK_RwIm2DRenderPrimitive ()
{
    _asm
    {
        pushad
        push    [esp+32+4*0]
        call    OnMY_RwIm2DRenderPrimitive_Pre
        add     esp, 4*1
        popad

        push    [esp+4*3]
        push    [esp+4*3]
        push    [esp+4*3]
        call inner
        add     esp, 4*3

        pushad
        push    [esp+32+4*0]
        call    OnMY_RwIm2DRenderPrimitive_Post
        add     esp, 4*1
        popad
        retn

inner:
        mov     eax, ds:0x0C97B24
        jmp     RETURN_RwIm2DRenderPrimitive
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CRenderWareSA::StaticSetHooks( void )
{
   EZHookInstall( RwTextureSetName );
   EZHookInstall( RwTextureDestroy_Mid );
   EZHookInstall( RwIm3DRenderIndexedPrimitive );
   EZHookInstall( RwIm3DRenderPrimitive );
   EZHookInstall( RwIm2DRenderIndexedPrimitive );
   EZHookInstall( RwIm2DRenderPrimitive );
}
