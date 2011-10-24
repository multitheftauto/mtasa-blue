/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPerfStatModule.h
*  PURPOSE:
*  DEVELOPERS:  Mr OCD
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

//
// Use global struct instead of calls for efficient gathering of certain stats
//
struct SStatData
{
    ZERO_ON_NEW

    struct {
        long long llSentPacketsByZone [ ZONE_MAX ];
        long long llSentBytesByZone [ ZONE_MAX ];
        long long llSkippedPacketsByZone [ ZONE_MAX ];
        long long llSkippedBytesByZone [ ZONE_MAX ];
    } puresync;

    struct {
        long long llSyncPacketsSkipped;
        long long llSyncBytesSkipped;
        long long llLightSyncPacketsSent;
        long long llLightSyncBytesSent;
    } lightsync;

    bool bFunctionTimingActive;
};

extern SStatData* g_pStats;


//
// CPerfStatResult
//
// Result of GetStats
//
class CPerfStatResult
{
    std::vector < SString > colNames;
    std::vector < SString > cellList;
    int iNumColumns;
    int iNumRows;
public:

    CPerfStatResult ()
    {
        iNumColumns = 0;
        iNumRows = 0;
    }

    const SString& ColumnName( unsigned long c ) const
    {
        unsigned long idx = c;
        if ( idx < colNames.size () )
            return colNames[idx];
        static SString dummy;
        return dummy;
    }

    int ColumnCount() const
    {
        return iNumColumns;
    }

    int RowCount() const
    {
        return iNumRows;
    }

    void AddColumn ( const SString& strColumnName )
    {
        colNames.push_back ( strColumnName );
        iNumColumns++;
    }

    SString* AddRow( void )
    {
        iNumRows++;
        cellList.insert( cellList.end (), ColumnCount(), SString() );
        return &cellList[ cellList.size () - ColumnCount() ];
    }

    SString& Data( unsigned long c, unsigned long r )
    {
        unsigned long idx = c + r * ColumnCount();
        if ( idx < cellList.size () )
            return cellList[idx];
        static SString cellDummy;
        return cellDummy;
    }

    void Clear ()
    {
        colNames.clear ();
        cellList.clear ();
        iNumColumns = 0;
        iNumRows = 0;
    }
};


//
// CPerfStatModule
//
class CPerfStatModule
{
public:
    virtual                     ~CPerfStatModule    ( void ) {}

    virtual const SString&      GetCategoryName     ( void ) = 0;
    virtual void                DoPulse             ( void ) = 0;
    virtual void                GetStats            ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter ) = 0;
};


//
// CPerfStatLuaTiming
//
class CPerfStatLuaTiming : public CPerfStatModule
{
public:
    // CPerfStatModule
    virtual const SString&      GetCategoryName     ( void ) = 0;
    virtual void                DoPulse             ( void ) = 0;
    virtual void                GetStats            ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter ) = 0;

    // CPerfStatLuaTiming
    virtual void                OnLuaMainCreate     ( CLuaMain* pLuaMain ) = 0;
    virtual void                OnLuaMainDestroy    ( CLuaMain* pLuaMain ) = 0;
    virtual void                UpdateLuaTiming     ( CLuaMain* pLuaMain, const char* szEventName, TIMEUS timeUs ) = 0;

    static CPerfStatLuaTiming*  GetSingleton        ( void );
};


//
// CPerfStatLuaMemory
//
class CPerfStatLuaMemory : public CPerfStatModule
{
public:
    // CPerfStatModule
    virtual const SString&      GetCategoryName     ( void ) = 0;
    virtual void                DoPulse             ( void ) = 0;
    virtual void                GetStats            ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter ) = 0;

    // CPerfStatLuaMemory
    virtual void                OnLuaMainCreate     ( CLuaMain* pLuaMain ) = 0;
    virtual void                OnLuaMainDestroy    ( CLuaMain* pLuaMain ) = 0;

    static CPerfStatLuaMemory*  GetSingleton        ( void );
};


//
// CPerfStatLibMemory
//
class CPerfStatLibMemory : public CPerfStatModule
{
public:
    // CPerfStatModule
    virtual const SString&      GetCategoryName     ( void ) = 0;
    virtual void                DoPulse             ( void ) = 0;
    virtual void                GetStats            ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter ) = 0;

    // CPerfStatLibMemory

    static CPerfStatLibMemory*  GetSingleton        ( void );
};


//
// CPerfStatPacketUsage
//
class CPerfStatPacketUsage : public CPerfStatModule
{
public:
    // CPerfStatModule
    virtual const SString&      GetCategoryName     ( void ) = 0;
    virtual void                DoPulse             ( void ) = 0;
    virtual void                GetStats            ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter ) = 0;

    // CPerfStatPacketUsage

    static CPerfStatPacketUsage*  GetSingleton      ( void );
};


//
// CPerfStatBandwidthUsage
//
class CPerfStatBandwidthUsage : public CPerfStatModule
{
public:
    // CPerfStatModule
    virtual const SString&      GetCategoryName     ( void ) = 0;
    virtual void                DoPulse             ( void ) = 0;
    virtual void                GetStats            ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter ) = 0;

    // CPerfStatBandwidthUsage

    static CPerfStatBandwidthUsage*  GetSingleton      ( void );
};


//
// CPerfStatSqliteTiming
//
class CPerfStatSqliteTiming : public CPerfStatModule
{
public:
    // CPerfStatModule
    virtual const SString&      GetCategoryName     ( void ) = 0;
    virtual void                DoPulse             ( void ) = 0;
    virtual void                GetStats            ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter ) = 0;

    // CPerfStatSqliteTiming
    virtual void                OnSqliteOpen        ( CRegistry* pRegistry, const SString& strFileName ) = 0;
    virtual void                OnSqliteClose       ( CRegistry* pRegistry ) = 0;
    virtual void                UpdateSqliteTiming  ( CRegistry* pRegistry, const char* szQuery, TIMEUS timeUs ) = 0;
    virtual void                SetCurrentResource  ( lua_State* luaVM ) = 0;

    static CPerfStatSqliteTiming*  GetSingleton        ( void );
};


//
// CPerfStatBandwidthReduction
//
class CPerfStatBandwidthReduction : public CPerfStatModule
{
public:
    // CPerfStatModule
    virtual const SString&      GetCategoryName     ( void ) = 0;
    virtual void                DoPulse             ( void ) = 0;
    virtual void                GetStats            ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter ) = 0;

    // CPerfStatBandwidthReduction

    static CPerfStatBandwidthReduction*  GetSingleton      ( void );
};


//
// CPerfStatServerInfo
//
class CPerfStatServerInfo : public CPerfStatModule
{
public:
    // CPerfStatModule
    virtual const SString&      GetCategoryName     ( void ) = 0;
    virtual void                DoPulse             ( void ) = 0;
    virtual void                GetStats            ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter ) = 0;

    // CPerfStatServerInfo

    static CPerfStatServerInfo*  GetSingleton      ( void );
};


//
// CPerfStatServerTiming
//
class CPerfStatServerTiming : public CPerfStatModule
{
public:
    // CPerfStatModule
    virtual const SString&      GetCategoryName     ( void ) = 0;
    virtual void                DoPulse             ( void ) = 0;
    virtual void                GetStats            ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter ) = 0;

    static CPerfStatServerTiming*  GetSingleton        ( void );
};


//
// CPerfStatFunctionTiming
//
class CPerfStatFunctionTiming : public CPerfStatModule
{
public:
    // CPerfStatModule
    virtual const SString&      GetCategoryName     ( void ) = 0;
    virtual void                DoPulse             ( void ) = 0;
    virtual void                GetStats            ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter ) = 0;

    // CPerfStatFunctionTiming
    virtual void                UpdateTiming        ( const char* szFunctionName, TIMEUS timeUs ) = 0;

    static CPerfStatFunctionTiming*  GetSingleton        ( void );
};
