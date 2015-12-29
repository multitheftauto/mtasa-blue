/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CRenderItem.EffectMan.cpp
*  PURPOSE:
*
*****************************************************************************/

#include "StdInc.h"
#include "CRenderItem.EffectCloner.h"


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CEffectClonerImpl
//
class CEffectClonerImpl : public CEffectCloner
{
public:
    ZERO_ON_NEW

                            CEffectClonerImpl       ( CRenderItemManager* pManager );
    virtual                 ~CEffectClonerImpl      ( void );
    virtual void            DoPulse                 ( void );
    virtual ID3DXEffect*    CreateD3DEffect         ( const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool& bOutUsesVertexShader, bool& bOutUsesDepthBuffer, bool bDebug );
    virtual void            ReleaseD3DEffect        ( ID3DXEffect* pD3DEffect );

    // CEffectClonerImpl
    void                    MaybeTidyUp             ( bool bForceDrasticMeasures = false );

    CElapsedTime                                m_TidyupTimer;
    CRenderItemManager*                         m_pManager;
    std::map < ID3DXEffect*, CEffectTemplate* > m_CloneMap;
    std::map < SString, CEffectTemplate* >      m_ValidMap;        // Active and files not changed since first created   
    std::vector < CEffectTemplate* >            m_OldList;         // Active but files changed since first created
    uint                                        m_uiCloneFailTotalCount;
    uint                                        m_uiCloneSuccessTotalCount;
};


///////////////////////////////////////////////////////////////
//
// CEffectCloner instantiation
//
///////////////////////////////////////////////////////////////
CEffectCloner* NewEffectCloner ( CRenderItemManager* pManager )
{
    return new CEffectClonerImpl ( pManager );
}


////////////////////////////////////////////////////////////////
//
// CEffectClonerImpl::CEffectClonerImpl
//
//
//
////////////////////////////////////////////////////////////////
CEffectClonerImpl::CEffectClonerImpl ( CRenderItemManager* pManager )
{
    m_TidyupTimer.SetMaxIncrement ( 500, true );
    m_pManager = pManager;
}


////////////////////////////////////////////////////////////////
//
// CEffectClonerImpl::~CEffectClonerImpl
//
//
//
////////////////////////////////////////////////////////////////
CEffectClonerImpl::~CEffectClonerImpl ( void )
{
}


////////////////////////////////////////////////////////////////
//
// CEffectClonerImpl::CreateD3DEffect
//
//
//
////////////////////////////////////////////////////////////////
ID3DXEffect* CEffectClonerImpl::CreateD3DEffect ( const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool& bOutUsesVertexShader, bool& bOutUsesDepthBuffer, bool bDebug )
{
    // Do we have a match with the initial path
    CEffectTemplate* pEffectTemplate = MapFindRef ( m_ValidMap, ConformPathForSorting ( strFilename ) );
    CEffectTemplate* pEffectTemplatePrevDebug = NULL;
    if ( pEffectTemplate )
    {
        // Have files changed since create?
        if ( pEffectTemplate->HaveFilesChanged () )
        {
            // EffectTemplate is no good for cloning now, so move it to the old list 
            MapRemove ( m_ValidMap, ConformPathForSorting ( strFilename ) );
            m_OldList.push_back ( pEffectTemplate );
            pEffectTemplatePrevDebug = pEffectTemplate;
            pEffectTemplate = NULL;
        }
    }

    // Need to create new EffectTemplate?
    if ( !pEffectTemplate )
    {
        for( uint i = 0 ; i < 2 ; i++ )
        {
            HRESULT hr;
            pEffectTemplate = NewEffectTemplate ( m_pManager, strFilename, strRootPath, strOutStatus, bDebug, hr );
            if ( pEffectTemplate || hr != E_OUTOFMEMORY || i > 0 )
                break;
            // Remove unused effects from memory any try again
            AddReportLog( 7542, SString( "NewEffectTemplate E_OUTOFMEMORY - Attempting to free unused resources (%s) %s", *strOutStatus, *strFilename ) );
            MaybeTidyUp( true );
        }
        if ( !pEffectTemplate )
        {
            AddReportLog( 7544, SString( "NewEffectTemplate failed (%s) %s", *strOutStatus, *strFilename ) );
            return NULL;
        }

        OutputDebugLine ( SString ( "[Shader] CEffectClonerImpl::CreateD3DEffect - New EffectTemplate for %s", *strFilename ) );
        // Add to active map
        MapSet ( m_ValidMap, ConformPathForSorting ( strFilename ), pEffectTemplate );
    }


    //
    // Now we have a valid EffectTemplate to clone the effect from
    //

    // Clone D3DXEffect
    ID3DXEffect* pNewD3DEffect;
    for( uint i = 0 ; i < 2 ; i++ )
    {
        HRESULT hr;
        pNewD3DEffect = pEffectTemplate->CloneD3DEffect ( strOutStatus, bOutUsesVertexShader, bOutUsesDepthBuffer, hr );
        if ( !pNewD3DEffect || hr != E_OUTOFMEMORY || i > 0 )
            break;
        // Remove unused effects from memory any try again
        AddReportLog( 7543, SString( "CloneD3DEffect E_OUTOFMEMORY - Attempting to free unused resources (%s) %s", *strOutStatus, *strFilename ) );
        MaybeTidyUp( true );
    }

    if( !pNewD3DEffect )
    {
        if ( strOutStatus.empty () )
            strOutStatus = "Error: Clone failed";

        // Debug #9085 - CloneEffect failure
        m_uiCloneFailTotalCount++;
        {
            CEffectTemplate::SDebugInfo debugInfo = pEffectTemplate->GetDebugInfo();
            SString strMessage( "CloneEffect failed %08x age:%d [fail:%d success:%d] [totfail:%d totsuccess:%d] '%s'"
                                    ,debugInfo.cloneResult
                                    ,( CTickCount::Now() - debugInfo.createTime ).ToInt()
                                    ,debugInfo.uiCloneFailCount
                                    ,debugInfo.uiCloneSuccessCount
                                    ,m_uiCloneFailTotalCount
                                    ,m_uiCloneSuccessTotalCount
                                    ,*strFilename
                                );
            AddReportLog( 7545, strMessage, 20 );
        }
        if ( pEffectTemplatePrevDebug )
        {
            CEffectTemplate::SDebugInfo debugInfo = pEffectTemplatePrevDebug->GetDebugInfo();
            SString strMessage( "CloneEffect PrevInfo: %08x age:%d [fail:%d success:%d]"
                                    ,debugInfo.cloneResult
                                    ,( CTickCount::Now() - debugInfo.createTime ).ToInt()
                                    ,debugInfo.uiCloneFailCount
                                    ,debugInfo.uiCloneSuccessCount
                                );
            AddReportLog( 7546, strMessage, 20 );
        }
        return NULL;
    }
    m_uiCloneSuccessTotalCount++;

    // Cross ref clone with original
    MapSet ( m_CloneMap, pNewD3DEffect, pEffectTemplate );

    // Return result
    return pNewD3DEffect;
}


////////////////////////////////////////////////////////////////
//
// CEffectClonerImpl::ReleaseD3DEffect
//
// Remove all refs to the d3d effect
//
////////////////////////////////////////////////////////////////
void CEffectClonerImpl::ReleaseD3DEffect ( ID3DXEffect* pD3DEffect )
{
    // Find pEffectTemplate from which this d3d effect was cloned from
    CEffectTemplate* pEffectTemplate = MapFindRef ( m_CloneMap, pD3DEffect );
    assert ( pEffectTemplate );

    // Remove from clone map
    MapRemove ( m_CloneMap, pD3DEffect );

    // Remove from pEffectTemplate. This will alse release the d3d effect.
    pEffectTemplate->UnCloneD3DEffect ( pD3DEffect );
}


////////////////////////////////////////////////////////////////
//
// CEffectClonerImpl::DoPulse
//
//
//
////////////////////////////////////////////////////////////////
void CEffectClonerImpl::DoPulse ( void )
{
    MaybeTidyUp ();
}


////////////////////////////////////////////////////////////////
//
// CEffectClonerImpl::MaybeTidyUp
//
// Tidy up if been a little while since last time
//
////////////////////////////////////////////////////////////////
void CEffectClonerImpl::MaybeTidyUp ( bool bForceDrasticMeasures )
{
    if ( !bForceDrasticMeasures && m_TidyupTimer.Get () < 1000 )
        return;

    m_TidyupTimer.Reset ();

    // Everything in Old List can go if not being used
    for ( uint i = 0 ; i < m_OldList.size () ; i++ )
    {
        CEffectTemplate* pEffectTemplate = m_OldList[i];
        if ( pEffectTemplate->GetTicksSinceLastUsed () > ( bForceDrasticMeasures ? 0 : 1 ) )
        {
            OutputDebugLine ( "[Shader] CEffectClonerImpl::MaybeTidyUp: Releasing old EffectTemplate" );
            SAFE_RELEASE( pEffectTemplate );
            ListRemoveIndex ( m_OldList, i-- );
        }
    }

    // Complex calculation to guess how long to leave an effect unused before deleting
    // 0=30 mins  100=25 mins  200=16 mins  300=1 sec
    float fTicksAlpha = UnlerpClamped ( 0, m_ValidMap.size (), 300 );
    int iTicks = static_cast < int > ( ( 1 - fTicksAlpha * fTicksAlpha ) * 30 * 60 * 1000 ) + 1000;

#ifdef MTA_DEBUG
    iTicks /= 60;  // Mins to seconds for debug
#endif

    // Valid Effect not used for a little while can go
    for ( std::map < SString, CEffectTemplate* >::iterator iter = m_ValidMap.begin () ; iter != m_ValidMap.end () ; )
    {
        CEffectTemplate* pEffectTemplate = iter->second;
        if ( pEffectTemplate->GetTicksSinceLastUsed () > ( bForceDrasticMeasures ? 0 : iTicks ) )
        {
            OutputDebugLine ( "[Shader] CEffectClonerImpl::MaybeTidyUp: Releasing valid EffectTemplate" );
            SAFE_RELEASE( pEffectTemplate );
            m_ValidMap.erase ( iter++ );
        }
        else
            ++iter;
    }

    if ( bForceDrasticMeasures )
    {
        CGraphics::GetSingleton().GetDevice()->EvictManagedResources();
    }
}
