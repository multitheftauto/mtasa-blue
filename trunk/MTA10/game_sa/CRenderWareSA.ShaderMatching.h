/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.ShaderMatching.h
*
*****************************************************************************/

#ifdef SHADER_DEBUG_OUTPUT
    class CShaderAndEntityPair;
    struct SWildcardMatchChain;
    class CMatchChannel;

    #define OutputDebug(x) OutputDebugLine(x)
    #define GetDebugTag(x) *GetDebugTagStr(x)
    SString GetDebugTagStr ( const CShaderAndEntityPair& key );
    SString GetDebugTagStr ( const SWildcardMatchChain& pMatchChain );
    SString GetDebugTagStr ( STexNameInfo* pTexNameInfo );
    SString GetDebugTagStr ( CClientEntityBase* pClientEntity );
    SString GetDebugTagStr ( SShaderInfo* pShaderInfo );
    SString GetDebugTagStr ( CMatchChannel* pChannel );
#else
    #define OutputDebug(x) {}
#endif


//
// A single additive or subtractive wildcard match string
//
struct SMatchType
{
    SMatchType ( const SString& strMatch, bool bAdditive )
        : strMatch ( strMatch )
        , bAdditive ( bAdditive )
    {
    }
    SString strMatch;
    bool    bAdditive;
};


//
// A chain of additive and subtractive wildcard matches
//
struct SWildcardMatchChain
{
    //
    // Return true if texture name results in an Add
    //
    bool IsAdditiveMatch ( const SString& strTextureName ) const
    {
        bool bIsMatch = false;
        for ( std::vector < SMatchType > ::const_iterator iter = matchTypeList.begin () ; iter != matchTypeList.end () ; ++iter )
            if ( WildcardMatch ( iter->strMatch, strTextureName ) )
                bIsMatch = iter->bAdditive;

        return bIsMatch;
    }

    //
    // Append a new match to the end of the match type list
    //
    void AppendMatchType ( const SString& strMatch, bool bAdditive )
    {
        // Delete any previous matches
        for ( std::vector < SMatchType > ::const_iterator iter = matchTypeList.begin () ; iter != matchTypeList.end () ; )
        {
            if ( strMatch == iter->strMatch )
                iter = matchTypeList.erase ( iter );
            else
                ++iter;
        }

        // Append to end
        matchTypeList.push_back ( SMatchType ( strMatch, bAdditive ) );

        // All subtractive matches at the start can be deleted
        while ( !matchTypeList.empty () && !matchTypeList[0].bAdditive )
            matchTypeList.erase ( matchTypeList.begin () );
    }

    bool operator== ( const SWildcardMatchChain& other ) const
    {
        // TODO: Check case
        uint uiSize = matchTypeList.size ();
        if ( uiSize != other.matchTypeList.size () )
            return false;

        for ( uint i = 0 ; i < uiSize ; i++ )
        {
            const SMatchType& A = matchTypeList[i];
            const SMatchType& B = other.matchTypeList[i];
            if ( A.bAdditive != B.bAdditive || A.strMatch != B.strMatch )
                return false;
        }
        return true;
    }

    std::vector < SMatchType > matchTypeList;          // List of additive and subtractive wildcard match strings
};


////////////////////////////////////////////////////////////////
//
// CShaderAndEntityPair
//
// Shader + Entity
//
////////////////////////////////////////////////////////////////
class CShaderAndEntityPair
{
public:
    CShaderAndEntityPair ( SShaderInfo* pShaderInfo, CClientEntityBase* pClientEntity, bool bAppendLayers )
        : pShaderInfo ( pShaderInfo )
        , pClientEntity ( pClientEntity )
        , bAppendLayers ( bAppendLayers )
    {}

    bool operator < ( const CShaderAndEntityPair& other ) const
    {
        return pShaderInfo < other.pShaderInfo ||
                ( pShaderInfo == other.pShaderInfo && pClientEntity < other.pClientEntity );
    }

    bool operator == ( const CShaderAndEntityPair& other ) const
    {
        return pShaderInfo == other.pShaderInfo && pClientEntity == other.pClientEntity;
    }

    SShaderInfo*        pShaderInfo;
    CClientEntityBase*  pClientEntity;
    bool                bAppendLayers;
};


////////////////////////////////////////////////////////////////
//
// CMatchChannel
//
// All shader and entity pairs using a particular match chain
//
////////////////////////////////////////////////////////////////
class CMatchChannel
{
public:
    CMatchChannel ( void )
    {
        m_bResetReplacements = false;
        m_uiId = ms_uiIdCounter++;
    }

    //////////////////////////////////////////////
    // Matching string
    //////////////////////////////////////////////
    void AppendAdditiveMatch ( const SString& strTextureNameMatch )
    {
        m_MatchChain.AppendMatchType ( strTextureNameMatch, true );
        OutputDebug ( SString ( "     Channel:%s appends additive %s (Match chain now:%s)", GetDebugTag ( this ), *strTextureNameMatch, GetDebugTag ( m_MatchChain ) ) );
    }

    void AppendSubtractiveMatch ( const SString& strTextureNameMatch )
    {
        m_MatchChain.AppendMatchType ( strTextureNameMatch, false );
        OutputDebug ( SString ( "     Channel:%s appends subtractive %s (Match chain now:%s)", GetDebugTag ( this ), *strTextureNameMatch, GetDebugTag ( m_MatchChain ) ) );
    }

    //////////////////////////////////////////////
    // Shader and entity
    //////////////////////////////////////////////
    void AddShaderAndEntity ( const CShaderAndEntityPair& key )
    {
    #ifdef SHADER_DEBUG_OUTPUT
        if ( !MapContains ( m_ShaderAndEntityList, key ) )
            OutputDebug ( SString ( "     +Channel:%s adds (ShaderEntity:%s)", GetDebugTag ( this ), GetDebugTag ( key ) ) );
    #endif
        MapInsert ( m_ShaderAndEntityList, key );
    }

    void RemoveShaderAndEntity ( const CShaderAndEntityPair& key )
    {
    #ifdef SHADER_DEBUG_OUTPUT
        if ( MapContains ( m_ShaderAndEntityList, key ) )
            OutputDebug ( SString ( "     -Channel:%s removes (ShaderEntity:%s)", GetDebugTag ( this ), GetDebugTag ( key ) ) );
    #endif
        MapRemove ( m_ShaderAndEntityList, key );
    }

    uint GetShaderAndEntityCount ( void ) const
    {
        return m_ShaderAndEntityList.size ();
    }

    void GetBestShaderForEntity ( CClientEntityBase* pClientEntity, int iEntityType, SShaderInfoLayers& outShaderLayers ) const
    {
        for ( std::set < CShaderAndEntityPair >::const_iterator iter = m_ShaderAndEntityList.begin () ; iter != m_ShaderAndEntityList.end () ; ++iter )
        {
            const CShaderAndEntityPair& key = *iter;
            if ( key.pClientEntity == pClientEntity && ( key.pShaderInfo->iTypeMask & iEntityType ) )
            {
                if ( !key.pShaderInfo->bUsesVertexShader || iEntityType != TYPE_MASK_PED )
                {
                    if ( !key.pShaderInfo->bLayered )
                    {
                        // Base shader
                        if ( !outShaderLayers.pBase.pShaderInfo )
                            outShaderLayers.pBase = SShaderInfoInstance ( key.pShaderInfo, key.bAppendLayers );
                        else
                        {
                            // Check priority, then bias later additions
                            if ( outShaderLayers.pBase.pShaderInfo->orderValue < key.pShaderInfo->orderValue )
                                outShaderLayers.pBase = SShaderInfoInstance ( key.pShaderInfo, key.bAppendLayers );
                        }
                    }
                    else
                    {
                        // Layers
                        outShaderLayers.layerList.push_back ( SShaderInfoInstance ( key.pShaderInfo, key.bAppendLayers ) );
                    }
                }
            }
        }
    }

    //////////////////////////////////////////////
    // Texture
    //////////////////////////////////////////////
    void AddTexture ( STexNameInfo* pTexNameInfo )
    {
    #ifdef SHADER_DEBUG_OUTPUT
        if ( !MapContains ( m_MatchedTextureList, pTexNameInfo ) )
            OutputDebug ( SString ( "     +Channel:%s adds Tex:%s", GetDebugTag ( this ), GetDebugTag ( pTexNameInfo ) ) );
    #endif
        MapInsert ( m_MatchedTextureList, pTexNameInfo );
    }

    void RemoveTexture ( STexNameInfo* pTexNameInfo )
    {
    #ifdef SHADER_DEBUG_OUTPUT
        if ( MapContains ( m_MatchedTextureList, pTexNameInfo ) )
            OutputDebug ( SString ( "     -Channel:%s removes Tex:%s", GetDebugTag ( this ), GetDebugTag ( pTexNameInfo ) ) );
    #endif
        MapRemove ( m_MatchedTextureList, pTexNameInfo );
    }


    //////////////////////////////////////////////
    // Misc
    //////////////////////////////////////////////
    void SetMatchChain ( const SWildcardMatchChain& matchChain )
    {
    #ifdef SHADER_DEBUG_OUTPUT
        if ( !(m_MatchChain == matchChain) )
            OutputDebug ( SString ( "     Channel:%s SetMatchChain:%s", GetDebugTag ( this ), GetDebugTag ( matchChain ) ) );
    #endif
        m_MatchChain = matchChain;
    }

    const SWildcardMatchChain& GetMatchChain ( void ) const
    {
        return m_MatchChain;
    }

    SWildcardMatchChain                     m_MatchChain;               // String matches this channel represents
    std::set < STexNameInfo* >              m_MatchedTextureList;       // All textures whose name matches the match chain
    std::set < CShaderAndEntityPair >       m_ShaderAndEntityList;
    bool                                    m_bResetReplacements;       // Flag to indicate texture replacements should be reset
    uint                                    m_uiId;
    static uint                             ms_uiIdCounter;
};


////////////////////////////////////////////////////////////////
//
// CMatchChannelManager
//
////////////////////////////////////////////////////////////////
class CMatchChannelManager
{
public:
    void                AppendAdditiveMatch             ( CSHADERDUMMY* pShaderData, CClientEntityBase* pClientEntity, const SString& strTextureNameMatch, float fShaderPriority, bool bShaderLayered, int iTypeMask, uint uiShaderCreateTime, bool bShaderUsesVertexShader, bool bAppendLayers );
    void                AppendSubtractiveMatch          ( CSHADERDUMMY* pShaderData, CClientEntityBase* pClientEntity, const SString& strTextureNameMatch );
    void                InsertTexture                   ( STexInfo* pTexInfo );
    void                RemoveTexture                   ( STexInfo* pTexInfo );
    SShaderInfoLayers*  GetShaderForTexAndEntity        ( STexInfo* pTexInfo, CClientEntityBase* pClientEntity, int iEntityType );
    void                RemoveClientEntityRefs          ( CClientEntityBase* pClientEntity );
    void                RemoveShaderRefs                ( CSHADERDUMMY* pShaderData );
    void                GetShaderReplacementStats       ( SShaderReplacementStats& outStats );

protected:
    void                CalcShaderForTexAndEntity       ( SShaderInfoLayers& outShaderLayers, STexNameInfo* pTexNameInfo, CClientEntityBase* pClientEntity, int iEntityType, bool bSilent );
    void                AddToOptimizeQueue              ( CMatchChannel* pChannel );
    void                AddToRematchQueue               ( CMatchChannel* pChannel );
    void                FlushChanges                    ( void );
    void                RecalcEverything                ( void );
    void                ProcessRematchTexturesQueue     ( void );
    void                ProcessOptimizeChannelsQueue    ( void );
    void                MergeChannelTo                  ( CMatchChannel* pSource, CMatchChannel* pTarget );
    CMatchChannel*      FindChannelWithMatchChain       ( const SWildcardMatchChain& matchChain, CMatchChannel* pExcluding );
    CMatchChannel*      GetChannelOnlyUsedBy            ( SShaderInfo* pShaderInfo, CClientEntityBase* pClientEntity, bool bAppendLayers );
    void                AddUsage                        ( const CShaderAndEntityPair& key, CMatchChannel* pChannel );
    void                RemoveUsage                     ( const CShaderAndEntityPair& key, CMatchChannel* pChannel );
    CMatchChannel*      GetChannel                      ( const CShaderAndEntityPair& key );
    CMatchChannel*      NewChannel                      ( void );
    void                DeleteChannel                   ( CMatchChannel* pChannel );
    SShaderInfo*        GetShaderInfo                   ( CSHADERDUMMY* pShaderData, bool bAddIfRequired, float fPriority, bool bLayered, int iTypeMask, uint uiShaderCreateTime, bool bUsesVertexShader );
    void                DeleteShaderInfo                ( SShaderInfo* pShaderInfo );

    STexShaderReplacement* UpdateTexShaderReplacement   ( STexNameInfo* pTexNameInfo, CClientEntityBase* pClientEntity, int iEntityType );
    void                UpdateTexShaderReplacementNoEntity  ( STexNameInfo* pTexNameInfo, STexShaderReplacement& texNoEntityShader, int iEntityType );
    void                FinalizeLayers                  ( SShaderInfoLayers& shaderLayers );

    bool                                                m_bChangesPending;
    std::map < CShaderAndEntityPair, CMatchChannel* >   m_ChannelUsageMap;
    std::set < CMatchChannel* >                         m_CreatedChannelList;
    std::set < CMatchChannel* >                         m_OptimizeQueue;
    std::set < CMatchChannel* >                         m_RematchQueue;
    CFastHashMap < SString, STexNameInfo* >             m_AllTextureList;
    std::map < CSHADERDUMMY*, SShaderInfo* >            m_ShaderInfoMap;
    std::set < CClientEntityBase* >                     m_KnownClientEntities;
};
