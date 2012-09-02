/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CRenderItem.EffectCloner.h
*  PURPOSE:
*
*****************************************************************************/


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CEffectCloner
//
// Maintain a list of active CEffectTemplate objects and clones d3d effects from them when needed
//
class CEffectCloner
{
public:
    virtual                 ~CEffectCloner      ( void ) {}
    virtual void            DoPulse             ( void ) = 0;
    virtual ID3DXEffect*    CreateD3DEffect     ( const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool& bOutUsesVertexShader, bool bDebug ) = 0;
    virtual void            ReleaseD3DEffect    ( ID3DXEffect* pD3DEffect ) = 0;
};

CEffectCloner* NewEffectCloner ( CRenderItemManager* pManager );



////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CEffectTemplate
//
// A compiled effect with which to clone d3d effects from
//
class CEffectTemplate : public CRenderItem
{
    DECLARE_CLASS( CEffectTemplate, CRenderItem )
                    CEffectTemplate                 ( void ) : ClassInit ( this ) {}

    virtual bool            HaveFilesChanged        ( void ) = 0;
    virtual int             GetTicksSinceLastUsed   ( void ) = 0;
    virtual ID3DXEffect*    CloneD3DEffect          ( SString& strOutStatus, bool& bOutUsesVertexShader ) = 0;
    virtual void            UnCloneD3DEffect        ( ID3DXEffect* pD3DEffect ) = 0;
};

CEffectTemplate* NewEffectTemplate ( CRenderItemManager* pManager, const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool bDebug );
