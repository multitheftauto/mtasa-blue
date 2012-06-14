/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CRenderItemManager.cpp
*  PURPOSE:
*  DEVELOPERS:  idiot
*
*****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>


////////////////////////////////////////////////////////////////
//
// class SNameMatchInfo
//
// Used to hold replacement requirements for a texture name match string
//
////////////////////////////////////////////////////////////////
struct SNameMatchInfo
{
    typedef std::multimap < float, CShaderItem* > SAppliedShaderOrderMap;

    // Map containing priority ordered shader replacements for different entities. NULL entity means global replace.
    std::map < CClientEntityBase*, SAppliedShaderOrderMap >  m_ClientEntityShaderMap;

    void AddShaderAndClientEntity ( CShaderItem* pShaderItem, CClientEntityBase* pClientEntity )
    {
        SAppliedShaderOrderMap& orderMap = MapGet ( m_ClientEntityShaderMap, pClientEntity );
        MapInsert ( orderMap, pShaderItem->m_fPriority, pShaderItem );
    }

    CShaderItem* FindShaderForClientEntity( CClientEntityBase* pClientEntity )
    {
        SAppliedShaderOrderMap* pOrderMap = MapFind ( m_ClientEntityShaderMap, pClientEntity );
        if ( pOrderMap )
        {
            if ( !pOrderMap->empty () )
            {
                CShaderItem* pShaderItem = pOrderMap->rbegin ()->second;
                return pShaderItem;
            }
        }
        return NULL;
    }

    void RemoveShaderAndClientEntity ( CShaderItem* pShaderItem, CClientEntityBase* pClientEntity )
    {
        // Find entity entry
        SAppliedShaderOrderMap* pOrderMap = MapFind ( m_ClientEntityShaderMap, pClientEntity );
        if ( pOrderMap )
        {
            // Find and remove matching shader
            for ( std::multimap < float, CShaderItem* >::iterator iterOrder = pOrderMap->begin () ; iterOrder != pOrderMap->end () ; )
            {
                if ( pShaderItem == iterOrder->second )
                    pOrderMap->erase ( iterOrder++ );
                else
                    ++iterOrder;
            }      

            // If no shaders left in order map, remove client entry
            if ( pOrderMap->empty () )
                MapRemove ( m_ClientEntityShaderMap, pClientEntity );
        }
    }

    void RemoveShader ( CShaderItem* pShaderItem )
    {
        // For each entity entry
        for ( std::map < CClientEntityBase*, SAppliedShaderOrderMap >::iterator iterEnt = m_ClientEntityShaderMap.begin () ; iterEnt != m_ClientEntityShaderMap.end () ; )
        {
            SAppliedShaderOrderMap* pOrderMap = &iterEnt->second;
            // Find and remove matching shader
            for ( std::multimap < float, CShaderItem* >::iterator iterOrder = pOrderMap->begin () ; iterOrder != pOrderMap->end () ; )
            {
                if ( pShaderItem == iterOrder->second )
                    pOrderMap->erase ( iterOrder++ );
                else
                    ++iterOrder;
            }

            // If no shaders left in order map, remove client entry
            if ( pOrderMap->empty () )
                m_ClientEntityShaderMap.erase ( iterEnt++ );
            else
                ++iterEnt;
        }      
    }

    bool IsEmpty ( void )
    {
        return m_ClientEntityShaderMap.empty ();
    }

    bool DoesHaveEntityDefs ( void )
    {
        if ( m_ClientEntityShaderMap.size () > 1 )              // If more than one item, must have entity defs
            return true;
        if ( m_ClientEntityShaderMap.empty () )                 // If less than one item, can't have entity defs
            return false;
        if ( m_ClientEntityShaderMap.begin()->first != NULL )   // If only item is not NULL, then it is an entity def
            return true;
        return false;                                           // If only item is NULL, then it is a global replace (no entity defs)
    }
};


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
// CRenderItemManager::GetNameMatchInfo
//
// Find or create name match info for this name match string
//
////////////////////////////////////////////////////////////////
SNameMatchInfo* CRenderItemManager::GetNameMatchInfo ( const SString& strTextureNameMatch )
{
    SNameMatchInfo* pNameMatchInfo = MapFindRef ( m_NameMatchInfoMap, strTextureNameMatch );
    if ( !pNameMatchInfo )
    {
        MapSet ( m_NameMatchInfoMap, strTextureNameMatch, new SNameMatchInfo () );
        pNameMatchInfo = MapFindRef ( m_NameMatchInfoMap, strTextureNameMatch );
    }
    return pNameMatchInfo;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::RemoveNameMatchInfo
//
// Remove and delete the supplied name match info
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::RemoveNameMatchInfo ( SNameMatchInfo* pNameMatchInfo )
{
    //
    // Remove match info item from all world textures
    //
    m_pRenderWare->RemoveWorldTextureWatchByContext ( (CSHADERDUMMY*)pNameMatchInfo );

    //
    // Remove match info item from D3DData map
    //
    for ( std::map < CD3DDUMMY*, SNameMatchInfo* >::iterator iterData = m_D3DDataShaderMap.begin () ; iterData != m_D3DDataShaderMap.end () ; )
    {
        if ( pNameMatchInfo == iterData->second )
            m_D3DDataShaderMap.erase ( iterData++ );
        else
            ++iterData;
    }

    //
    // Remove match info from the NameMatchInfoMap
    //
    uint uiSizeBefore = m_NameMatchInfoMap.size ();
    for ( std::map < SString, SNameMatchInfo* >::iterator iterMatch = m_NameMatchInfoMap.begin () ; iterMatch != m_NameMatchInfoMap.end () ; )
    {
        if ( iterMatch->second == pNameMatchInfo )
            m_NameMatchInfoMap.erase ( iterMatch++ );
        else
            ++iterMatch;
    }
    assert ( uiSizeBefore == m_NameMatchInfoMap.size () + 1 );

    //
    // Finally delete match info
    //
    delete pNameMatchInfo;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::GetRenderingClientEntity
//
// Get pointer to client entity being rendered
//
////////////////////////////////////////////////////////////////
CClientEntityBase* CRenderItemManager::GetRenderingClientEntity( void )
{
    CEntitySAInterface* pEntitySAInterface = CCore::GetSingleton().GetMultiplayer()->GetRenderingGameEntity ();
    CClientBase* pClient = CModManager::GetSingleton ().GetCurrentMod ();
    CClientEntityBase* pClientEntity = pClient->FindClientEntity ( pEntitySAInterface );
    return pClientEntity;
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
    for ( std::map < SString, SNameMatchInfo* >::iterator iterMatch = m_NameMatchInfoMap.begin () ; iterMatch != m_NameMatchInfoMap.end () ; )
    {
        const SString& strTextureNameMatch = iterMatch->first;
        SNameMatchInfo* pNameMatchInfo = iterMatch->second;
        CShaderItem* pShaderItem = pNameMatchInfo->FindShaderForClientEntity( pClientEntity );
        if ( pShaderItem )
        {
            RemoveShaderItemFromWorldTexture ( pShaderItem, strTextureNameMatch, pClientEntity );
            iterMatch = m_NameMatchInfoMap.begin ();
        }
        else
            ++iterMatch;
    }
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::GetAppliedShaderForD3DData
//
// Find which shader item is being used to render this D3DData
//
////////////////////////////////////////////////////////////////
CShaderItem* CRenderItemManager::GetAppliedShaderForD3DData ( CD3DDUMMY* pD3DData )
{
    // Save texture usage for later
    MapInsert ( m_FrameTextureUsage, pD3DData );

    // Is this D3DData being replaced at all?
    SNameMatchInfo* pNameMatchInfo = (SNameMatchInfo*)MapFindRef ( m_D3DDataShaderMap, pD3DData );
    if ( pNameMatchInfo )
    {
        // Does replace info use entities?
        if ( pNameMatchInfo->DoesHaveEntityDefs () )
        {
            // See if entity being rendered has a replace
            CClientEntityBase* pClientEntity = GetRenderingClientEntity ();
            CShaderItem* pShaderItem = pNameMatchInfo->FindShaderForClientEntity( pClientEntity );
            if ( pShaderItem )
                return pShaderItem;
        }

        // See if there is a global replace for this D3DData
        CShaderItem* pShaderItem = pNameMatchInfo->FindShaderForClientEntity( NULL );
        if ( pShaderItem )
            return pShaderItem;
    }

    return NULL;
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::ApplyShaderItemToWorldTexture
//
// Add an association between the shader item and a world texture match
//
////////////////////////////////////////////////////////////////
bool CRenderItemManager::ApplyShaderItemToWorldTexture ( CShaderItem* pShaderItem, const SString& strTextureNameMatch, CClientEntityBase* pClientEntity )
{
    assert ( pShaderItem );

    SNameMatchInfo* pNameMatchInfo = GetNameMatchInfo ( strTextureNameMatch );

    pNameMatchInfo->AddShaderAndClientEntity ( pShaderItem, pClientEntity );

    // Add new match at the end
    m_pRenderWare->AddWorldTextureWatch ( (CSHADERDUMMY*)pNameMatchInfo, strTextureNameMatch, pShaderItem->m_fPriority );

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

    SNameMatchInfo* pNameMatchInfo = GetNameMatchInfo ( strTextureNameMatch );

    pNameMatchInfo->RemoveShaderAndClientEntity ( pShaderItem, pClientEntity );

    if ( pNameMatchInfo->IsEmpty () )
    {
        // Remove match info if not longer used
        m_pRenderWare->RemoveWorldTextureWatch ( (CSHADERDUMMY*)pNameMatchInfo, strTextureNameMatch );
        RemoveNameMatchInfo ( pNameMatchInfo );
    }

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
    for ( std::map < SString, SNameMatchInfo* >::iterator iterMatch = m_NameMatchInfoMap.begin () ; iterMatch != m_NameMatchInfoMap.end () ; )
    {
        const SString& strTextureNameMatch = iterMatch->first;
        SNameMatchInfo* pNameMatchInfo = iterMatch->second;
        pNameMatchInfo->RemoveShader ( pShaderItem );
        if ( pNameMatchInfo->IsEmpty () )
        {
            // Remove match info if not longer used
            m_pRenderWare->RemoveWorldTextureWatch ( (CSHADERDUMMY*)pNameMatchInfo, strTextureNameMatch );
            RemoveNameMatchInfo ( pNameMatchInfo );
            iterMatch = m_NameMatchInfoMap.begin ();
        }
        else
            ++iterMatch;
    }
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::WatchCallback
//
// Notification that the pD3DData is being changed for this context
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::WatchCallback ( CSHADERDUMMY* pShaderDummy, CD3DDUMMY* pD3DDataNew, CD3DDUMMY* pD3DDataOld )
{
    // Remove old pointer
    if ( pD3DDataOld )
        MapRemove ( m_D3DDataShaderMap, pD3DDataOld );

    // Add new pointer
    if ( pD3DDataNew )
        MapSet ( m_D3DDataShaderMap, pD3DDataNew, (SNameMatchInfo*)pShaderDummy );
}


////////////////////////////////////////////////////////////////
//
// CRenderItemManager::StaticWatchCallback
//
//
//
////////////////////////////////////////////////////////////////
void CRenderItemManager::StaticWatchCallback ( CSHADERDUMMY* pShaderItem, CD3DDUMMY* pD3DDataNew, CD3DDUMMY* pD3DDataOld )
{
    ( ( CRenderItemManager* ) CGraphics::GetSingleton ().GetRenderItemManager () )->WatchCallback ( pShaderItem, pD3DDataNew, pD3DDataOld );
}
