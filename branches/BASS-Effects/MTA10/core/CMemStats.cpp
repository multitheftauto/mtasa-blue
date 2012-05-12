/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CMemStats.cpp
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include <Psapi.h>
#include <game/CGame.h>

namespace
{
    //
    // Helper functions/classes for CMemStats
    //

    //
    // Commas in big numbers
    //
    template<class T>
    static SString FormatNumberWithCommas(T value, int numberOfDecimalPlaces = 0)
    {
#if 0
        static std::stringstream ss;
        static bool ss_init = false;
        static std::string empty_string;
        if (!ss_init) { ss.imbue(std::locale("")); ss_init = true; }

        ss.str(empty_string);

        ss.precision(numberOfDecimalPlaces);
        ss << std::fixed << value;
#else
        std::stringstream ss;
        ss << value;
#endif
        return SString ( ss.str() );
    }


    //
    // For drawing tables with nice neat columns in D3D
    //
    class CDxTable
    {
    public:
        struct SColumn
        {
            uint uiWidth;
            uint uiAlignment;
            SString strText;
        };

        CDxTable ( const SString& strColumnDivider = "," )
            : m_strColumnDivider ( strColumnDivider )
            , m_uiTotalWidth ( 0 )
            , m_uiNumRows ( 0 )
        {
        }

        // Setup number of columns and the width/alignment of each column
        // This also resets the table
        void SetColumnWidths ( const SString& strWidths )
        {
            m_ColumnList.clear ();
            m_uiTotalWidth = 0;
            m_uiNumRows = 0;

            CSplitString splitString ( strWidths, ",", 0, m_ColumnList.size () );

            for ( uint i = 0 ; i < splitString.size () ; i++ )
            {
                CSplitString splitString2 ( splitString[i], ":", 0, 2 );
                uint uiWidth = atoi ( splitString2[0] );
                const char* szAlignment = splitString2[1];

                SColumn column;
                column.uiWidth = uiWidth;
                column.uiAlignment = szAlignment[0] == 'R' ? DT_RIGHT : szAlignment[0] == 'C' ? DT_CENTER : DT_LEFT;
                m_ColumnList.push_back ( column );

                m_uiTotalWidth += uiWidth;
            }
        }

        // For cell colors depending on cell value
        void SetNumberColors ( const SString& strTag, const SString& strColorInfo )
        {
            std::map < int, SString >& valueColorCodeMap = MapGet ( m_ValueColorCodeMapMap, strTag );

            valueColorCodeMap.clear ();

            CSplitString splitString ( strColorInfo, "," );

            for ( uint i = 0 ; i < splitString.size () ; i++ )
            {
                const char* szItem = splitString[i];
                if ( strlen ( szItem ) < 8 )
                    continue;

                int iValue =  atoi ( szItem + 7 );
                MapSet ( valueColorCodeMap, iValue, SStringX ( szItem ).SubStr ( 0, 7 ) );
            }
        }

        // Add a new row of data
        void AddRow ( const SString& strRow )
        {
            CSplitString splitString ( strRow, m_strColumnDivider, 0, m_ColumnList.size () );

            SString strTemp;
            for ( uint i = 0 ; i < splitString.size () ; i++ )
            {
                SColumn& column = m_ColumnList[i];

                const char* szText = splitString[i];

                // Replace ~X0 with X
                if ( const char* szPos = strchr ( szText, '~' ) )
                {
                    uint iPos = (uint)szPos - (uint)szText;
                    SStringX strText ( szText );
                    if ( szPos[2] == '0' )
                    {
                        strTemp = strText.SubStr ( 0, iPos ) + strText.SubStr ( iPos + 1, 1 ) + strText.SubStr ( iPos + 3 );
                    }
                    else
                    {
                        strTemp = strText.SubStr ( 0, iPos ) + strText.SubStr ( iPos + 2 );
                    }
                    szText = strTemp;
                }

                // If it starts with ^, eval following number and prepend color code
                if ( szText[0] == '^' && szText[1] )
                {
                    const std::map < int, SString >& valueColorCodeMap = MapGet ( m_ValueColorCodeMapMap, std::string ( szText, 2 ) );

                    const char* szValue = szText + 2;
                    int iValue = atoi ( szValue );
                    SString strColorCode = "#00FFFF";

                    std::map < int, SString >::const_iterator iterNext = valueColorCodeMap.begin ();
                    if ( iterNext != valueColorCodeMap.end () )
                    {
                        std::map < int, SString >::const_iterator iterCur = iterNext++;
                        for ( ; iterNext != valueColorCodeMap.end () ; ++iterCur, ++iterNext )
                        {
                            if ( iValue < 0 )
                            {
                                if ( iValue < iterNext->first )
                                {
                                    strColorCode = iterCur->second;
                                    break;
                                }
                            }
                            else
                            if ( iValue == 0 )
                            {
                                if ( iValue == iterCur->first )
                                {
                                    strColorCode = iterCur->second;
                                    break;
                                }
                            }
                            else
                            if ( iValue > 0 )
                            {
                                if ( iValue <= iterNext->first )
                                {
                                    strColorCode = iterNext->second;
                                    break;
                                }
                            }
                        }
                    }
                    column.strText += strColorCode;
                    column.strText += szValue;
                }
                else
                {
                    column.strText += szText;
                }
                column.strText += "\n";
            }
            m_uiNumRows++;
        }

        uint GetPixelWidth ( void ) const
        {
            return m_uiTotalWidth;
        }

        uint GetPixelHeight ( void ) const
        {
            return m_uiNumRows * 15;
        }

        // Draw the table with D3D
        void Draw ( float fX, float fY, DWORD dwBackColor, uint uiBorderLeft, uint uiBorderRight, uint uiBorderUp, uint uiBorderDown )
        {
            // Draw background if required
            if ( dwBackColor )
                g_pGraphics->DrawRectQueued (   fX - uiBorderLeft, fY - uiBorderUp, GetPixelWidth () + uiBorderLeft + uiBorderRight, GetPixelHeight () + uiBorderUp + uiBorderDown, dwBackColor, true );

            // Draw each column
            for ( uint i = 0 ; i < m_ColumnList.size () ; i++ )
            {
                const SColumn& column = m_ColumnList[i];
                g_pGraphics->DrawTextQueued ( fX, fY, fX + column.uiWidth, fY, 0xFFFFFFFF, column.strText, 1, 1, DT_NOCLIP | column.uiAlignment, NULL, true, true );
                fX += column.uiWidth;
            }
        }

    protected:
        std::map < SString, std::map < int, SString > >  m_ValueColorCodeMapMap;
        SString                         m_strColumnDivider;
        uint                            m_uiTotalWidth;
        uint                            m_uiNumRows;
        std::vector < SColumn >         m_ColumnList;
    };
}


///////////////////////////////////////////////////////////////
//
// CMemStats class
//
///////////////////////////////////////////////////////////////
class CMemStats : public CMemStatsInterface
{
public:
    ZERO_ON_NEW
                        CMemStats               ( void );
                        ~CMemStats              ( void );

    // CMemStatsInterface methods
    virtual void        Draw                    ( void );
    virtual void        SetEnabled              ( bool bEnabled );
    virtual bool        IsEnabled               ( void );
    virtual void        SampleState             ( SMemStatsInfo& memStatsInfo );

protected:
    void                UpdateFrameStats        ( void );
    void                UpdateIntervalStats     ( void );
    void                CreateTables            ( void );

    CElapsedTime            m_UpdateTimer;
    bool                    m_bEnabled;
    SMemStatsInfo           m_MemStatsNow;
    SMemStatsInfo           m_MemStatsPrev;
    SMemStatsInfo           m_MemStatsDelta;
    SMemStatsInfo           m_MemStatsMax;
    std::list < CDxTable >  m_TableList;
};


///////////////////////////////////////////////////////////////
//
// CMemStats instantiation
//
///////////////////////////////////////////////////////////////
CMemStats* g_pMemStats = NULL;

CMemStatsInterface* GetMemStats ( void )
{
    if ( !g_pMemStats )
        g_pMemStats = new CMemStats ();
    return g_pMemStats;
}


///////////////////////////////////////////////////////////////
//
// CVideoModeManager implementation
//
///////////////////////////////////////////////////////////////
CMemStats::CMemStats ( void )
{
}

CMemStats::~CMemStats ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CMemStats::SetEnabled
//
//
//
///////////////////////////////////////////////////////////////
void CMemStats::SetEnabled ( bool bEnabled )
{
    if ( m_bEnabled != bEnabled )
    {
        m_bEnabled = bEnabled;
        // Clear accumulated changes for first display
        if ( m_bEnabled )
        {
            UpdateIntervalStats ();
            UpdateIntervalStats ();
            CreateTables ();
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CMemStats::IsEnabled
//
//
//
///////////////////////////////////////////////////////////////
bool CMemStats::IsEnabled (void )
{
    return m_bEnabled;
}


///////////////////////////////////////////////////////////////
//
// CMemStats::Draw
//
//
//
///////////////////////////////////////////////////////////////
void CMemStats::Draw ( void )
{
    if ( !m_bEnabled )
        return;

    UpdateFrameStats ();

    // Time to update?
    if ( m_UpdateTimer.Get () > 2000 )
    {
        m_UpdateTimer.Reset ();
        UpdateIntervalStats ();
        CreateTables ();
    }

    // Draw tables
    if ( !m_TableList.empty () )
    {
        float fResWidth = static_cast < float > ( g_pGraphics->GetViewportWidth () );
        float fResHeight = static_cast < float > ( g_pGraphics->GetViewportHeight () );

        float fX = fResWidth - m_TableList.front ().GetPixelWidth () - 15;
        float fY = 200;

        for ( std::list < CDxTable >::iterator iter = m_TableList.begin () ; iter != m_TableList.end () ; ++iter )
        {
            CDxTable& table = *iter;
            table.Draw ( fX, fY, 0x78000000, 10, 10, 8, 8 );
            fY += 20;
            fY += table.GetPixelHeight ();
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CMemStats::UpdateFrameStats
//
// Update values that are measured each frame
//
///////////////////////////////////////////////////////////////
void CMemStats::UpdateFrameStats ( void )
{

    m_MemStatsNow.d3dMemory = g_pDeviceState->MemoryState;

    static CProxyDirect3DDevice9::SResourceMemory* const nowList[] = {    &m_MemStatsNow.d3dMemory.StaticVertexBuffer, &m_MemStatsNow.d3dMemory.DynamicVertexBuffer,
                                                                                &m_MemStatsNow.d3dMemory.StaticIndexBuffer, &m_MemStatsNow.d3dMemory.DynamicIndexBuffer,
                                                                                &m_MemStatsNow.d3dMemory.StaticTexture, &m_MemStatsNow.d3dMemory.DynamicTexture };

    static CProxyDirect3DDevice9::SResourceMemory* const maxList[] = {    &m_MemStatsMax.d3dMemory.StaticVertexBuffer, &m_MemStatsMax.d3dMemory.DynamicVertexBuffer,
                                                                                &m_MemStatsMax.d3dMemory.StaticIndexBuffer, &m_MemStatsMax.d3dMemory.DynamicIndexBuffer,
                                                                                &m_MemStatsMax.d3dMemory.StaticTexture, &m_MemStatsMax.d3dMemory.DynamicTexture };

    CProxyDirect3DDevice9::SResourceMemory* const prevList[] = {   &m_MemStatsPrev.d3dMemory.StaticVertexBuffer, &m_MemStatsPrev.d3dMemory.DynamicVertexBuffer,
                                                                                &m_MemStatsPrev.d3dMemory.StaticIndexBuffer, &m_MemStatsPrev.d3dMemory.DynamicIndexBuffer,
                                                                                &m_MemStatsPrev.d3dMemory.StaticTexture, &m_MemStatsPrev.d3dMemory.DynamicTexture };

    for ( uint i = 0 ; i < NUMELMS( nowList ) ; i++ )
    {
        maxList[i]->iLockedCount = Max ( maxList[i]->iLockedCount, nowList[i]->iLockedCount - prevList[i]->iLockedCount );
        prevList[i]->iLockedCount = nowList[i]->iLockedCount;
    }
}


///////////////////////////////////////////////////////////////
//
// CMemStats::SampleState
//
// Sample current memory state
//
///////////////////////////////////////////////////////////////
void CMemStats::SampleState ( SMemStatsInfo& memStatsInfo )
{
    memset ( &memStatsInfo, 0, sizeof ( memStatsInfo ) );

    //
    // Update 'now' state
    //
    memStatsInfo.d3dMemory = g_pDeviceState->MemoryState;

    g_pGraphics->GetRenderItemManager ()->GetDxStatus ( memStatsInfo.dxStatus );

    PROCESS_MEMORY_COUNTERS psmemCounters;  
    if ( GetProcessMemoryInfo ( GetCurrentProcess (), &psmemCounters, sizeof ( psmemCounters ) ) )
        memStatsInfo.iProcessMemSizeKB = psmemCounters.WorkingSetSize / 1024LL;

    memStatsInfo.iStreamingMemoryUsed                  = *(int*)0x08E4CB4;
    memStatsInfo.iStreamingMemoryAvailable             = *(int*)0x08A5A80;

    uint* pModelInfoArray = (uint*)0x08E4CC0;
    for ( uint i = 0 ; i < 25755 ; i++ )
    {
        uint* pModelInfo = pModelInfoArray + 5 * i;
        uint uiLoadedFlag = pModelInfo[4];
        if ( uiLoadedFlag )
        {
            memStatsInfo.modelInfo.uiTotal++;
            if ( i < 313 )          memStatsInfo.modelInfo.uiPlayerModels_0_312++;
            else if ( i < 318 )     memStatsInfo.modelInfo.uiUnknown_313_317++;
            else if ( i < 373 )     memStatsInfo.modelInfo.uiWeaponModels_318_372++;
            else if ( i < 400 )     memStatsInfo.modelInfo.uiUnknown_373_399++;
            else if ( i < 612 )     memStatsInfo.modelInfo.uiVehicles_400_611++;
            else if ( i < 1000 )    memStatsInfo.modelInfo.uiUnknown_612_999++;
            else if ( i < 1194 )    memStatsInfo.modelInfo.uiUpgrades_1000_1193++;
            else if ( i < 20000 )   memStatsInfo.modelInfo.uiUnknown_1194_19999++;
            else if ( i < 25000 )   memStatsInfo.modelInfo.uiTextures_20000_24999++;
            else if ( i < 25255 )   memStatsInfo.modelInfo.uiCollisions_25000_25254++;
            else if ( i < 25511 )   memStatsInfo.modelInfo.uiIpls_25255_25510++;
            else if ( i < 25575 )   memStatsInfo.modelInfo.uiPaths_25511_25574++;
            else if ( i < 25755 )   memStatsInfo.modelInfo.uiAnims_25575_25754++;
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CMemStats::UpdateIntervalStats
//
// Update stats which are sampled at regular intervals
//
///////////////////////////////////////////////////////////////
void CMemStats::UpdateIntervalStats ( void )
{
    memset ( &m_MemStatsNow, 0, sizeof ( m_MemStatsNow ) );
    memset ( &m_MemStatsDelta, 0, sizeof ( m_MemStatsDelta ) );

    //
    // Update 'now' state
    //
    SampleState ( m_MemStatsNow );

    //
    // Calculate 'delta'
    //
    m_MemStatsDelta.iProcessMemSizeKB = m_MemStatsNow.iProcessMemSizeKB - m_MemStatsPrev.iProcessMemSizeKB;

    m_MemStatsDelta.dxStatus.videoMemoryKB.iFreeForMTA              = m_MemStatsNow.dxStatus.videoMemoryKB.iFreeForMTA          - m_MemStatsPrev.dxStatus.videoMemoryKB.iFreeForMTA;
    m_MemStatsDelta.dxStatus.videoMemoryKB.iUsedByFonts             = m_MemStatsNow.dxStatus.videoMemoryKB.iUsedByFonts         - m_MemStatsPrev.dxStatus.videoMemoryKB.iUsedByFonts;
    m_MemStatsDelta.dxStatus.videoMemoryKB.iUsedByTextures          = m_MemStatsNow.dxStatus.videoMemoryKB.iUsedByTextures      - m_MemStatsPrev.dxStatus.videoMemoryKB.iUsedByTextures;
    m_MemStatsDelta.dxStatus.videoMemoryKB.iUsedByRenderTargets     = m_MemStatsNow.dxStatus.videoMemoryKB.iUsedByRenderTargets - m_MemStatsPrev.dxStatus.videoMemoryKB.iUsedByRenderTargets;

    m_MemStatsDelta.iStreamingMemoryAvailable   = m_MemStatsNow.iStreamingMemoryAvailable   - m_MemStatsPrev.iStreamingMemoryAvailable;
    m_MemStatsDelta.iStreamingMemoryUsed        = m_MemStatsNow.iStreamingMemoryUsed        - m_MemStatsPrev.iStreamingMemoryUsed;

    for ( uint i = 0 ; i < sizeof ( m_MemStatsDelta.modelInfo ) / sizeof ( uint ) ; i++ )
    {
        m_MemStatsDelta.modelInfo.uiArray[i]  = m_MemStatsNow.modelInfo.uiArray[i]  - m_MemStatsPrev.modelInfo.uiArray[i];
    }

    static const CProxyDirect3DDevice9::SResourceMemory* const nowList[] = {    &m_MemStatsNow.d3dMemory.StaticVertexBuffer, &m_MemStatsNow.d3dMemory.DynamicVertexBuffer,
                                                                                &m_MemStatsNow.d3dMemory.StaticIndexBuffer, &m_MemStatsNow.d3dMemory.DynamicIndexBuffer,
                                                                                &m_MemStatsNow.d3dMemory.StaticTexture, &m_MemStatsNow.d3dMemory.DynamicTexture };

    static const CProxyDirect3DDevice9::SResourceMemory* const prevList[] = {   &m_MemStatsPrev.d3dMemory.StaticVertexBuffer, &m_MemStatsPrev.d3dMemory.DynamicVertexBuffer,
                                                                                &m_MemStatsPrev.d3dMemory.StaticIndexBuffer, &m_MemStatsPrev.d3dMemory.DynamicIndexBuffer,
                                                                                &m_MemStatsPrev.d3dMemory.StaticTexture, &m_MemStatsPrev.d3dMemory.DynamicTexture };

    static CProxyDirect3DDevice9::SResourceMemory* const deltaList[] = {        &m_MemStatsDelta.d3dMemory.StaticVertexBuffer, &m_MemStatsDelta.d3dMemory.DynamicVertexBuffer,
                                                                                &m_MemStatsDelta.d3dMemory.StaticIndexBuffer, &m_MemStatsDelta.d3dMemory.DynamicIndexBuffer,
                                                                                &m_MemStatsDelta.d3dMemory.StaticTexture, &m_MemStatsDelta.d3dMemory.DynamicTexture };

    static const CProxyDirect3DDevice9::SResourceMemory* const maxList[] = {    &m_MemStatsMax.d3dMemory.StaticVertexBuffer, &m_MemStatsMax.d3dMemory.DynamicVertexBuffer,
                                                                                &m_MemStatsMax.d3dMemory.StaticIndexBuffer, &m_MemStatsMax.d3dMemory.DynamicIndexBuffer,
                                                                                &m_MemStatsMax.d3dMemory.StaticTexture, &m_MemStatsMax.d3dMemory.DynamicTexture };

    for ( uint i = 0 ; i < NUMELMS( nowList ) ; i++ )
    {
        deltaList[i]->iCreatedCount     = nowList[i]->iCreatedCount - prevList[i]->iCreatedCount;
        deltaList[i]->iCreatedBytes     = nowList[i]->iCreatedBytes - prevList[i]->iCreatedBytes;
        deltaList[i]->iDestroyedCount   = nowList[i]->iDestroyedCount - prevList[i]->iDestroyedCount;
        deltaList[i]->iDestroyedBytes   = nowList[i]->iDestroyedBytes - prevList[i]->iDestroyedBytes;
        deltaList[i]->iLockedCount      = maxList[i]->iLockedCount;     // Use per-frame max for lock stats
    }

    //
    // Set 'prev' for next time
    //
    m_MemStatsPrev = m_MemStatsNow;

    // Clear max records
    memset ( &m_MemStatsMax, 0, sizeof ( m_MemStatsMax ) );
}


///////////////////////////////////////////////////////////////
//
// CMemStats::CreateTables
//
// Create formatted tables for drawing with
//
///////////////////////////////////////////////////////////////
void CMemStats::CreateTables ( void )
{
    m_TableList.clear ();

    //
    // Color setups
    //
    #define YELLOW "#FFFF00"
    #define RED "#FF0000"
    #define BLUE "#0000FF"
    #define WHITE "#FFFFFF"

    #define LT_RED "#FF5050"
    #define DK_RED "#CF0000"
    #define GREY "#808080"
    #define LT_GREY "#C0C0C0"
    #define INVIS "#000000"
    #define DK_GREEN "#00CF00"
    #define LT_GREEN "#30FF30"
    #define PURPLE "#FF00FF"
    #define CYAN "#00FFFF"
    #define LT_CYAN "#00C0F0"

    // Table header
    #define HEADER1(text) LT_CYAN text WHITE

    // Cell colour depending upon the value
    SString strNumberColorsCreat =
                        GREY "0,"
                        CYAN "999999,"
                        ;

    SString strNumberColorsDstry =
                        GREY "0,"
                        PURPLE "999999,"
                        ;

    SString strNumberColorsLockStatic =
                        GREY "0,"
                        YELLOW "999999,"
                        ;

    SString strNumberColorsLockDynamic =
                        WHITE "0,"
                        WHITE "999999,"
                        ;

    SString strNumberColorsMtaVidMem =
                        LT_GREEN "-999999,"
                        GREY "0,"
                        LT_RED "999999,"
                        ;

    SString strNumberColorsModels =
                        LT_GREEN "-99999,"
                        GREY "0,"
                        LT_RED "999999,"
                        ;

    //
    // Key for weird codes in table.AddRow string:
    //
    //      |  is usually the cell delimiter character
    //      ~X means replace any following zero with character X
    //      ^2 means use number color ^2 for the following value
    //

    {
/*
    GTA vidmemory         Lock Create Destroy Total TotalKB
    StaticVertexBuffer      1     1      1      10     1000
    DynamicVertexBuffer     1     1      1      10     1000
    StaticIndexBuffer       1     1      1      10     1000
    DynamicIndexBuffer      1     1      1      10     1000
    StaticTexture           1     1      1      10     1000
    DynamicTexture          1     1      1      10     1000
*/
        static const char* const nameList[] = {  "Vertices", "Vertices dynamic", "Indices", "Indices dynamic", "Textures", "Textures dynamic" };

        static const CProxyDirect3DDevice9::SResourceMemory* const nowList[] = {    &m_MemStatsNow.d3dMemory.StaticVertexBuffer, &m_MemStatsNow.d3dMemory.DynamicVertexBuffer,
                                                                                    &m_MemStatsNow.d3dMemory.StaticIndexBuffer, &m_MemStatsNow.d3dMemory.DynamicIndexBuffer,
                                                                                    &m_MemStatsNow.d3dMemory.StaticTexture, &m_MemStatsNow.d3dMemory.DynamicTexture };

        static const CProxyDirect3DDevice9::SResourceMemory* const deltaList[] = {  &m_MemStatsDelta.d3dMemory.StaticVertexBuffer, &m_MemStatsDelta.d3dMemory.DynamicVertexBuffer,
                                                                                    &m_MemStatsDelta.d3dMemory.StaticIndexBuffer, &m_MemStatsDelta.d3dMemory.DynamicIndexBuffer,
                                                                                    &m_MemStatsDelta.d3dMemory.StaticTexture, &m_MemStatsDelta.d3dMemory.DynamicTexture };

        m_TableList.push_back ( CDxTable ( "|" ) );
        CDxTable& table = m_TableList.back ();
        table.SetColumnWidths( "100,45:R,40:R,40:R,50:R,60:R" );
        table.SetNumberColors ( "^1", strNumberColorsLockStatic );
        table.SetNumberColors ( "^2", strNumberColorsLockDynamic );
        table.SetNumberColors ( "^3", strNumberColorsCreat );
        table.SetNumberColors ( "^4", strNumberColorsDstry );
        table.AddRow ( HEADER1( "GTA vid memory" ) "|" HEADER1( "Lock" ) "|" HEADER1( "Creat" ) BLUE "|" HEADER1( "Dstry" ) "|" HEADER1( "Count" ) "|" HEADER1( "Using KB" ) );
        for ( uint i = 0 ; i < NUMELMS( nameList ) ; i++ )
        {
            if ( i & 1 )
                table.AddRow( SString ( "%s|^2~ %d|^3~.%d|^4~.%d|%d|%s"
                                            ,nameList[i]
                                            ,deltaList[i]->iLockedCount
                                            ,deltaList[i]->iCreatedCount
                                            ,deltaList[i]->iDestroyedCount
                                            ,nowList[i]->iCurrentCount
                                            ,*FormatNumberWithCommas ( nowList[i]->iCurrentBytes / 1024  )
                                      ) );
            else
                table.AddRow( SString ( "%s|^1~ %d|^3~.%d|^4~.%d|%d|%s"
                                            ,nameList[i]
                                            ,deltaList[i]->iLockedCount
                                            ,deltaList[i]->iCreatedCount
                                            ,deltaList[i]->iDestroyedCount
                                            ,nowList[i]->iCurrentCount
                                            ,*FormatNumberWithCommas ( nowList[i]->iCurrentBytes / 1024  )
                                      ) );
        }
    }

    {
/*
    MTA videoMemory           ChangeKB        TotalKB
    FreeForMTA                  -100            1000
    UsedByFonts                 -100            1000
    UsedByTextures              -100            1000
    UsedByRenderTargets         -100            1000
*/
        m_TableList.push_back ( CDxTable ( "|" ) );
        CDxTable& table = m_TableList.back ();
        table.SetColumnWidths( "110,80:R,80:R" );
        table.SetNumberColors ( "^1", strNumberColorsMtaVidMem );
        table.AddRow ( HEADER1( "MTA vid memory" ) "|" HEADER1( "Change KB" ) "|" HEADER1( "Using KB" ) );
        table.AddRow ( SString ( "FreeForMTA|^1~.%s|%s", *FormatNumberWithCommas ( m_MemStatsDelta.dxStatus.videoMemoryKB.iFreeForMTA ), *FormatNumberWithCommas ( m_MemStatsNow.dxStatus.videoMemoryKB.iFreeForMTA ) ) );
        table.AddRow ( SString ( "Fonts|^1~.%s|%s", *FormatNumberWithCommas ( m_MemStatsDelta.dxStatus.videoMemoryKB.iUsedByFonts ), *FormatNumberWithCommas ( m_MemStatsNow.dxStatus.videoMemoryKB.iUsedByFonts ) ) );
        table.AddRow ( SString ( "Textures|^1~.%s|%s", *FormatNumberWithCommas ( m_MemStatsDelta.dxStatus.videoMemoryKB.iUsedByTextures ), *FormatNumberWithCommas ( m_MemStatsNow.dxStatus.videoMemoryKB.iUsedByTextures ) ) );
        table.AddRow ( SString ( "RenderTargets|^1~.%s|%s", *FormatNumberWithCommas ( m_MemStatsDelta.dxStatus.videoMemoryKB.iUsedByRenderTargets ), *FormatNumberWithCommas ( m_MemStatsNow.dxStatus.videoMemoryKB.iUsedByRenderTargets ) ) );
    }

    {
/*
    GTA memory                ChangeKB        TotalKB
    iProcessMemSizeKB            -100            1000
    streamMemSizeKB             -100            1000
*/
        m_TableList.push_back ( CDxTable ( "|" ) );
        CDxTable& table = m_TableList.back ();
        table.SetColumnWidths( "110,30:R,50:R,80:R" );
        table.SetNumberColors ( "^1", strNumberColorsMtaVidMem );
        table.AddRow ( HEADER1( "GTA memory" ) "| |" HEADER1( "Change KB" ) "|" HEADER1( "Using KB" ) );
        table.AddRow ( SString ( "Process memory| | |%s", *FormatNumberWithCommas ( m_MemStatsNow.iProcessMemSizeKB - m_MemStatsNow.iStreamingMemoryUsed / 1024 ) ) );
        table.AddRow ( SString ( "Streaming memory| |^1~.%s|%s", *FormatNumberWithCommas ( m_MemStatsDelta.iStreamingMemoryUsed / 1024 ), *FormatNumberWithCommas ( m_MemStatsNow.iStreamingMemoryUsed / 1024 ) ) );
        table.AddRow ( SString ( "|Total:|^1~.%s|%s", *FormatNumberWithCommas ( m_MemStatsDelta.iProcessMemSizeKB ), *FormatNumberWithCommas ( m_MemStatsNow.iProcessMemSizeKB ) ) );
    }

    {
/*
    Settings                                AmountKB
    videoCardInstalledMemoryKB                 2000
    streamMemSettingKB                         2000
*/
        m_TableList.push_back ( CDxTable ( "|" ) );
        CDxTable& table = m_TableList.back ();
        table.SetColumnWidths( "140,130:R" );
        table.AddRow ( HEADER1( "GTA settings" ) "|" HEADER1( "Setting KB" ) );
        table.AddRow ( SString ( "Video card installed memory|%s", *FormatNumberWithCommas ( m_MemStatsNow.dxStatus.videoCard.iInstalledMemoryKB ) ) );
        table.AddRow ( SString ( "Streaming memory limit|%s", *FormatNumberWithCommas ( m_MemStatsNow.iStreamingMemoryAvailable / 1024 ) ) );
    }

    {
/*
    Model usage                 Change   Amount
    0-288           Players         1       10
    289-399         Other1          1       10
    400-611         Vehicles        1       10
    612-999         Other2          1       10
    1000-1193       Upgrades        1       10
    1194-19999      Other3          1       10
    20000-24999     Textures        1       10
    24999-27000     Other4          1       10
                    Total           1       10
*/
        m_TableList.push_back ( CDxTable ( "|" ) );
        CDxTable& table = m_TableList.back ();
        table.SetColumnWidths( "90,50,50:R,60:R" );
        table.SetNumberColors ( "^1", strNumberColorsModels );
        table.AddRow ( HEADER1( "Models in use" ) "| |" HEADER1( "Change" ) "|" HEADER1( "Count" ) );
        table.AddRow ( SString ( "0-312|(Players)|^1~.%d|%d", m_MemStatsDelta.modelInfo.uiPlayerModels_0_312, m_MemStatsNow.modelInfo.uiPlayerModels_0_312 ) );
        table.AddRow ( SString ( "313-317| |^1~.%d|%d", m_MemStatsDelta.modelInfo.uiUnknown_313_317, m_MemStatsNow.modelInfo.uiUnknown_313_317 ) );
        table.AddRow ( SString ( "318-372|(Weapons)|^1~.%d|%d", m_MemStatsDelta.modelInfo.uiWeaponModels_318_372, m_MemStatsNow.modelInfo.uiWeaponModels_318_372 ) );
        table.AddRow ( SString ( "373-399| |^1~.%d|%d", m_MemStatsDelta.modelInfo.uiUnknown_373_399, m_MemStatsNow.modelInfo.uiUnknown_373_399 ) );
        table.AddRow ( SString ( "400-611|(Vehicles)|^1~.%d|%d", m_MemStatsDelta.modelInfo.uiVehicles_400_611, m_MemStatsNow.modelInfo.uiVehicles_400_611 ) );
        table.AddRow ( SString ( "612-999| |^1~.%d|%d", m_MemStatsDelta.modelInfo.uiUnknown_612_999, m_MemStatsNow.modelInfo.uiUnknown_612_999 ) );
        table.AddRow ( SString ( "1000-1193|(Upgrades)|^1~.%d|%d", m_MemStatsDelta.modelInfo.uiUpgrades_1000_1193, m_MemStatsNow.modelInfo.uiUpgrades_1000_1193 ) );
        table.AddRow ( SString ( "1194-19999|(World)|^1~.%d|%d", m_MemStatsDelta.modelInfo.uiUnknown_1194_19999, m_MemStatsNow.modelInfo.uiUnknown_1194_19999 ) );
        table.AddRow ( SString ( "20000-24999|(Textures)|^1~.%d|%d", m_MemStatsDelta.modelInfo.uiTextures_20000_24999, m_MemStatsNow.modelInfo.uiTextures_20000_24999 ) );
        table.AddRow ( SString ( "25000-25254|(Collisions)|^1~.%d|%d", m_MemStatsDelta.modelInfo.uiCollisions_25000_25254, m_MemStatsNow.modelInfo.uiCollisions_25000_25254 ) );
        table.AddRow ( SString ( "25255-25510|(Ipls)|^1~.%d|%d", m_MemStatsDelta.modelInfo.uiIpls_25255_25510, m_MemStatsNow.modelInfo.uiIpls_25255_25510 ) );
        table.AddRow ( SString ( "25511-25574|(Paths)|^1~.%d|%d", m_MemStatsDelta.modelInfo.uiPaths_25511_25574, m_MemStatsNow.modelInfo.uiPaths_25511_25574 ) );
        table.AddRow ( SString ( "25575-25754|(Anims)|^1~.%d|%d", m_MemStatsDelta.modelInfo.uiAnims_25575_25754, m_MemStatsNow.modelInfo.uiAnims_25575_25754 ) );
        table.AddRow ( SString ( "|Total:|^1~.%d|%d", m_MemStatsDelta.modelInfo.uiTotal, m_MemStatsNow.modelInfo.uiTotal ) );
    }
}
