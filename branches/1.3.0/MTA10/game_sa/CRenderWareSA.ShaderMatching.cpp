/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.ShaderMatching.cpp
*
*****************************************************************************/

#include "StdInc.h"
#include "CRenderWareSA.ShaderMatching.h"

uint CMatchChannel::ms_uiIdCounter = 1;


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::AppendAdditiveMatch
//
// Add additive match for a shader+entity combo
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::AppendAdditiveMatch ( CSHADERDUMMY* pShaderData, CClientEntityBase* pClientEntity, const SString& strTextureNameMatch, float fShaderPriority, bool bShaderLayered, int iTypeMask, uint uiShaderCreateTime, bool bShaderUsesVertexShader, bool bAppendLayers )
{
    SShaderInfo* pShaderInfo = GetShaderInfo ( pShaderData, true, fShaderPriority, bShaderLayered, iTypeMask, uiShaderCreateTime, bShaderUsesVertexShader );

    // Make channel unique before modifying match chain
    CMatchChannel* pChannel = GetChannelOnlyUsedBy ( pShaderInfo, pClientEntity, bAppendLayers );
    pChannel->AppendAdditiveMatch ( strTextureNameMatch.ToLower () );
    AddToOptimizeQueue ( pChannel );
    AddToRematchQueue ( pChannel );

    // Keep a list of entites we know about, so we can ignore the rest
    if ( pClientEntity )
        MapInsert ( m_KnownClientEntities, pClientEntity );
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::AppendSubtractiveMatch
//
// Add subtractive match for a shader+entity combo
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::AppendSubtractiveMatch ( CSHADERDUMMY* pShaderData, CClientEntityBase* pClientEntity, const SString& strTextureNameMatch )
{
    bool bAppendLayers = false;

    // Don't bother if shader hasn't been seen before
    SShaderInfo* pShaderInfo = GetShaderInfo ( pShaderData, false, 0, false, TYPE_MASK_NONE, 0, false );
    if ( !pShaderInfo )
        return;

    // Don't bother if there are no existing additive matches for this pair
    if ( !MapContains ( m_ChannelUsageMap, CShaderAndEntityPair ( pShaderInfo, pClientEntity, bAppendLayers ) ) )
        return;

    // Make channel unique before modifying match chain
    CMatchChannel* pChannel = GetChannelOnlyUsedBy ( pShaderInfo, pClientEntity, bAppendLayers );
    pChannel->AppendSubtractiveMatch ( strTextureNameMatch.ToLower () );
    AddToOptimizeQueue ( pChannel );
    AddToRematchQueue ( pChannel );
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::InsertTexture
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::InsertTexture ( STexInfo* pTexInfo )
{
    // Find/create TexNameInfo
    STexNameInfo* pTexNameInfo = MapFindRef ( m_AllTextureList, pTexInfo->strTextureName );
    if ( !pTexNameInfo )
    {
        // Create TexNameInfo
        MapSet ( m_AllTextureList, pTexInfo->strTextureName, new STexNameInfo ( pTexInfo->strTextureName ) );
        pTexNameInfo = MapFindRef ( m_AllTextureList, pTexInfo->strTextureName );

        // Insert into existing channels
        for ( std::set < CMatchChannel* >::iterator iter = m_CreatedChannelList.begin () ; iter != m_CreatedChannelList.end () ; ++iter )
        {
            CMatchChannel* pChannel = *iter;
            if ( pChannel->m_MatchChain.IsAdditiveMatch ( pTexNameInfo->strTextureName ) )
            {
                pChannel->AddTexture ( pTexNameInfo );
                MapInsert ( pTexNameInfo->matchChannelList, pChannel );
            }
        }
    }

    // Add association
    MapInsert ( pTexNameInfo->usedByTexInfoList, pTexInfo );
    pTexInfo->pAssociatedTexNameInfo = pTexNameInfo;
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::RemoveTexture
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::RemoveTexture ( STexInfo* pTexInfo )
{
    STexNameInfo* pTexNameInfo = pTexInfo->pAssociatedTexNameInfo;

    // Remove association
    dassert ( MapContains ( pTexNameInfo->usedByTexInfoList, pTexInfo ) );
    MapRemove ( pTexNameInfo->usedByTexInfoList, pTexInfo );
    pTexInfo->pAssociatedTexNameInfo = NULL;
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::FinalizeLayers
//
// Sort layers and generate output
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::FinalizeLayers ( SShaderInfoLayers& shaderLayers )
{
    // Sort layers by priority
    std::sort ( shaderLayers.layerList.begin (), shaderLayers.layerList.end () );
    uint uiNumLayers = shaderLayers.layerList.size ();

    // Set output
    shaderLayers.output = SShaderItemLayers ();
    shaderLayers.output.layerList.resize ( uiNumLayers );

    // Copy base
    SShaderInfo* pShaderInfo = shaderLayers.pBase.pShaderInfo;
    if ( pShaderInfo )
    {
        shaderLayers.output.pBase = pShaderInfo->pShaderData;
        shaderLayers.output.bUsesVertexShader = pShaderInfo->bUsesVertexShader;
    }

    // Copy layers
    for ( uint i = 0 ; i < uiNumLayers ; i++ )
    {
        SShaderInfo* pShaderInfo = shaderLayers.layerList[i].pShaderInfo;
        shaderLayers.output.layerList[i] = pShaderInfo->pShaderData;
        shaderLayers.output.bUsesVertexShader |= pShaderInfo->bUsesVertexShader;
    }
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::UpdateTexShaderReplacement
//
//
//
//////////////////////////////////////////////////////////////////
STexShaderReplacement* CMatchChannelManager::UpdateTexShaderReplacement ( STexNameInfo* pTexNameInfo, CClientEntityBase* pClientEntity, int iEntityType )
{
    STexShaderReplacement* pTexShaderReplacement = MapFind ( pTexNameInfo->texEntityShaderMap, pClientEntity );

    if ( !pTexShaderReplacement )
    {
        // If not done yet for this entity, needs to be done
        MapSet ( pTexNameInfo->texEntityShaderMap, pClientEntity, STexShaderReplacement () );
        pTexShaderReplacement = MapFind ( pTexNameInfo->texEntityShaderMap, pClientEntity );
    }

    if ( !pTexShaderReplacement->bSet )
    {
        // If not done yet for this entity, needs to be done
        dassert ( !pTexShaderReplacement->shaderLayers.pBase.pShaderInfo && pTexShaderReplacement->shaderLayers.layerList.empty () );
        CalcShaderForTexAndEntity ( pTexShaderReplacement->shaderLayers, pTexNameInfo, pClientEntity, iEntityType, false );
        pTexShaderReplacement->bSet = true;

        // texNoEntityShader need to be done also, so we can see what needs to be inherited from it
        {
            STexShaderReplacement& texNoEntityShader = pTexNameInfo->GetTexNoEntityShader ( iEntityType );
            UpdateTexShaderReplacementNoEntity ( pTexNameInfo, texNoEntityShader, iEntityType );

            // Handle base inheritance
            if ( pTexShaderReplacement->shaderLayers.pBase.pShaderInfo == NULL )
            {
                if ( texNoEntityShader.shaderLayers.pBase.bMixEntityAndNonEntity )
                    pTexShaderReplacement->shaderLayers.pBase = texNoEntityShader.shaderLayers.pBase;
            }

            // Handle layer inheritance
            for ( uint i = 0 ; i < texNoEntityShader.shaderLayers.layerList.size () ; i++ )
            {
                const SShaderInfoInstance& info = texNoEntityShader.shaderLayers.layerList[i];
                if ( info.bMixEntityAndNonEntity )
                    pTexShaderReplacement->shaderLayers.layerList.push_back ( info );
            }
        }

        FinalizeLayers ( pTexShaderReplacement->shaderLayers );
    }

    return pTexShaderReplacement;
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::UpdateTexShaderReplacementNoEntity
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::UpdateTexShaderReplacementNoEntity ( STexNameInfo* pTexNameInfo, STexShaderReplacement& texNoEntityShader, int iEntityType )
{
    if ( !texNoEntityShader.bSet )
    {
        // If not done yet, needs to be done
        dassert ( !texNoEntityShader.shaderLayers.pBase.pShaderInfo && texNoEntityShader.shaderLayers.layerList.empty () );
        CalcShaderForTexAndEntity ( texNoEntityShader.shaderLayers, pTexNameInfo, NULL, iEntityType, false );
        texNoEntityShader.bSet = true;

        FinalizeLayers ( texNoEntityShader.shaderLayers );
    }
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::GetShaderForTexAndEntity
//
//
//
//////////////////////////////////////////////////////////////////
SShaderInfoLayers* CMatchChannelManager::GetShaderForTexAndEntity ( STexInfo* pTexInfo, CClientEntityBase* pClientEntity, int iEntityType )
{
    if ( m_bChangesPending )
        FlushChanges ();

    STexNameInfo* pTexNameInfo = pTexInfo->pAssociatedTexNameInfo;

    // Ignore unknown client entities
    if ( pClientEntity )
        if ( !MapContains ( m_KnownClientEntities, pClientEntity ) )
            pClientEntity = NULL;

    if ( pClientEntity )
    {
        // Get entity info for this replace
        STexShaderReplacement* pTexShaderReplacement = MapFind ( pTexNameInfo->texEntityShaderMap, pClientEntity );

        if ( !pTexShaderReplacement || !pTexShaderReplacement->bSet )
        {
            pTexShaderReplacement = UpdateTexShaderReplacement ( pTexNameInfo, pClientEntity, iEntityType );
        }

    #ifdef SHADER_DEBUG_CHECKS
        if ( pTexNameInfo->iDebugCounter1++ > 400 )
        {
            // Check cached shader is correct
            pTexNameInfo->iDebugCounter1 = rand () % 100;
            SShaderInfoLayers shaderLayersCheck1;
            CalcShaderForTexAndEntity ( shaderLayersCheck1, pTexNameInfo, pClientEntity, iEntityType, true );

            // Handle base inheritance
            STexShaderReplacement& texNoEntityShader = pTexNameInfo->GetTexNoEntityShader ( iEntityType );
            if ( shaderLayersCheck1.pBase.pShaderInfo == NULL )
            {
                if ( texNoEntityShader.shaderLayers.pBase.bMixEntityAndNonEntity )
                    shaderLayersCheck1.pBase = texNoEntityShader.shaderLayers.pBase;
            }

            // Handle layer inheritance
            for ( uint i = 0 ; i < texNoEntityShader.shaderLayers.layerList.size () ; i++ )
            {
                const SShaderInfoInstance& info = texNoEntityShader.shaderLayers.layerList[i];
                if ( info.bMixEntityAndNonEntity )
                    shaderLayersCheck1.layerList.push_back ( info );
            }

            FinalizeLayers ( shaderLayersCheck1 );
            assert ( pTexShaderReplacement->shaderLayers == shaderLayersCheck1 );
        }
    #endif

        // Return layers for this entity
        return &pTexShaderReplacement->shaderLayers;
    }
    else
    {
        STexShaderReplacement& texNoEntityShader = pTexNameInfo->GetTexNoEntityShader ( iEntityType );

        if ( !texNoEntityShader.bSet )
        {
            UpdateTexShaderReplacementNoEntity ( pTexNameInfo, texNoEntityShader, iEntityType );
        }

    #ifdef SHADER_DEBUG_CHECKS
        if ( pTexNameInfo->iDebugCounter2++ > 400 )
        {
            // Check cached shader is correct
            pTexNameInfo->iDebugCounter2 = rand () % 100;
            SShaderInfoLayers shaderLayersCheck2;
            CalcShaderForTexAndEntity ( shaderLayersCheck2, pTexNameInfo, NULL, iEntityType, true );
            FinalizeLayers ( shaderLayersCheck2 );
            assert ( texNoEntityShader.shaderLayers == shaderLayersCheck2 );
        }
    #endif

        // Return layers for any entity
        return &texNoEntityShader.shaderLayers;
    }
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::CalcShaderForTexAndEntity
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::CalcShaderForTexAndEntity ( SShaderInfoLayers& outShaderLayers, STexNameInfo* pTexNameInfo, CClientEntityBase* pClientEntity, int iEntityType, bool bSilent )
{
    // Get match channels for this d3d data
    const std::set < CMatchChannel* >& resultChannelList = pTexNameInfo->matchChannelList;

    // In each channel, get the best shader that has the correct entity
    for ( std::set < CMatchChannel* >::const_iterator iter = resultChannelList.begin () ; iter != resultChannelList.end () ; ++iter )
    {
        CMatchChannel* pChannel = *iter;
        pChannel->GetBestShaderForEntity ( pClientEntity, iEntityType, outShaderLayers );
    }

#ifdef SHADER_DEBUG_OUTPUT
    if ( ( outShaderLayers.pBase.pShaderInfo || !outShaderLayers.layerList.empty () ) && !bSilent )
        OutputDebug ( SString ( "    CalcShaderForTexAndEntity - Tex:%s Entity:%s Base:%s NumLayers:%d", GetDebugTag ( pTexNameInfo ), GetDebugTag ( pClientEntity ), GetDebugTag ( outShaderLayers.pBase.pShaderInfo ), outShaderLayers.layerList.size () ) );
#endif
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::RemoveClientEntityRefs
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::RemoveClientEntityRefs ( CClientEntityBase* pClientEntity )
{
    assert ( pClientEntity );

    // Ignore unknown client entities
    if ( !MapContains ( m_KnownClientEntities, pClientEntity ) )
        return;
    MapRemove ( m_KnownClientEntities, pClientEntity );

    OutputDebug ( SString ( "RemoveClientEntityRefs - Entity:%s", GetDebugTag ( pClientEntity ) ) );

    std::set < CMatchChannel* > affectedChannels;
    for ( std::map < CShaderAndEntityPair, CMatchChannel* >::iterator iter = m_ChannelUsageMap.begin () ; iter != m_ChannelUsageMap.end () ; )
    {
        if ( pClientEntity == iter->first.pClientEntity )
        {
            CMatchChannel* pChannel = iter->second;
            pChannel->RemoveShaderAndEntity ( iter->first );
            m_ChannelUsageMap.erase ( iter++ );

            MapInsert ( affectedChannels, pChannel );
        }
        else
            ++iter;
    }

    // Flag affected textures to re-calc shader results
    for ( std::set < CMatchChannel* >::iterator iter = affectedChannels.begin () ; iter != affectedChannels.end () ; ++iter )
    {
        CMatchChannel* pChannel = *iter;
        for ( std::set < STexNameInfo* >::iterator iter = pChannel->m_MatchedTextureList.begin () ; iter != pChannel->m_MatchedTextureList.end () ; ++iter )
            (*iter)->ResetReplacementResults ();

        // Also delete channel if is not refed anymore
        if ( pChannel->GetShaderAndEntityCount () == 0 )
            DeleteChannel ( pChannel );
        // This could be optimized
    }

    // Need to remove client entity entries that were used even though they had no matches
    for ( CFastHashMap < SString, STexNameInfo* >::const_iterator iter = m_AllTextureList.begin () ; iter != m_AllTextureList.end () ; ++iter )
    {
        MapRemove ( iter->second->texEntityShaderMap, pClientEntity );
    }

#ifdef SHADER_DEBUG_CHECKS
    for ( std::set < CMatchChannel* >::const_iterator iter = m_CreatedChannelList.begin () ; iter != m_CreatedChannelList.end () ; ++iter )
    {
        CMatchChannel* pChannel = *iter;
        for ( std::set < CShaderAndEntityPair >::const_iterator iter = pChannel->m_ShaderAndEntityList.begin () ; iter != pChannel->m_ShaderAndEntityList.end () ; ++iter )
        {
            assert ( pClientEntity != iter->pClientEntity );
        }
    }
#endif
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::RemoveShaderRefs
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::RemoveShaderRefs ( CSHADERDUMMY* pShaderData )
{
    SShaderInfo* pShaderInfo = GetShaderInfo ( pShaderData, false, 0, false, TYPE_MASK_NONE, 0, false );
    if ( !pShaderInfo )
        return;

    OutputDebug ( SString ( "RemoveShaderRefs - Shader:%s", GetDebugTag ( pShaderInfo ) ) );

    std::set < CMatchChannel* > affectedChannels;
    for ( std::map < CShaderAndEntityPair, CMatchChannel* >::iterator iter = m_ChannelUsageMap.begin () ; iter != m_ChannelUsageMap.end () ; )
    {
        if ( pShaderInfo == iter->first.pShaderInfo )
        {
            CMatchChannel* pChannel = iter->second;
            pChannel->RemoveShaderAndEntity ( iter->first );
            m_ChannelUsageMap.erase ( iter++ );

            MapInsert ( affectedChannels, pChannel );
        }
        else
            ++iter;
    }

    // Flag affected textures to re-calc shader matches
    for ( std::set < CMatchChannel* >::iterator iter = affectedChannels.begin () ; iter != affectedChannels.end () ; ++iter )
    {
        CMatchChannel* pChannel = *iter;
        for ( std::set < STexNameInfo* >::iterator iter = pChannel->m_MatchedTextureList.begin () ; iter != pChannel->m_MatchedTextureList.end () ; ++iter )
            (*iter)->ResetReplacementResults ();

        // Also delete channel if is not refed anymore
        if ( pChannel->GetShaderAndEntityCount () == 0 )
            DeleteChannel ( pChannel );
        // This could be optimized
    }

#ifdef SHADER_DEBUG_CHECKS
    // Check has been remove from textures
    for ( CFastHashMap < SString, STexNameInfo* >::const_iterator iter = m_AllTextureList.begin () ; iter != m_AllTextureList.end () ; ++iter )
    {
        STexNameInfo* pTexNameInfo = iter->second;
        for ( std::map < CClientEntityBase*, STexShaderReplacement >::const_iterator iter = pTexNameInfo->texEntityShaderMap.begin () ; iter != pTexNameInfo->texEntityShaderMap.end () ; ++iter )
        {
            assert ( iter->second.shaderLayers.pBase.pShaderInfo != pShaderInfo );
            for ( uint i = 0 ; i < iter->second.shaderLayers.layerList.size () ; i++ )
                assert ( iter->second.shaderLayers.layerList[i].pShaderInfo != pShaderInfo );
        }
        
        for ( uint i = 0 ; i < NUMELMS( pTexNameInfo->texNoEntityShaders ) ; i++ )
        {
            const STexShaderReplacement& texNoEntityShader = pTexNameInfo->texNoEntityShaders[i];
            assert ( texNoEntityShader.shaderLayers.pBase.pShaderInfo != pShaderInfo );
            for ( uint i = 0 ; i < texNoEntityShader.shaderLayers.layerList.size () ; i++ )
                assert ( texNoEntityShader.shaderLayers.layerList[i].pShaderInfo != pShaderInfo );
        }
    }
#endif

#ifdef SHADER_DEBUG_CHECKS
    for ( std::set < CMatchChannel* >::const_iterator iter = m_CreatedChannelList.begin () ; iter != m_CreatedChannelList.end () ; ++iter )
    {
        CMatchChannel* pChannel = *iter;
        for ( std::set < CShaderAndEntityPair >::const_iterator iter = pChannel->m_ShaderAndEntityList.begin () ; iter != pChannel->m_ShaderAndEntityList.end () ; ++iter )
        {
            assert ( pShaderInfo != iter->pShaderInfo );
        }
    }
#endif

    DeleteShaderInfo ( pShaderInfo );
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::AddToOptimizeQueue
//
// Flag a channel for optimization
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::AddToOptimizeQueue ( CMatchChannel* pChannel )
{
    MapInsert ( m_OptimizeQueue, pChannel );
    m_bChangesPending = true;
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::AddToRematchQueue
//
// Flag a channel for rematching its textures
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::AddToRematchQueue ( CMatchChannel* pChannel )
{
    MapInsert ( m_RematchQueue, pChannel );
    m_bChangesPending = true;
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::FlushChanges
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::FlushChanges ( void )
{
    if ( !m_OptimizeQueue.empty () )
        ProcessOptimizeChannelsQueue ();

    if ( !m_RematchQueue.empty () )
    {
        ProcessRematchTexturesQueue ();
        RecalcEverything ();
    }

    dassert ( m_OptimizeQueue.empty () && m_RematchQueue.empty () );
    m_bChangesPending = false;
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::RecalcEverything
//
// Reset replacement info for all changed channels
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::RecalcEverything ( void )
{
    OutputDebug ( SString ( "RecalcEverything - %d channels", m_CreatedChannelList.size () ) );

    std::set < CMatchChannel* > cloneList = m_CreatedChannelList;
    for ( std::set < CMatchChannel* >::iterator iter = cloneList.begin () ; iter != cloneList.end () ; ++iter )
    {
        CMatchChannel* pChannel = *iter;
        if ( pChannel->GetMatchChain ().matchTypeList.empty () )
        {
            // Delete channel if no match text

            // Remove shader and entity refs
            std::set < CShaderAndEntityPair > cloneList2 = pChannel->m_ShaderAndEntityList;
            for ( std::set < CShaderAndEntityPair >::iterator iter = cloneList2.begin () ; iter != cloneList2.end () ; ++iter )
            {
                const CShaderAndEntityPair& key = *iter;
                RemoveUsage ( key, pChannel );
            }

            DeleteChannel ( pChannel );
        }
        else
        {

            if ( pChannel->m_bResetReplacements )
            {
                // Force textures to find rematches
                pChannel->m_bResetReplacements = false;
                for ( std::set < STexNameInfo* >::iterator iter = pChannel->m_MatchedTextureList.begin () ; iter != pChannel->m_MatchedTextureList.end () ; ++iter )
                    (*iter)->ResetReplacementResults ();
            }
        }
    }

    // Remove ClientEntitys with no matches
    std::set < CClientEntityBase* > removeList = m_KnownClientEntities;
    for ( std::map < CShaderAndEntityPair, CMatchChannel* >::iterator iter = m_ChannelUsageMap.begin () ; iter != m_ChannelUsageMap.end () ; ++iter )
    {
        if ( iter->first.pClientEntity )
            MapRemove ( removeList, iter->first.pClientEntity );
    }

    for ( std::set < CClientEntityBase* >::iterator iter = removeList.begin () ; iter != removeList.end () ; ++iter )
    {
        // This call could be optimized as the entity won't be present in some maps
        RemoveClientEntityRefs ( *iter );
    }
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::ProcessRematchTexturesQueue
//
// Recheck all textures against the required channels
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::ProcessRematchTexturesQueue ( void )
{
    OutputDebug ( SString ( "ProcessRematchTexturesQueue - %d items (%d textures)", m_RematchQueue.size (), m_AllTextureList.size () ) );

    std::set < CMatchChannel* > rematchQueue = m_RematchQueue;
    m_RematchQueue.clear ();

    // For each queued channel
    for ( std::set < CMatchChannel* >::iterator iter = rematchQueue.begin () ; iter != rematchQueue.end () ; ++iter )
    {
        CMatchChannel* pChannel = *iter;
        pChannel->m_bResetReplacements = true;

        OutputDebug ( SString ( "    [ProcessRematchTexturesQueue] - Channel:%s", GetDebugTag ( pChannel ) ) );

        // Remove existing matches
        std::set < STexNameInfo* > matchedTextureList = pChannel->m_MatchedTextureList;
        for ( std::set < STexNameInfo* >::iterator iter = matchedTextureList.begin () ; iter != matchedTextureList.end () ; ++iter )
        {
            STexNameInfo* pTexNameInfo = *iter;
            pChannel->RemoveTexture ( pTexNameInfo );
            MapRemove ( pTexNameInfo->matchChannelList, pChannel );
            pTexNameInfo->ResetReplacementResults ();    // Do this here as it won't get picked up in RecalcEverything now
        }

        // Rematch against texture list
        for ( CFastHashMap < SString, STexNameInfo* >::iterator iter = m_AllTextureList.begin () ; iter != m_AllTextureList.end () ; ++iter )
        {
            STexNameInfo* pTexNameInfo = iter->second;
            if ( pChannel->m_MatchChain.IsAdditiveMatch ( pTexNameInfo->strTextureName ) )
            {
                pChannel->AddTexture ( pTexNameInfo );
                MapInsert ( pTexNameInfo->matchChannelList, pChannel );
            }
        }
    }
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::ProcessOptimizeChannelsQueue
//
// Merge channels which have the same match chain
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::ProcessOptimizeChannelsQueue ( void )
{
    OutputDebug ( SString ( "ProcessOptimizeChannelsQueue - %d items", m_OptimizeQueue.size () ) );

    std::set < CMatchChannel* > updateQueue = m_OptimizeQueue;
    m_OptimizeQueue.clear ();
    for ( std::set < CMatchChannel* >::iterator iter = updateQueue.begin () ; iter != updateQueue.end () ; ++iter )
    {
        CMatchChannel* pChannel = *iter;

        OutputDebug ( SString ( "    [ProcessOptimizeChannelsQueue] - Channel:%s", GetDebugTag ( pChannel ) ) );

        // Find another channel with the same match chain
        CMatchChannel* pOther = FindChannelWithMatchChain ( pChannel->GetMatchChain (), pChannel );
        if ( pOther )
        {
            // Merge and delete original
            MergeChannelTo ( pChannel, pOther );
            DeleteChannel ( pChannel );

            AddToRematchQueue ( pOther );
        }
    }
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::MergeChannelTo
//
// Remove usage from pSource, add to pTarget
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::MergeChannelTo ( CMatchChannel* pSource, CMatchChannel* pTarget )
{
    OutputDebug ( SString ( "    MergeChannelTo - Source:%s  Target:%s", GetDebugTag ( pSource ), GetDebugTag ( pTarget ) ) );

    std::set < CShaderAndEntityPair > usedByMap = pSource->m_ShaderAndEntityList;
    for ( std::set < CShaderAndEntityPair >::iterator iter = usedByMap.begin () ; iter != usedByMap.end () ; ++iter )
    {
        const CShaderAndEntityPair& key = *iter;
        RemoveUsage ( key, pSource );
        AddUsage ( key, pTarget );
    }
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::FindChannelWithMatchChain
//
// Find another channel which has the same match chain
//
//////////////////////////////////////////////////////////////////
CMatchChannel* CMatchChannelManager::FindChannelWithMatchChain ( const SWildcardMatchChain& matchChain, CMatchChannel* pExcluding )
{
    for ( std::set < CMatchChannel* >::iterator iter = m_CreatedChannelList.begin () ; iter != m_CreatedChannelList.end () ; ++iter )
    {
        CMatchChannel* pTarget = *iter;
        if ( pTarget != pExcluding && matchChain == pTarget->GetMatchChain () )
        {
            if ( pTarget->GetShaderAndEntityCount () > 0 )
                return pTarget;
        }
    }
    return NULL;
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::GetChannelOnlyUsedBy
//
// Find/add channel for this pair, make sure it is not used by another pair
//
//////////////////////////////////////////////////////////////////
CMatchChannel* CMatchChannelManager::GetChannelOnlyUsedBy ( SShaderInfo* pShaderInfo, CClientEntityBase* pClientEntity, bool bAppendLayers )
{
    CShaderAndEntityPair key ( pShaderInfo, pClientEntity, bAppendLayers );
    CMatchChannel* pChannel = GetChannel ( key );

    if ( pChannel->GetShaderAndEntityCount () == 1 )
    {
        // Channel is only used by this key, so we can return that
        return pChannel;
    }
    else
    {
        // Channel is used by other keys, so remove this key from it
        RemoveUsage ( key, pChannel );

        // And clone a new channel
        CMatchChannel* pClone = NewChannel ();
        pClone->SetMatchChain ( pChannel->GetMatchChain () );
        AddUsage ( key, pClone );

        // Both channels will need rematching
        AddToRematchQueue ( pChannel );
        AddToRematchQueue ( pClone );

        return pClone;
    }
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::AddUsage
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::AddUsage ( const CShaderAndEntityPair& key, CMatchChannel* pChannel )
{
    OutputDebug ( SString ( "    AddUsage - Channel:%s  key:%s", GetDebugTag ( pChannel ), GetDebugTag ( key ) ) );
    dassert ( !MapContains ( m_ChannelUsageMap, key ) );
    pChannel->AddShaderAndEntity ( key );
    MapSet ( m_ChannelUsageMap, key, pChannel );
    pChannel->m_bResetReplacements = true;
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::RemoveUsage
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::RemoveUsage ( const CShaderAndEntityPair& key, CMatchChannel* pChannel )
{
    OutputDebug ( SString ( "    RemoveUsage - Channel:%s  key:%s", GetDebugTag ( pChannel ), GetDebugTag ( key ) ) );
    dassert ( MapContains ( m_ChannelUsageMap, key ) );
    pChannel->RemoveShaderAndEntity ( key );
    MapRemove ( m_ChannelUsageMap, key );
    pChannel->m_bResetReplacements = true;
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::GetChannel
//
// Find the channel which is being used by this pair
//
//////////////////////////////////////////////////////////////////
CMatchChannel* CMatchChannelManager::GetChannel ( const CShaderAndEntityPair& key )
{
    CMatchChannel* pChannel = MapFindRef ( m_ChannelUsageMap, key );
    if ( !pChannel )
    {
        pChannel = NewChannel ();
        AddUsage ( key, pChannel );
    }
    return pChannel;
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::NewChannel
//
// Create a channel
//
//////////////////////////////////////////////////////////////////
CMatchChannel* CMatchChannelManager::NewChannel ( void )
{
    CMatchChannel* pChannel = new CMatchChannel ();
    MapInsert ( m_CreatedChannelList, pChannel );
    OutputDebug ( SString ( "    NewChannel - Channel:%s", GetDebugTag ( pChannel ) ) );
    return pChannel;
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::DeleteChannel
//
// Destroy a channel
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::DeleteChannel ( CMatchChannel* pChannel )
{
    OutputDebug ( SString ( "    DeleteChannel - Channel:%s", GetDebugTag ( pChannel ) ) );

    std::set < STexNameInfo* > matchedTextureList = pChannel->m_MatchedTextureList;
    for ( std::set < STexNameInfo* >::iterator iter = matchedTextureList.begin () ; iter != matchedTextureList.end () ; ++iter )
    {
        STexNameInfo* pTexNameInfo = *iter;
        pChannel->RemoveTexture ( pTexNameInfo );
        MapRemove ( pTexNameInfo->matchChannelList, pChannel );

        // Reset shader matches now as this channel is going
        pTexNameInfo->ResetReplacementResults ();
    }

#ifdef SHADER_DEBUG_CHECKS
    for ( std::map < CShaderAndEntityPair, CMatchChannel* >::iterator iter = m_ChannelUsageMap.begin () ; iter != m_ChannelUsageMap.end () ; ++iter )
        assert ( pChannel != iter->second );
#endif
    MapRemove ( m_OptimizeQueue, pChannel );
    MapRemove ( m_CreatedChannelList, pChannel );
    MapRemove ( m_RematchQueue, pChannel );
    assert ( pChannel->m_MatchedTextureList.empty () );
    delete pChannel;
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::GetShaderInfo
//
//
//
//////////////////////////////////////////////////////////////////
SShaderInfo* CMatchChannelManager::GetShaderInfo ( CSHADERDUMMY* pShaderData, bool bAddIfRequired, float fPriority, bool bLayered, int iTypeMask, uint uiShaderCreateTime, bool bUsesVertexShader )
{
    // Find existing
    SShaderInfo* pShaderInfo = MapFindRef ( m_ShaderInfoMap, pShaderData );
    if ( !pShaderInfo && bAddIfRequired )
    {
        // Add new
        MapSet ( m_ShaderInfoMap, pShaderData, new SShaderInfo ( pShaderData, fPriority, bLayered, iTypeMask, uiShaderCreateTime, bUsesVertexShader ) );
        pShaderInfo = MapFindRef ( m_ShaderInfoMap, pShaderData );
    }

    return pShaderInfo;
}


//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::DeleteShaderInfo
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::DeleteShaderInfo ( SShaderInfo* pShaderInfo )
{
    MapRemove ( m_ShaderInfoMap, pShaderInfo->pShaderData );
    delete pShaderInfo;
}


////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::GetShaderReplacementStats
//
// Get stats
//
////////////////////////////////////////////////////////////////
void CMatchChannelManager::GetShaderReplacementStats ( SShaderReplacementStats& outStats )
{
    outStats.uiTotalTextures = m_AllTextureList.size ();
    outStats.uiTotalShaders = m_ShaderInfoMap.size ();
    outStats.uiTotalEntitesRefed = m_KnownClientEntities.size ();
    outStats.channelStatsList.clear ();

    for ( std::set < CMatchChannel* >::iterator iter = m_CreatedChannelList.begin () ; iter != m_CreatedChannelList.end () ; ++iter )
    {
        CMatchChannel* pChannel = *iter;
        SMatchChannelStats channelStats;

        std::vector < SMatchType >& matchTypeList = pChannel->m_MatchChain.matchTypeList;
        SString strResult;
        for ( uint i = 0 ; i < matchTypeList.size () ; i++ )
        {
            const SMatchType& matchType = matchTypeList[i];
            if ( i )
                strResult += " ";
            if ( matchTypeList[i].bAdditive )
                strResult += "+";
            else
                strResult += "-";
            strResult += matchTypeList[i].strMatch;

            if ( strResult.length () > 25 )
                break;
        }
        channelStats.strTag = strResult.Left ( 30 );
        channelStats.uiNumMatchedTextures = pChannel->m_MatchedTextureList.size ();
        channelStats.uiNumShaderAndEntities = pChannel->m_ShaderAndEntityList.size ();
        MapSet ( outStats.channelStatsList, pChannel->m_uiId, channelStats );
    }
}


//
//
//
// Debugging
//
//
//

#ifdef SHADER_DEBUG_OUTPUT

std::map < CClientEntityBase*, int > entityIdMap;
std::map < SShaderInfo*, int > shaderInfoIdMap;

int GetEntityId ( CClientEntityBase* pClientEntity )
{
    int* pId = MapFind ( entityIdMap, pClientEntity );
    if ( !pId )
    {
        MapSet ( entityIdMap, pClientEntity, entityIdMap.size () + 1 );
        pId = MapFind ( entityIdMap, pClientEntity );
    }
    return *pId;
}

int GetShaderInfoId ( SShaderInfo* pShaderInfo )
{
    int* pId = MapFind ( shaderInfoIdMap, pShaderInfo );
    if ( !pId )
    {
        MapSet ( shaderInfoIdMap, pShaderInfo, shaderInfoIdMap.size () + 1 );
        pId = MapFind ( shaderInfoIdMap, pShaderInfo );
    }
    return *pId;
}


SString GetDebugTagStr ( const CShaderAndEntityPair& key )
{
    return SString ( "[%s_%s]", GetDebugTag ( key.pShaderInfo ), GetDebugTag ( key.pClientEntity ) );
}

SString GetDebugTagStr ( const SWildcardMatchChain& matchChain )
{
    SString strResult = "{";
    for ( uint i = 0 ; i < matchChain.matchTypeList.size () ; i++ )
    {
        if ( i )
            strResult += " ";
        if ( matchChain.matchTypeList[i].bAdditive )
            strResult += "+";
        else
            strResult += "-";
        strResult += matchChain.matchTypeList[i].strMatch;
    }

    return strResult + "}";
}

SString GetDebugTagStr ( STexNameInfo* pTexNameInfo )
{
    if ( !pTexNameInfo )
        return "NULL";
    return pTexNameInfo->strTextureName;
}

SString GetDebugTagStr ( CClientEntityBase* pClientEntity )
{
    if ( !pClientEntity )
        return "E----";
    return SString ( "E%04d", GetEntityId ( pClientEntity ) );
}

SString GetDebugTagStr ( SShaderInfo* pShaderInfo )
{
    if ( !pShaderInfo )
        return "S--";
    return SString ( "S%02d", GetShaderInfoId ( pShaderInfo ) );
}

SString GetDebugTagStr ( CMatchChannel* pChannel )
{
    return SString ( "%08x", pChannel->m_uiId );
}

#endif
