/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CRenderItemManager.TextureReplace.cpp
*
*****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>

////////////////////////////////////////////////////////////////
//
// CRenderItemManager::GetVisibleTextureNames
//
// Get the names of all streamed in world textures, filtered by name and/or model
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::GetVisibleTextureNames ( std::vector < SString >& outNameList, const SString& strTextureNameMatch, ushort usModelID )
{
    // If modelid supplied, get the model texture names into a map
    std::set < SString > modelTextureNameMap;
    if ( usModelID )
    {
        std::vector < SString > modelTextureNameList;
        m_pRenderWare->GetModelTextureNames ( modelTextureNameList, usModelID );
        for ( std::vector < SString >::const_iterator iter = modelTextureNameList.begin () ; iter != modelTextureNameList.end () ; ++iter )
            modelTextureNameMap.insert ( (*iter).ToLower () );
    }

    SString strTextureNameMatchLower = strTextureNameMatch.ToLower ();

    std::set < SString > resultMap;

    // For each texture that was used in the previous frame
    for ( std::set < CD3DDUMMY* >::const_iterator iter = m_PrevFrameTextureUsage.begin () ; iter != m_PrevFrameTextureUsage.end () ; ++iter )
    {
        // Get the texture name
        SString strTextureName = m_pRenderWare->GetTextureName ( *iter );
        SString strTextureNameLower = strTextureName.ToLower ();

        if ( strTextureName.empty () )
            continue;

        // Filter by wildcard match
        if ( !WildcardMatch ( strTextureNameMatchLower, strTextureNameLower ) )
            continue;

        // Filter by model
        if ( usModelID )
            if ( !MapContains ( modelTextureNameMap, strTextureNameLower ) )
                continue;

        resultMap.insert ( strTextureName );
    }

    for ( std::set < SString >::const_iterator iter = resultMap.begin () ; iter != resultMap.end () ; ++iter )
    {
        outNameList.push_back ( *iter );
    }
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::RemoveClientEntityRefs
//
// Make sure all replacements are cleared for this entity
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::RemoveClientEntityRefs ( CClientEntityBase* pClientEntity )
{
    m_pRenderWare->RemoveClientEntityRefs ( pClientEntity );
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::GetAppliedShaderForD3DData
//
// Find which shader item is being used to render this D3DData
//
////////////////////////////////////////////////////////////////
SShaderItemLayers* CRenderItemManager::GetAppliedShaderForD3DData ( CD3DDUMMY* pD3DData )
{
    // Save texture usage for later
    MapInsert ( m_FrameTextureUsage, pD3DData );

    return m_pRenderWare->GetAppliedShaderForD3DData ( pD3DData );
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::ApplyShaderItemToWorldTexture
//
// Add an association between the shader item and a world texture match
//
////////////////////////////////////////////////////////////////
bool CRenderItemManager::ApplyShaderItemToWorldTexture ( CShaderItem* pShaderItem, const SString& strTextureNameMatch, CClientEntityBase* pClientEntity, bool bAppendLayers )
{
    assert ( pShaderItem );
    m_pRenderWare->AppendAdditiveMatch ( (CSHADERDUMMY*)pShaderItem, pClientEntity, strTextureNameMatch, pShaderItem->m_fPriority, pShaderItem->m_bLayered, pShaderItem->m_iTypeMask, pShaderItem->m_uiCreateTime, pShaderItem->GetUsesVertexShader (), bAppendLayers );
    return true;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::RemoveShaderItemFromWorldTexture
//
// Remove an association between the shader item and the world texture
//
////////////////////////////////////////////////////////////////
bool CRenderItemManager::RemoveShaderItemFromWorldTexture ( CShaderItem* pShaderItem, const SString& strTextureNameMatch, CClientEntityBase* pClientEntity )
{
    assert ( pShaderItem );
    m_pRenderWare->AppendSubtractiveMatch ( (CSHADERDUMMY*)pShaderItem, pClientEntity, strTextureNameMatch );
    return true;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::RemoveShaderItemFromWatchLists
//
// Ensure the shader item is not being referred to by the texture replacement system
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::RemoveShaderItemFromWatchLists ( CShaderItem* pShaderItem )
{
    m_pRenderWare->RemoveShaderRefs ( (CSHADERDUMMY*)pShaderItem );
}
