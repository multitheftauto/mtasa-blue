/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPerfStat.SqliteTiming.cpp
*  PURPOSE:     Performance stats class
*  DEVELOPERS:  Mr OCD
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"


namespace
{
    struct CTimingInfo
    {
        SString strQuery;
        TIMEUS timeUs;
        long long timeStamp;
        SString databaseName;
        SString resourceName;
    };
}


///////////////////////////////////////////////////////////////
//
// CPerfStatSqliteTimingImpl
//
//
//
///////////////////////////////////////////////////////////////
class CPerfStatSqliteTimingImpl : public CPerfStatSqliteTiming
{
public:
    ZERO_ON_NEW
                                CPerfStatSqliteTimingImpl  ( void );
    virtual                     ~CPerfStatSqliteTimingImpl ( void );

    // CPerfStatModule
    virtual const SString&      GetCategoryName         ( void );
    virtual void                DoPulse                 ( void );
    virtual void                GetStats                ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter );

    // CPerfStatSqliteTiming
    virtual void                OnSqliteOpen            ( CRegistry* pRegistry, const SString& strFileName );
    virtual void                OnSqliteClose           ( CRegistry* pRegistry );
    virtual void                UpdateSqliteTiming      ( CRegistry* pRegistry, const char* szQuery, TIMEUS timeUs );
    virtual void                SetCurrentResource      ( lua_State* luaVM );

    // CPerfStatSqliteTimingImpl functions
    void                        GetSqliteTimingStats    ( CPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter );

    SString                             m_strCategoryName;
    long long                           m_llRecordStatsEndTime;
    bool                                m_bDisableBatching;
    std::map < CRegistry*, SString >    m_RegistryMap;
    std::list < CTimingInfo >           m_TimingList;
    lua_State*                          m_currentluaVM;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CPerfStatSqliteTimingImpl* g_pPerfStatSqliteTimingImp = NULL;

CPerfStatSqliteTiming* CPerfStatSqliteTiming::GetSingleton ()
{
    if ( !g_pPerfStatSqliteTimingImp )
        g_pPerfStatSqliteTimingImp = new CPerfStatSqliteTimingImpl ();
    return g_pPerfStatSqliteTimingImp;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatSqliteTimingImpl::CPerfStatSqliteTimingImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatSqliteTimingImpl::CPerfStatSqliteTimingImpl ( void )
{
    m_strCategoryName = "Sqlite timing";
}


///////////////////////////////////////////////////////////////
//
// CPerfStatSqliteTimingImpl::CPerfStatSqliteTimingImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatSqliteTimingImpl::~CPerfStatSqliteTimingImpl ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CPerfStatSqliteTimingImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CPerfStatSqliteTimingImpl::GetCategoryName ( void )
{
    return m_strCategoryName;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatSqliteTimingImpl::OnSqliteOpen
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatSqliteTimingImpl::OnSqliteOpen ( CRegistry* pRegistry, const SString& strFileName )
{
    MapSet ( m_RegistryMap, pRegistry, strFileName.Replace ( "/", "\\" ).SplitRight ( "\\", NULL, -1 ) );
}


///////////////////////////////////////////////////////////////
//
// CPerfStatSqliteTimingImpl::OnSqliteClose
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatSqliteTimingImpl::OnSqliteClose ( CRegistry* pRegistry )
{
    MapRemove ( m_RegistryMap, pRegistry );
}


///////////////////////////////////////////////////////////////
//
// CPerfStatSqliteTimingImpl::UpdateSqliteTiming
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatSqliteTimingImpl::UpdateSqliteTiming ( CRegistry* pRegistry, const char* szQuery, TIMEUS timeUs )
{
    // Only record stats if requested
    if ( GetTickCount64_ () > m_llRecordStatsEndTime )
        return;

    CTimingInfo info;
    info.strQuery = szQuery;
    info.timeUs = timeUs;
    info.timeStamp = GetTickCount64_ ();


    // Get resource name
    if ( m_currentluaVM )
        if ( CResource* pResource = g_pGame->GetResourceManager()->GetResourceFromLuaState ( m_currentluaVM ) )
            info.resourceName = pResource->GetName ();
    m_currentluaVM = NULL;

    // Use registry name if resource name empty
    if ( info.resourceName.empty () )
        if ( SString* pRegistryName = MapFind ( m_RegistryMap, pRegistry ) )
            info.resourceName = *pRegistryName;

    info.resourceName = info.resourceName.SplitRight ( "/", NULL, -1 );

    m_TimingList.push_back ( info );
}


///////////////////////////////////////////////////////////////
//
// CPerfStatSqliteTimingImpl::SetCurrentResource
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatSqliteTimingImpl::SetCurrentResource ( lua_State* luaVM )
{
    m_currentluaVM = luaVM;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatSqliteTimingImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatSqliteTimingImpl::DoPulse ( void )
{
    long long llTime =  GetTickCount64_ ();
    // Remove old stats
    while ( m_TimingList.size () )
    {
        CTimingInfo& info = m_TimingList.front ();
        int iAgeSeconds = static_cast < int > ( ( llTime - info.timeStamp ) / 1000 );
        if ( iAgeSeconds < 2000 && m_TimingList.size () < 1000 )
            break;
        m_TimingList.pop_front ();
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatSqliteTimingImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatSqliteTimingImpl::GetStats ( CPerfStatResult* pResult, const std::map < SString, int >& optionMap, const SString& strFilter )
{
    GetSqliteTimingStats ( pResult, optionMap, strFilter );

    uint uiTicks = 1000 * 10;   // 10 seconds
    long long llTime =  GetTickCount64_ ();

    m_llRecordStatsEndTime = llTime + uiTicks;

    // Update batching setting
    for ( std::map < CRegistry*, SString >::iterator iter = m_RegistryMap.begin () ; iter != m_RegistryMap.end () ; ++iter )
    {
        if ( m_bDisableBatching )
            iter->first->SuspendBatching ( uiTicks );   // Suspend batching
        else
            iter->first->SuspendBatching ( 0 );         // Unsuspend batching
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatSqliteTimingImpl::GetSqliteTimingStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatSqliteTimingImpl::GetSqliteTimingStats ( CPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter )
{
    //
    // Set option flags
    //
    bool bHelp = MapContains ( strOptionMap, "h" );
    m_bDisableBatching = MapContains ( strOptionMap, "b" );
    float fIgnoreCpuMs = 0;
    if ( MapContains ( strOptionMap, "i1" ) )        fIgnoreCpuMs = 0.001f;
    if ( MapContains ( strOptionMap, "i10" ) )       fIgnoreCpuMs = 0.010f;
    if ( MapContains ( strOptionMap, "i100" ) )      fIgnoreCpuMs = 0.100f;
    float fIgnoreAge = 2000;
    if ( MapContains ( strOptionMap, "a10" ) )        fIgnoreAge = 10;
    if ( MapContains ( strOptionMap, "a100" ) )       fIgnoreAge = 100;
    if ( MapContains ( strOptionMap, "a1000" ) )      fIgnoreAge = 1000;
    int iMaxResults = 200;
    if ( MapContains ( strOptionMap, "m10" ) )        iMaxResults = 10;
    if ( MapContains ( strOptionMap, "m100" ) )       iMaxResults = 100;
    if ( MapContains ( strOptionMap, "m1000" ) )      iMaxResults = 1000;

    //
    // Process help
    //
    if ( bHelp )
    {
        pResult->AddColumn ( "Sqlite timings help" );
        pResult->AddRow ()[0] ="Option h - This help";
        pResult->AddRow ()[0] ="Option b - Disable batching (to measure single queries - May slow server a little)";
        pResult->AddRow ()[0] ="Option i1 - Ignore cpu < 1ms";
        pResult->AddRow ()[0] ="Option i10 - Ignore cpu < 10ms";
        pResult->AddRow ()[0] ="Option i100 - Ignore cpu < 100ms";
        pResult->AddRow ()[0] ="Option a10 - Ignore age > 10s";
        pResult->AddRow ()[0] ="Option a100 - Ignore age > 100s";
        pResult->AddRow ()[0] ="Option a1000 - Ignore age > 1000s";
        pResult->AddRow ()[0] ="Option m10 - Max 10 results";
        pResult->AddRow ()[0] ="Option m100 - Max 100 results";
        pResult->AddRow ()[0] ="Option m1000 - Max 1000 results";
        return;
    }

    //
    // Set column names
    //

    pResult->AddColumn ( "age" );
    pResult->AddColumn ( "resource name" );
    pResult->AddColumn ( "cpu seconds" );
    pResult->AddColumn ( m_bDisableBatching ? "query . . *Note: Viewing this page may slow server" : "query" );

    long long llTime = GetTickCount64_ ();
    // Output
    for ( std::list < CTimingInfo >::reverse_iterator iter = m_TimingList.rbegin () ; iter != m_TimingList.rend () ; ++iter )
    {
        const CTimingInfo& info = *iter;

        // Apply filter
        if ( strFilter != "" && info.resourceName.find ( strFilter ) == SString::npos )
            continue;

        float fCpuMs = info.timeUs * ( 1/1000000.f );
        float fAgeSeconds = ( llTime - info.timeStamp ) * ( 1/1000.f );

        if ( fCpuMs < fIgnoreCpuMs )
            continue;

        if ( fAgeSeconds > fIgnoreAge )
            break;

        // Add row
        SString* row = pResult->AddRow ();

        int c = 0;
        row[c++] = SString ( "%2.0f", fAgeSeconds );
        row[c++] = info.resourceName;
        row[c++] = SString ( "%2.3f", fCpuMs );
        row[c++] = info.strQuery;

        if ( --iMaxResults == 0 )
            break;
    }
}
