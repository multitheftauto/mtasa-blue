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

#ifdef MTA_DEBUG
    // Comment out the lines below for your desired debug options
    //#define SHADER_DEBUG_CHECKS         // Do lots of validation which can slow a debug build
    //#define SHADER_DEBUG_OUTPUT         // Output lots of debug strings
#endif

class CMatchChannel;
class CMatchChannelManager;
struct STexNameInfo;

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

    bool operator== ( ushort usTxdId ) const
    {
        return m_bUsingTxdId && usTxdId == m_usTxdId;
    }

    bool operator== ( RwTexture* pTex ) const
    {
        return !m_bUsingTxdId && pTex == m_pTex;
    }

    const bool          m_bUsingTxdId;
    const ushort        m_usTxdId;      // Streamed textures are identified using the TXD id
    const RwTexture*    m_pTex;         // Custom textures are identified using the RwTexture pointer
};


//
// Info about an active shader which can replace textures
//
struct SShaderInfo
{
    SShaderInfo ( CSHADERDUMMY* pShaderData, float fOrderPriority, uint uiShaderCreateTime )
        : pShaderData ( pShaderData )
        , fOrderPriority ( fOrderPriority )
        , uiShaderCreateTime ( uiShaderCreateTime )
    {
    }

    CSHADERDUMMY* const         pShaderData;
    const float                 fOrderPriority;
    const uint                  uiShaderCreateTime;
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
        , pAssociatedTexNameInfo ( NULL )
    {
    }
    STexTag                 texTag;
    const SString           strTextureName;         // Always lower case
    CD3DDUMMY* const        pD3DData;
    STexNameInfo*           pAssociatedTexNameInfo;
};


//
// Shader replacement, sort of cached
//
struct STexShaderReplacement
{
    STexShaderReplacement ( void ) : bSet ( false ), pShaderInfo ( NULL ) {}
    bool bSet;
    SShaderInfo* pShaderInfo;
};


//
// Info about a persistent texture
//
struct STexNameInfo
{
    STexNameInfo ( const SString& strTextureName )
        : strTextureName ( strTextureName.ToLower () )
    {
#ifdef SHADER_DEBUG_CHECKS
        iDebugCounter1 = 0;
        iDebugCounter2 = 0;
#endif
    }

    void ResetReplacementResults ( void )
    {
        texNoEntityShader = STexShaderReplacement ();
        texEntityShaderMap.clear ();
    }

    const SString                                       strTextureName;         // Always lower case
    std::set < STexInfo* >                              usedByTexInfoList;

    std::set < CMatchChannel* >                             matchChannelList;
    STexShaderReplacement                                   texNoEntityShader;
    std::map < CClientEntityBase*, STexShaderReplacement >  texEntityShaderMap;

#ifdef SHADER_DEBUG_CHECKS
    int iDebugCounter1;
    int iDebugCounter2;
#endif
};
