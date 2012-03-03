/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CRealtimeStats.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "SharedUtil.Profiling.hpp"

CORE_API CStatEvents g_StatEvents;

///////////////////////////////////////////////////////////////
//
//
// CRealtimeStats class
//
//
///////////////////////////////////////////////////////////////
class CRealtimeStats : public CRealtimeStatsInterface
{
public:
    ZERO_ON_NEW
                        CRealtimeStats             ( void );
                        ~CRealtimeStats            ( void );

    // CRealtimeStatsInterface methods
    virtual void        FrameEnd                    ( void );
    virtual void        DoPulse                     ( void );
    virtual void        OnCommand                   ( const std::string& strParameters );
    virtual void        RemovedStatSource           ( void );

    // CRealtimeStats methods
protected:
    void                DrawStats                   ( void );
    void                UpdateStatsTexture          ( void );
    void                DrawStatLine                ( int& x, int& y, const SString& strText );
    void                SetVisible                  ( bool bVisible );

    bool                                m_bVisible;
    CStatResults                        m_StatResults;

    CGraphics*                          m_pGraphics;
    std::map < std::string, int >       m_EnabledSectionNames;
    bool                                m_bInvertEnabled;
    float                               m_fNextMaxClearTime;
    CRenderTargetItem*                  m_pRenderTargetItem;
    CElapsedTime                        m_ElapsedTime;
};


///////////////////////////////////////////////////////////////
//
// CRealtimeStats instantiation
//
///////////////////////////////////////////////////////////////
CRealtimeStatsInterface* g_pRealtimeStats = NULL;

CRealtimeStatsInterface* GetRealtimeStats ( void )
{
    if ( !g_pRealtimeStats )
        g_pRealtimeStats = new CRealtimeStats ();
    return g_pRealtimeStats;
}



///////////////////////////////////////////////////////////////
//
// CRealtimeStats implementation
//
//
///////////////////////////////////////////////////////////////
CRealtimeStats::CRealtimeStats ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CRealtimeStats::~CRealtimeStats
//
//
///////////////////////////////////////////////////////////////
CRealtimeStats::~CRealtimeStats ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CRealtimeStats::RemovedStatSource
//
// String pointers may be invalid, so clear all stats
//
///////////////////////////////////////////////////////////////
void CRealtimeStats::RemovedStatSource ( void )
{
    g_StatEvents.ClearBuffer ( false );
}


///////////////////////////////////////////////////////////////
//
// CRealtimeStats::FrameEnd
//
// Save all stats for the next draw
//
///////////////////////////////////////////////////////////////
void CRealtimeStats::FrameEnd ( void )
{
    if ( m_bVisible )
        m_StatResults.FrameEnd ();
    else
        g_StatEvents.ClearBuffer ( false );
}


///////////////////////////////////////////////////////////////
//
// CRealtimeStats::DoPulse
//
// Update
//
///////////////////////////////////////////////////////////////
void CRealtimeStats::DoPulse ( void )
{
    // Draw the enabled stats
    CLOCK( "Stats", "Draw" );
    DrawStats (  );
    UNCLOCK( "Stats", "Draw" );
}


///////////////////////////////////////////////////////////////
//
// CRealtimeStats::OnCommand
//
// Choose section to display by name
//
///////////////////////////////////////////////////////////////
void CRealtimeStats::OnCommand ( const std::string& strParametersIn )
{
    std::vector < SString > parts;
    SString ( strParametersIn ).ToLower ().Split ( " ", parts );

    for ( uint i = 0 ; i < parts.size () ; i++ )
    {
        const SString& strPart = parts[i];
        if ( strPart.empty () )
            continue;

        if ( strPart == "all" )
        {
            m_bInvertEnabled = true;
            m_EnabledSectionNames.clear ();
            break;
        }

        if ( strPart == "none" )
        {
            m_bInvertEnabled = false;
            m_EnabledSectionNames.clear ();
        }
        else
        {
            if ( MapContains ( m_EnabledSectionNames, strPart ) )
                MapRemove ( m_EnabledSectionNames, strPart );
            else
                MapSet ( m_EnabledSectionNames, strPart, 1 );
        }
    }

    if ( !m_EnabledSectionNames.empty () || m_bInvertEnabled )
        SetVisible ( true );
    else
        SetVisible ( false );
}


///////////////////////////////////////////////////////////////
//
// CRealtimeStats::SetVisible
//
//
//
///////////////////////////////////////////////////////////////
void CRealtimeStats::SetVisible ( bool bVisible )
{
    m_bVisible = bVisible;
    g_StatEvents.SetEnabled ( m_bVisible );
}


///////////////////////////////////////////////////////////////
//
// CRealtimeStats::DrawStats
//
//
//
///////////////////////////////////////////////////////////////
void CRealtimeStats::DrawStats ( void )
{
    if ( !m_bVisible )
    {
        SAFE_RELEASE ( m_pRenderTargetItem );
        return;
    }

	m_pGraphics = CGraphics::GetSingletonPtr ();
    int width = m_pGraphics->GetViewportWidth ();
    int height = m_pGraphics->GetViewportHeight ();

    m_pGraphics->SetBlendMode ( EBlendMode::MODULATE_ADD );

    UpdateStatsTexture (  );

    m_pGraphics->SetBlendMode ( EBlendMode::ADD );

    if ( m_pRenderTargetItem )
        m_pGraphics->DrawTextureQueued ( 0, 0, width, height, 0, 0, 1, 1, true, m_pRenderTargetItem, 0, 0, 0, -1, true );

    m_pGraphics->SetBlendMode ( EBlendMode::BLEND );
}


///////////////////////////////////////////////////////////////
//
// CRealtimeStats::UpdateStatsTexture
//
//
//
///////////////////////////////////////////////////////////////
void CRealtimeStats::UpdateStatsTexture ( void )
{
    CRenderItemManagerInterface* pRenderItemManager = m_pGraphics->GetRenderItemManager ();
    int width = m_pGraphics->GetViewportWidth ();
    int height = m_pGraphics->GetViewportHeight ();

    bool bDraw = false;
    if ( m_ElapsedTime.Get () > 1000 )
    {
        bDraw = true;
        m_ElapsedTime.Reset ();
    }

    if ( bDraw )
    {
        m_pGraphics->EnableSetRenderTarget ( true );
        if ( m_pRenderTargetItem )
            pRenderItemManager->SetRenderTarget ( m_pRenderTargetItem, true );
    }

    int x = 800;
    int y = 200;

    int columnWidth = m_pGraphics->GetViewportWidth () / 3;
    x = m_pGraphics->GetViewportWidth () - columnWidth;

    y = y - 15;
    bool bDoneKey = false;

    const SStatResultCollection& collection = m_StatResults.m_CollectionCombo;

    for ( std::map < std::string, SStatResultSection > :: const_iterator itSection = collection.begin () ; itSection != collection.end () ; itSection++ )
    {
        const SString& sectionName  = itSection->first;
        const SStatResultSection& section = itSection->second;

        if ( MapContains ( m_EnabledSectionNames, sectionName.ToLower () ) == m_bInvertEnabled )
            continue;

        if ( !m_pRenderTargetItem )
        {
            m_pRenderTargetItem = pRenderItemManager->CreateRenderTarget ( width, height, true );
        }

        if ( !bDoneKey )
        {
            bDoneKey = true;
            if ( bDraw )
                DrawStatLine( x, y, "[#calls] callsMS (2 sec peak) {2 sec total}" );
        }

        if ( bDraw )
            DrawStatLine( x, y, sectionName );

        if ( bDraw )
            DrawStatLine( x, y, "------------------" );

        for ( std::map < std::string, SStatResultItem > :: const_iterator itItem = section.begin () ; itItem != section.end () ; itItem++ )
        {
            const SString& itemName = itItem->first;
            const SStatResultItem& item   = itItem->second;

            if ( item.fMsTotal > 1 || item.iCounterTotalAcc > 0 )
            {
                if ( bDraw )
                    DrawStatLine( x, y, SString ( "[%u] %2.1f ms (%2.1f ms) {[%u] %2.1f ms %2.0f%%} %s", item.iCounter, item.fMs, item.fMsMax, item.iCounterTotal, item.fMsTotal, item.fMsTotalPercent, itemName.c_str () ) );
            }
            else
            {
                y += 15;
                if ( y > Min < int > ( 800, m_pGraphics->GetViewportHeight () - 30 ) )
                {
                    y = 200;
                    x -= columnWidth;
                }
            }
        }
        if ( bDraw )
            DrawStatLine( x, y, "" );
    }

    if ( bDraw )
    {
        if ( m_pRenderTargetItem )
            pRenderItemManager->RestoreDefaultRenderTarget ();
        m_pGraphics->EnableSetRenderTarget ( false );
    }
}


///////////////////////////////////////////////////////////////
//
// CRealtimeStats::DrawStatLine
//
//
//
///////////////////////////////////////////////////////////////
void CRealtimeStats::DrawStatLine ( int& x, int& y, const SString& strText )
{
    int columnWidth = m_pGraphics->GetViewportWidth () / 3;

    m_pGraphics->DrawText ( x+1, y+1, x+columnWidth-10, y+100, 0xFF000000, strText, 1, 1, 0, NULL );
    m_pGraphics->DrawText ( x, y, x+columnWidth-10, y+100, 0xFFFFFFFF, strText, 1, 1, 0, NULL );

    y += 15;
    if ( y > Min < int > ( 800, m_pGraphics->GetViewportHeight () - 30 ) )
    {
        y = 200;
        x -= columnWidth;
    }
}
