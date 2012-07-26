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
    // Find all TexInfo's for this txd
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
void CRenderWareSA::SetRenderingClientEntity ( CClientEntityBase* pClientEntity, bool bIsPed )
{
    m_pRenderingClientEntity = pClientEntity;
    m_bIsRenderingPed = bIsPed;
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetAppliedShaderForD3DData
//
//
//
////////////////////////////////////////////////////////////////
CSHADERDUMMY* CRenderWareSA::GetAppliedShaderForD3DData ( CD3DDUMMY* pD3DData )
{
    m_uiReplacementRequestCounter++;

    STexInfo* pTexInfo = MapFindRef ( m_D3DDataTexInfoMap, pD3DData );

    if ( !pTexInfo )
        return NULL;

    SShaderInfo* pShaderInfo = m_pMatchChannelManager->GetShaderForTexAndEntity ( pTexInfo, m_pRenderingClientEntity );

    if ( !pShaderInfo )
        return NULL;

    // Check for vertex shader rule violation
    if ( m_bIsRenderingPed && pShaderInfo->bUsesVertexShader )
        return NULL;

    m_uiReplacementMatchCounter++;

    return pShaderInfo->pShaderData;
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::AppendAdditiveMatch
//
// Add additive match for a shader+entity combo
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::AppendAdditiveMatch ( CSHADERDUMMY* pShaderData, CClientEntityBase* pClientEntity, const char* strTextureNameMatch, float fShaderPriority, uint uiShaderCreateTime, bool bShaderUsesVertexShader )
{
    TIMING_CHECKPOINT( "+AppendAddMatch" );
    m_pMatchChannelManager->AppendAdditiveMatch ( pShaderData, pClientEntity, strTextureNameMatch, fShaderPriority, uiShaderCreateTime, bShaderUsesVertexShader );
    TIMING_CHECKPOINT( "-AppendAddMatch" );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::AppendSubtractiveMatch
//
// Add subtractive match for a shader+entity combo
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::AppendSubtractiveMatch ( CSHADERDUMMY* pShaderData, CClientEntityBase* pClientEntity, const char* strTextureNameMatch )
{
    TIMING_CHECKPOINT( "+AppendSubMatch" );
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
