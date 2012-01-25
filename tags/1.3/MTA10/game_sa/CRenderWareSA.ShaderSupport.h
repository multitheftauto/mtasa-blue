/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*  PURPOSE:
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


//
// Info on how a texture is identified.
//
struct STexTag
{
    STexTag ( ushort usTxdId )
        : m_bUsingTxdId ( true )
        , m_usTxdId ( usTxdId )
        , m_pTex ( NULL )
    {
    }

    STexTag ( RwTexture* pTex )
        : m_bUsingTxdId ( false )
        , m_usTxdId ( 0 )
        , m_pTex ( pTex )
    {
    }

    bool Matches ( ushort usTxdId ) const
    {
        return m_bUsingTxdId && usTxdId == m_usTxdId;
    }

    bool Matches ( RwTexture* pTex ) const
    {
        return !m_bUsingTxdId && pTex == m_pTex;
    }

    const bool          m_bUsingTxdId;
    const ushort        m_usTxdId;      // Streamed textures are identified using the TXD id
    const RwTexture*    m_pTex;         // Custom textures are identified using the RwTexture pointer
};


//
// Info about a streamed in texture
//
struct STexInfo
{
    STexInfo ( const STexTag& texTag, const SString& strTextureName, CD3DDUMMY* pD3DData )
        : texTag ( texTag )
        , strTextureName ( strTextureName.ToLower () )
        , pD3DData ( pD3DData )
        , pAssociatedShadInfo ( NULL )
    {
    }
    STexTag                 texTag;
    const SString           strTextureName;         // Always lower case
    CD3DDUMMY* const        pD3DData;
    struct SShadInfo*       pAssociatedShadInfo;    // The shader which is currently replacing this texture
};



//
// Either an additive or subtractive wildcard match string
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
// Info about an active shader which can replace textures
//
struct SShadInfo
{
    SShadInfo ( CSHADERDUMMY* pShaderData, float fOrderPriority )
        : pShaderData ( pShaderData )
        , fOrderPriority ( fOrderPriority )
    {
    }

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

    std::vector < SMatchType >  matchTypeList;          // List of additive and subtractive wildcard match strings
    CSHADERDUMMY* const         pShaderData;
    const float                 fOrderPriority;
    std::set < STexInfo* >      associatedTexInfoMap;   // The list of textures this shader is currently replacing
};
