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
        static std::stringstream ss;
        static bool ss_init = false;
        static std::string empty_string;
        if (!ss_init) { ss.imbue(std::locale("")); ss_init = true; }

        ss.str(empty_string);

        ss.precision(numberOfDecimalPlaces);
        ss << std::fixed << value;
        return SString ( ss.str() );
    }


    //
    // Faster type of SString::Split
    // Uses pointers to a big buffer rather than an array of SStrings
    //
    class CSplitString : public std::vector < const char* >
    {
    public:
        CSplitString ( void ) {}
        CSplitString ( const SString& strInput, const SString& strDelim, unsigned int uiMaxAmount = 0, unsigned int uiMinAmount = 0 )
        {
            Split ( strInput, strDelim, uiMaxAmount, uiMinAmount );
        }

        void Split ( const SString& strInput, const SString& strDelim, unsigned int uiMaxAmount = 0, unsigned int uiMinAmount = 0 )
        {
            // Copy string to buffer
            uint iInputLength = strInput.length ();
            buffer.resize ( iInputLength + 1 );
            memcpy ( &buffer[0], &strInput[0], iInputLength + 1 );

            // Prime result list
            clear ();
            reserve ( Min ( 16U, uiMaxAmount ) );

            // Split into pointers
            unsigned long ulCurrentPoint = 0;
            while ( true )
            {
                unsigned long ulPos = strInput.find ( strDelim, ulCurrentPoint );
                if ( ulPos == SString::npos || ( uiMaxAmount > 0 && uiMaxAmount <= size () + 1 ) )
                {
                    if ( ulCurrentPoint <= strInput.length () )
                        push_back ( &buffer[ ulCurrentPoint ] );
                    break;
                }
                push_back ( &buffer[ ulCurrentPoint ] );
                buffer[ ulPos ] = 0;
                ulCurrentPoint = ulPos + strDelim.length ();
            }
            while ( size () < uiMinAmount )
                push_back ( &buffer[ iInputLength ] );        
        }

    protected:
        std::vector < char > buffer;
    };


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

        // Add a new row of data
        void AddRow ( const SString& strRow )
        {
            CSplitString splitString ( strRow, m_strColumnDivider, 0, m_ColumnList.size () );

            for ( uint i = 0 ; i < splitString.size () ; i++ )
            {
                SColumn& column = m_ColumnList[i];
                column.strText += SStringX ( splitString[i] ) + "\n";
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
                g_pGraphics->DrawTextQueued ( fX, fY, fX + column.uiWidth, fY, 0xFFFFFFFF, *column.strText, 1, 1, DT_NOCLIP | column.uiAlignment, NULL, true );
                fX += column.uiWidth;
            }
        }

    protected:
        SString                     m_strColumnDivider;
        uint                        m_uiTotalWidth;
        uint                        m_uiNumRows;
        std::vector < SColumn >     m_ColumnList;
    };


    //
    // Memory state used in CMemStats
    //
    struct SMemStatsInfo
    {
        CProxyDirect3DDevice9::SMemoryState d3dMemory;
        SDxStatus dxStatus;
        int iProcessMemSizeKB;
        int iStreamingMemoryUsed;
        int iStreamingMemoryAvailable;

        union {
            uint uiArray[];
            struct
            {
                uint uiPlayerModels_0_312;
                uint uiUnknown_313_317;
                uint uiWeaponModels_318_372;
                uint uiUnknown_373_399;
                uint uiVehicles_400_611;
                uint uiUnknown_612_999;
                uint uiUpgrades_1000_1193;
                uint uiUnknown_1194_19999;
                uint uiTextures_20000_24999;
                uint uiCollisions_25000_25254;
                uint uiIpls_25255_25510;
                uint uiPaths_25511_25574;
                uint uiAnims_25575_25754;
                uint uiTotal;
            };
        } modelInfo;
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

protected:
    void                SampleValues            ( void );
    void                CreateTables            ( void );

    CElapsedTime            m_UpdateTimer;
    bool                    m_bEnabled;
    SMemStatsInfo           m_MemStatsNow;
    SMemStatsInfo           m_MemStatsPrev;
    SMemStatsInfo           m_MemStatsDelta;
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
    m_bEnabled = bEnabled;
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

    // Time to update?
    if ( m_UpdateTimer.Get () > 1000 )
    {
        m_UpdateTimer.Reset ();
        SampleValues();
        CreateTables();
    }

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
// CMemStats::SampleValues
//
// Sample values
//
///////////////////////////////////////////////////////////////
void CMemStats::SampleValues ( void )
{
    memset ( &m_MemStatsNow, 0, sizeof ( m_MemStatsNow ) );
    memset ( &m_MemStatsDelta, 0, sizeof ( m_MemStatsDelta ) );

    //
    // Update 'now' state
    //
    m_MemStatsNow.d3dMemory = g_pDeviceState->MemoryState;

    g_pGraphics->GetRenderItemManager ()->GetDxStatus ( m_MemStatsNow.dxStatus );

    PROCESS_MEMORY_COUNTERS psmemCounters;  
    if ( GetProcessMemoryInfo ( GetCurrentProcess (), &psmemCounters, sizeof ( psmemCounters ) ) )
        m_MemStatsNow.iProcessMemSizeKB = psmemCounters.WorkingSetSize / 1024LL;

    m_MemStatsNow.iStreamingMemoryUsed                  = *(int*)0x08E4CB4;
    m_MemStatsNow.iStreamingMemoryAvailable             = *(int*)0x08A5A80;

    uint* pModelInfoArray = (uint*)0x08E4CC0;
    for ( uint i = 0 ; i < 25755 ; i++ )
    {
        uint* pModelInfo = pModelInfoArray + 5 * i;
        uint uiLoadedFlag = pModelInfo[4];
        if ( uiLoadedFlag )
        {
            m_MemStatsNow.modelInfo.uiTotal++;
            if ( i < 313 )          m_MemStatsNow.modelInfo.uiPlayerModels_0_312++;
            else if ( i < 318 )     m_MemStatsNow.modelInfo.uiUnknown_313_317++;
            else if ( i < 373 )     m_MemStatsNow.modelInfo.uiWeaponModels_318_372++;
            else if ( i < 400 )     m_MemStatsNow.modelInfo.uiUnknown_373_399++;
            else if ( i < 612 )     m_MemStatsNow.modelInfo.uiVehicles_400_611++;
            else if ( i < 1000 )    m_MemStatsNow.modelInfo.uiUnknown_612_999++;
            else if ( i < 1194 )    m_MemStatsNow.modelInfo.uiUpgrades_1000_1193++;
            else if ( i < 20000 )   m_MemStatsNow.modelInfo.uiUnknown_1194_19999++;
            else if ( i < 25000 )   m_MemStatsNow.modelInfo.uiTextures_20000_24999++;
            else if ( i < 25255 )   m_MemStatsNow.modelInfo.uiCollisions_25000_25254++;
            else if ( i < 25511 )   m_MemStatsNow.modelInfo.uiIpls_25255_25510++;
            else if ( i < 25575 )   m_MemStatsNow.modelInfo.uiPaths_25511_25574++;
            else if ( i < 25755 )   m_MemStatsNow.modelInfo.uiAnims_25575_25754++;
        }
    }

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

    for ( uint i = 0 ; i < NUMELMS( nowList ) ; i++ )
    {
        deltaList[i]->iCreatedCount     = nowList[i]->iCreatedCount - prevList[i]->iCreatedCount;
        deltaList[i]->iCreatedBytes     = nowList[i]->iCreatedBytes - prevList[i]->iCreatedBytes;
        deltaList[i]->iDestroyedCount   = nowList[i]->iDestroyedCount - prevList[i]->iDestroyedCount;
        deltaList[i]->iDestroyedBytes   = nowList[i]->iDestroyedBytes - prevList[i]->iDestroyedBytes;
        deltaList[i]->iLockedCount      = nowList[i]->iLockedCount - prevList[i]->iLockedCount;
    }

    //
    // Set 'prev' for next time
    //
    m_MemStatsPrev = m_MemStatsNow;
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
// static
        const char* const nameList[] = {  "Vertices", "Vertices dynamic", "Indices", "Indices dynamic", "Textures", "Textures dynamic" };

        static const CProxyDirect3DDevice9::SResourceMemory* const nowList[] = {    &m_MemStatsNow.d3dMemory.StaticVertexBuffer, &m_MemStatsNow.d3dMemory.DynamicVertexBuffer,
                                                                                    &m_MemStatsNow.d3dMemory.StaticIndexBuffer, &m_MemStatsNow.d3dMemory.DynamicIndexBuffer,
                                                                                    &m_MemStatsNow.d3dMemory.StaticTexture, &m_MemStatsNow.d3dMemory.DynamicTexture };

        static const CProxyDirect3DDevice9::SResourceMemory* const deltaList[] = {  &m_MemStatsDelta.d3dMemory.StaticVertexBuffer, &m_MemStatsDelta.d3dMemory.DynamicVertexBuffer,
                                                                                    &m_MemStatsDelta.d3dMemory.StaticIndexBuffer, &m_MemStatsDelta.d3dMemory.DynamicIndexBuffer,
                                                                                    &m_MemStatsDelta.d3dMemory.StaticTexture, &m_MemStatsDelta.d3dMemory.DynamicTexture };

        m_TableList.push_back ( CDxTable ( "|" ) );
        CDxTable& table = m_TableList.back ();
        table.SetColumnWidths( "100,45:R,40:R,40:R,50:R,60:R" );
        table.AddRow( "GTA vid memory|Lock|Creat|Dstry|Count|Using KB" );
        for ( uint i = 0 ; i < NUMELMS( nameList ) ; i++ )
        {
            table.AddRow( SString ( "%s|%d|%d|%d|%d|%s"
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
        table.AddRow ( "MTA vid memory|Change KB|Using KB" );
        table.AddRow ( SString ( "FreeForMTA|%s|%s", *FormatNumberWithCommas ( m_MemStatsDelta.dxStatus.videoMemoryKB.iFreeForMTA ), *FormatNumberWithCommas ( m_MemStatsNow.dxStatus.videoMemoryKB.iFreeForMTA ) ) );
        table.AddRow ( SString ( "Fonts|%s|%s", *FormatNumberWithCommas ( m_MemStatsDelta.dxStatus.videoMemoryKB.iUsedByFonts ), *FormatNumberWithCommas ( m_MemStatsNow.dxStatus.videoMemoryKB.iUsedByFonts ) ) );
        table.AddRow ( SString ( "Textures|%s|%s", *FormatNumberWithCommas ( m_MemStatsDelta.dxStatus.videoMemoryKB.iUsedByTextures ), *FormatNumberWithCommas ( m_MemStatsNow.dxStatus.videoMemoryKB.iUsedByTextures ) ) );
        table.AddRow ( SString ( "RenderTargets|%s|%s", *FormatNumberWithCommas ( m_MemStatsDelta.dxStatus.videoMemoryKB.iUsedByTextures ), *FormatNumberWithCommas ( m_MemStatsNow.dxStatus.videoMemoryKB.iUsedByTextures ) ) );
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
        table.AddRow ( "GTA memory| |Change KB|Using KB" );
        table.AddRow ( SString ( "Process memory| | |%s", *FormatNumberWithCommas ( m_MemStatsNow.iProcessMemSizeKB - m_MemStatsNow.iStreamingMemoryUsed / 1024 ) ) );
        table.AddRow ( SString ( "Streaming memory| |%s|%s", *FormatNumberWithCommas ( m_MemStatsDelta.iStreamingMemoryUsed / 1024 ), *FormatNumberWithCommas ( m_MemStatsNow.iStreamingMemoryUsed / 1024 ) ) );
        table.AddRow ( SString ( "|Total:|%s|%s", *FormatNumberWithCommas ( m_MemStatsDelta.iProcessMemSizeKB ), *FormatNumberWithCommas ( m_MemStatsNow.iProcessMemSizeKB ) ) );
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
        table.AddRow ( "GTA settings|Setting KB" );
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
        table.AddRow ( "Models in use| |Change|Count" );
        table.AddRow ( SString ( "0-312|(Players)|%d|%d", m_MemStatsDelta.modelInfo.uiPlayerModels_0_312, m_MemStatsNow.modelInfo.uiPlayerModels_0_312 ) );
        table.AddRow ( SString ( "313-317| |%d|%d", m_MemStatsDelta.modelInfo.uiUnknown_313_317, m_MemStatsNow.modelInfo.uiUnknown_313_317 ) );
        table.AddRow ( SString ( "318-372|(Weapons)|%d|%d", m_MemStatsDelta.modelInfo.uiWeaponModels_318_372, m_MemStatsNow.modelInfo.uiWeaponModels_318_372 ) );
        table.AddRow ( SString ( "373-399| |%d|%d", m_MemStatsDelta.modelInfo.uiUnknown_373_399, m_MemStatsNow.modelInfo.uiUnknown_373_399 ) );
        table.AddRow ( SString ( "400-611|(Vehicles)|%d|%d", m_MemStatsDelta.modelInfo.uiVehicles_400_611, m_MemStatsNow.modelInfo.uiVehicles_400_611 ) );
        table.AddRow ( SString ( "612-999| |%d|%d", m_MemStatsDelta.modelInfo.uiUnknown_612_999, m_MemStatsNow.modelInfo.uiUnknown_612_999 ) );
        table.AddRow ( SString ( "1000-1193|(Upgrades)|%d|%d", m_MemStatsDelta.modelInfo.uiUpgrades_1000_1193, m_MemStatsNow.modelInfo.uiUpgrades_1000_1193 ) );
        table.AddRow ( SString ( "1194-19999|(World)|%d|%d", m_MemStatsDelta.modelInfo.uiUnknown_1194_19999, m_MemStatsNow.modelInfo.uiUnknown_1194_19999 ) );
        table.AddRow ( SString ( "20000-24999|(Textures)|%d|%d", m_MemStatsDelta.modelInfo.uiTextures_20000_24999, m_MemStatsNow.modelInfo.uiTextures_20000_24999 ) );
        table.AddRow ( SString ( "25000-25254|(Collisions)|%d|%d", m_MemStatsDelta.modelInfo.uiCollisions_25000_25254, m_MemStatsNow.modelInfo.uiCollisions_25000_25254 ) );
        table.AddRow ( SString ( "25255-25510|(Ipls)|%d|%d", m_MemStatsDelta.modelInfo.uiIpls_25255_25510, m_MemStatsNow.modelInfo.uiIpls_25255_25510 ) );
        table.AddRow ( SString ( "25511-25574|(Paths)|%d|%d", m_MemStatsDelta.modelInfo.uiPaths_25511_25574, m_MemStatsNow.modelInfo.uiPaths_25511_25574 ) );
        table.AddRow ( SString ( "25575-25754|(Anims)|%d|%d", m_MemStatsDelta.modelInfo.uiAnims_25575_25754, m_MemStatsNow.modelInfo.uiAnims_25575_25754 ) );
        table.AddRow ( SString ( "|Total:|%d|%d", m_MemStatsDelta.modelInfo.uiTotal, m_MemStatsNow.modelInfo.uiTotal ) );
    }
}
