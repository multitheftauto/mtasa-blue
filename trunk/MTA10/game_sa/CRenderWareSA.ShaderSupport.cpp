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
    bool bAdded;
    ushort usTxdId;
};

static std::vector < STxdStreamEvent > ms_txdStreamEventList;


////////////////////////////////////////////////////////////////
// Txd created
////////////////////////////////////////////////////////////////
void _cdecl OnStreamingAddedTxd ( DWORD dwTxdId )
{
    STxdStreamEvent action;
    action.bAdded = true;
    action.usTxdId = (ushort)dwTxdId;
    ms_txdStreamEventList.push_back ( action );
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
    STxdStreamEvent action;
    action.bAdded = false;
    action.usTxdId = (ushort)dwTxdId - 20000;
    ms_txdStreamEventList.push_back ( action );
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
    FlushPendingAssociations ();

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
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::StreamingAddedTexture
//
// Called when a TXD is loaded.
// Create a texinfo for the texture and find a best match shader for it
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::StreamingAddedTexture ( ushort usTxdId, const SString& strTextureName, CD3DDUMMY* pD3DData )
{
    STexInfo* pTexInfo = CreateTexInfo ( usTxdId, strTextureName, pD3DData );
    UpdateAssociationForTexInfo ( pTexInfo );
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
    // Find all TexInfo's for this txd
    for ( std::list < STexInfo > ::iterator iter = m_TexInfoList.begin () ; iter != m_TexInfoList.end () ; )
    {
        STexInfo* pTexInfo = &*iter;
        if ( pTexInfo->texTag.Matches ( usTxdId ) )
        {
            // If texinfo has a shadinfo, unassociate
            if ( SShadInfo* pShadInfo = pTexInfo->pAssociatedShadInfo )
            {
                BreakAssociation ( pShadInfo, pTexInfo );
            }

            OnDestroyTexInfo ( pTexInfo );
            iter = m_TexInfoList.erase ( iter );
        }
        else
            ++iter;
    }
}



////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ScriptAddedTxd
//
// Called when a TXD is loaded outside of streaming
// Create texinfos for the textures and find a best match shaders for them
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::ScriptAddedTxd ( RwTexDictionary *pTxd )
{
    std::vector < RwTexture* > textureList;
    GetTxdTextures ( textureList, pTxd );
    for ( std::vector < RwTexture* > ::iterator iter = textureList.begin () ; iter != textureList.end () ; iter++ )
    {
        RwTexture* texture = *iter;
        const char* szTextureName = texture->name;
        CD3DDUMMY* pD3DData = texture->raster ? (CD3DDUMMY*)texture->raster->renderResource : NULL;

        // Added texture
        STexInfo* pTexInfo = CreateTexInfo ( texture, szTextureName, pD3DData );
        UpdateAssociationForTexInfo ( pTexInfo );
    }
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
    // Find all TexInfo's for this txd
    for ( std::list < STexInfo > ::iterator iter = m_TexInfoList.begin () ; iter != m_TexInfoList.end () ; )
    {
        STexInfo* pTexInfo = &*iter;
        if ( pTexInfo->texTag.Matches ( pTex ) )
        {
            // If texinfo has a shadinfo, unassociate
            if ( SShadInfo* pShadInfo = pTexInfo->pAssociatedShadInfo )
            {
                BreakAssociation ( pShadInfo, pTexInfo );
            }

            OnDestroyTexInfo ( pTexInfo );
            iter = m_TexInfoList.erase ( iter );
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
// CRenderWareSA::InitWorldTextureWatch
//
// Setup texture watch callback
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::InitWorldTextureWatch ( PFN_WATCH_CALLBACK pfnWatchCallback )
{
    m_pfnWatchCallback = pfnWatchCallback;
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::AddWorldTextureWatch
//
// Begin watching for changes to the d3d resource associated with this texture name 
//
// Returns false if shader/match already exists
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::AddWorldTextureWatch ( CSHADERDUMMY* pShaderData, const char* szMatch, float fShaderPriority )
{
    FlushPendingAssociations ();

    // Get info for this shader
    SShadInfo* pShadInfo = GetShadInfo ( pShaderData, true, fShaderPriority );

    // Add this string match to the list
    pShadInfo->AppendMatchType ( szMatch, true );

    // Step through texinfo list looking for all add matches
    for ( std::list < STexInfo > ::iterator iter = m_TexInfoList.begin () ; iter != m_TexInfoList.end () ; ++iter )
    {
        STexInfo* pTexInfo = &*iter;

        // No need to check if aleady associated
        if ( pShadInfo == pTexInfo->pAssociatedShadInfo )
            continue;

        // Only need to check if this new string match will make an association
        if ( WildcardMatch ( szMatch, pTexInfo->strTextureName ) )
        {
            // Check previous association
            if ( SShadInfo* pPrevShadInfo = pTexInfo->pAssociatedShadInfo )
            {
				// Check priority
                if ( IsFirstShadInfoHigherOrSamePriority ( pPrevShadInfo, pShadInfo ) )
                    continue;   // Previous shadinfo has a higher priority

                // Remove previous association
                BreakAssociation ( pPrevShadInfo, pTexInfo );
            }

            // Add new association
            MakeAssociation ( pShadInfo, pTexInfo );
        }
    }

    return true;
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::RemoveWorldTextureWatch
//
// End watching for changes to the d3d resource associated with this texture name
//
// When shadinfo removed:
//      unassociate with matches, let each texture find a new latest match
//
// If szMatch is NULL, remove all usage of the shader
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::RemoveWorldTextureWatch ( CSHADERDUMMY* pShaderData, const char* szMatch )
{
    SShadInfo* pShadInfo = GetShadInfo ( pShaderData, false, 0 );
    if ( !pShadInfo )
        return;

    if ( szMatch )
    {
        // Append subtractive match
        pShadInfo->AppendMatchType ( szMatch, false );
    }
    else
    {
        // Remove all matches
        pShadInfo->matchTypeList.clear ();
    }

    // Add to list of texinfos that will need updating
    std::set < STexInfo* > associatedTexInfoMap = pShadInfo->associatedTexInfoMap;
    for ( std::set < STexInfo* > ::iterator iter = associatedTexInfoMap.begin () ; iter != associatedTexInfoMap.end () ; ++iter )
    {
        m_PendingTexInfoMap.insert ( *iter );
    }

    m_PendingShadInfoMap.insert ( pShadInfo );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::RemoveWorldTextureWatchByContext
//
// End watching for changes to the d3d resource associated with this context 
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::RemoveWorldTextureWatchByContext ( CSHADERDUMMY* pShaderData )
{
    RemoveWorldTextureWatch ( pShaderData, NULL );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::UpdateAssociationForTexInfo
//
// Refresh association for this texture
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::UpdateAssociationForTexInfo ( STexInfo* pTexInfo )
{
    // Check current association if no longer a match
    if ( SShadInfo* pCurrentShadInfo = pTexInfo->pAssociatedShadInfo )
    {
        if ( !pCurrentShadInfo->IsAdditiveMatch ( pTexInfo->strTextureName ) )
        {
            BreakAssociation ( pCurrentShadInfo, pTexInfo );
        }
    }

    // Stepping backwards will bias high priority shaders, and if the priorty is the same, bias the latest additions
    for ( std::multimap < float, SShadInfo* > ::reverse_iterator iter = m_OrderMap.rbegin () ; iter != m_OrderMap.rend () ; ++iter )
    {
        SShadInfo* pShadInfo = iter->second;
        if ( pShadInfo->IsAdditiveMatch ( pTexInfo->strTextureName ) )
        {
            // Found one

            // Check previous association
            if ( SShadInfo* pPrevShadInfo = pTexInfo->pAssociatedShadInfo )
            {
				// Check if any change required
                if ( pShadInfo == pTexInfo->pAssociatedShadInfo )
                    return; // No change

                // Remove previous association
                BreakAssociation ( pPrevShadInfo, pTexInfo );
            }

            // Add new association
            MakeAssociation ( pShadInfo, pTexInfo );
            return; // Done
        }
    }
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::MakeAssociation
//
// Make the texture use the shader
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::MakeAssociation ( SShadInfo* pShadInfo, STexInfo* pTexInfo )
{
    assert ( !pTexInfo->pAssociatedShadInfo );
    assert ( !MapContains ( pShadInfo->associatedTexInfoMap, pTexInfo ) );
    pTexInfo->pAssociatedShadInfo = pShadInfo;
    pShadInfo->associatedTexInfoMap.insert ( pTexInfo );

    if ( m_pfnWatchCallback )
        m_pfnWatchCallback ( pShadInfo->pShaderData, pTexInfo->pD3DData, NULL );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::BreakAssociation
//
// Stop the texture using the shader
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::BreakAssociation ( SShadInfo* pShadInfo, STexInfo* pTexInfo )
{
    assert ( pTexInfo->pAssociatedShadInfo == pShadInfo );
    assert ( MapContains ( pShadInfo->associatedTexInfoMap, pTexInfo ) );
    pTexInfo->pAssociatedShadInfo = NULL;
    MapRemove ( pShadInfo->associatedTexInfoMap, pTexInfo );
    if ( m_pfnWatchCallback )
        m_pfnWatchCallback ( pShadInfo->pShaderData, NULL, pTexInfo->pD3DData );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::IsFirstShadInfoHigherOrSamePriority
//
//
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::IsFirstShadInfoHigherOrSamePriority ( SShadInfo* pShadInfoA, SShadInfo* pShadInfoB )
{
    // Check if same priority
    if ( pShadInfoA == pShadInfoB )
        return true;

    // Check defo higher priority
    if ( pShadInfoA->fOrderPriority > pShadInfoB->fOrderPriority )
        return true;

    // Check defo lower priority
    if ( pShadInfoA->fOrderPriority < pShadInfoB->fOrderPriority )
        return false;

    // Check age
    for ( std::multimap < float, SShadInfo* > ::reverse_iterator iter = m_OrderMap.rbegin () ; iter != m_OrderMap.rend () ; ++iter )
    {
        if ( iter->second == pShadInfoA )
            return true;    // A is latest addition
        if ( iter->second == pShadInfoB )
            return false;   // B is latest addition
    }
    assert ( 0 );
    return false;
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::FlushPendingAssociations
//
// Process pending lists
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::FlushPendingAssociations ( void )
{
    if ( m_PendingTexInfoMap.empty () )
        return;

    // Process pending texinfos
    for ( std::set < STexInfo* > ::iterator iter = m_PendingTexInfoMap.begin () ; iter != m_PendingTexInfoMap.end () ; ++iter )
        UpdateAssociationForTexInfo ( *iter );

    m_PendingTexInfoMap.clear ();

    // Check if any pending shadinfos are now empty
    for ( std::set < SShadInfo* > ::iterator iter = m_PendingShadInfoMap.begin () ; iter != m_PendingShadInfoMap.end () ; ++iter )
    {
        SShadInfo* pShadInfo = *iter;
        if ( pShadInfo->associatedTexInfoMap.empty () )
            OnDestroyShadInfo ( pShadInfo );
    }

    m_PendingShadInfoMap.clear ();
}


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
    m_TexInfoList.push_back ( STexInfo ( texTag, strTextureName, pD3DData ) );
    STexInfo* pTexInfo = &m_TexInfoList.back ();

    // Add to lookup maps
#if WITH_UNIQUE_CHECK
    SString strUniqueKey ( "%d_%08x_%s", pTexInfo->texTag.m_usTxdId, pTexInfo->texTag.m_pTex, *pTexInfo->strTextureName );
    if ( MapContains ( m_UniqueTexInfoMap, strUniqueKey ) )
        AddReportLog ( 5132, SString ( "CreateTexInfo duplicate %s", *strUniqueKey ) );
    MapSet ( m_UniqueTexInfoMap, strUniqueKey, pTexInfo );
#endif

    // Add to D3DData/name lookup map. (Currently only used by GetTextureName (CRenderItemManager::GetVisibleTextureNames))
    MapSet ( m_D3DDataTexInfoMap, pTexInfo->pD3DData, pTexInfo );
    return pTexInfo;
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::OnDestroyTexInfo
//
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::OnDestroyTexInfo ( STexInfo* pTexInfo )
{
    // Remove from lookup maps
#if WITH_UNIQUE_CHECK
    SString strUniqueKey ( "%d_%08x_%s", pTexInfo->texTag.m_usTxdId, pTexInfo->texTag.m_pTex, *pTexInfo->strTextureName );
    if ( !MapContains ( m_UniqueTexInfoMap, strUniqueKey ) )
        AddReportLog ( 5133, SString ( "OnDestroyTexInfo missing %s", *strUniqueKey ) );
    MapRemove ( m_UniqueTexInfoMap, strUniqueKey );
#endif

    // Remove from D3DData/name lookup map
    if ( MapFindRef ( m_D3DDataTexInfoMap, pTexInfo->pD3DData ) == pTexInfo )
        MapRemove ( m_D3DDataTexInfoMap, pTexInfo->pD3DData );
    else
        pTexInfo = pTexInfo;
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetShadInfo
//
// Get SShadInfo linked with the pShaderData
//
////////////////////////////////////////////////////////////////
SShadInfo* CRenderWareSA::GetShadInfo ( CSHADERDUMMY* pShaderData, bool bAddIfRequired, float fPriority )
{
    // Find existing
    SShadInfo* pShadInfo = MapFindRef ( m_ShadInfoMap, pShaderData );
    if ( !pShadInfo && bAddIfRequired )
    {
        // Add new
        MapSet ( m_ShadInfoMap, pShaderData, new SShadInfo ( pShaderData, fPriority ) );
        pShadInfo = MapFindRef ( m_ShadInfoMap, pShaderData );

        // Add to order map
        MapInsert ( m_OrderMap, pShadInfo->fOrderPriority, pShadInfo );
    }

    return pShadInfo;
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::OnDestroyShadInfo
//
//
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::OnDestroyShadInfo ( SShadInfo* pShadInfo )
{
    // Remove from lookup maps
    assert ( MapContains ( m_ShadInfoMap, pShadInfo->pShaderData ) );
    MapRemove ( m_ShadInfoMap, pShadInfo->pShaderData );

    //  Remove from order map
    for ( std::multimap < float, SShadInfo* > ::iterator iter = m_OrderMap.begin () ; iter != m_OrderMap.end () ; )
    {
        if ( iter->second == pShadInfo )
            m_OrderMap.erase ( iter++ );
        else
            ++iter;
    }
    delete pShadInfo;
}
