/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Time.h
*  PURPOSE:
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

namespace SharedUtil
{
    //
    // Retrieves the number of milliseconds that have elapsed since the function was first called (plus a little bit to make it look good).
    // This keeps the counter as low as possible to delay any precision or wrap around issues.
    // Note: Return value is module dependent
    //
    uint        GetTickCount32 ( void );

    // Forbid use of GetTickCount
    #define GetTickCount GetTickCount_has_been_replaced_with_GetTickCount32

    //
    // Retrieves the number of milliseconds that have elapsed since some arbitrary point in time.
    //
    // GetTickCount64() exists on Vista and up and is like GetTickCount() except it returns
    // an __int64 and will effectively never wrap. This is an emulated version for XP and down.
    // Note: Wrap around issue is only defeated if the gap between calls is less than 24 days.
    //
    long long   GetTickCount64_ ( void );

    //
    // Retrieves the number of seconds that have elapsed since some arbitrary point in time.
    //
    double      GetSecondCount ( void );

    //
    // Get the time as a sortable string.
    // Set bDate to include the date, bMs to include milliseconds
    // Return example: "2010-09-01 14:54:31.091"
    SString     GetTimeString ( bool bDate = false, bool bMilliseconds = false, bool bLocal = false );

    // Get the local time as a sortable string.
    SString     GetLocalTimeString ( bool bDate = false, bool bMilliseconds = false );

    // Get time in microseconds
    typedef ulong TIMEUS;
    TIMEUS      GetTimeUs ( void );

    // Get tick count cached per module
    long long   GetModuleTickCount64 ( void );
    void        UpdateModuleTickCount64 ( void );


    //
    // Encapsulate a tick count value
    //
    class CTickCount
    {
        long long m_llTicks;
    public:
        // Constructors
                 CTickCount ( void )                : m_llTicks ( 0 ) {}
        explicit CTickCount ( long long llTicks )   : m_llTicks ( llTicks ) {}
        explicit CTickCount ( double dTicks )       : m_llTicks ( static_cast < long long > ( dTicks ) ) {}

        // Operators
        CTickCount operator+ ( const CTickCount& other ) const  { return CTickCount ( m_llTicks + other.m_llTicks ); }
        CTickCount operator- ( const CTickCount& other ) const  { return CTickCount ( m_llTicks - other.m_llTicks ); }
        CTickCount& operator+= ( const CTickCount& other )      { m_llTicks += other.m_llTicks; return *this; }
        CTickCount& operator-= ( const CTickCount& other )      { m_llTicks -= other.m_llTicks; return *this; }

        // Comparison
        bool operator < ( const CTickCount& other ) const       { return m_llTicks < other.m_llTicks; }
        bool operator > ( const CTickCount& other ) const       { return m_llTicks > other.m_llTicks; }
        bool operator <= ( const CTickCount& other ) const      { return m_llTicks <= other.m_llTicks; }
        bool operator >= ( const CTickCount& other ) const      { return m_llTicks >= other.m_llTicks; }
        bool operator == ( const CTickCount& other ) const      { return m_llTicks == other.m_llTicks; }
        bool operator != ( const CTickCount& other ) const      { return m_llTicks != other.m_llTicks; }

        // Conversion
        double      ToDouble        ( void ) const      { return static_cast < double > ( m_llTicks ); }
        long long   ToLongLong      ( void ) const      { return m_llTicks; }

        // Static functions
        static CTickCount Now ( bool bUseModuleTickCount = false )
        {
            return CTickCount ( bUseModuleTickCount ? GetModuleTickCount64 () : GetTickCount64_ () );
        }
    };


    //
    // Simple class to measure time passing
    // Main feature is the limit on how much time is allowed to pass between each Get()
    //
    class CElapsedTime
    {
        long long   m_llUpdateTime;
        long long   m_llElapsedTime;
        long long   m_llMaxIncrement;
        bool        m_bUseModuleTickCount;
    public:

        CElapsedTime ( long lMaxIncrement = 500, bool bUseModuleTickCount = false )
            : m_llMaxIncrement ( lMaxIncrement )
            , m_bUseModuleTickCount ( bUseModuleTickCount )
        {
            Reset ();
        }

        void Reset ( void )
        {
            m_llUpdateTime = DoGetTickCount ();
            m_llElapsedTime = 0;
        }

        long long Get ( void )
        {
            long long llTime = DoGetTickCount ();
            m_llElapsedTime += Min ( m_llMaxIncrement, llTime - m_llUpdateTime );
            m_llUpdateTime = llTime;
            return m_llElapsedTime;
        }

    protected:
        long long DoGetTickCount ( void )
        {
            return m_bUseModuleTickCount ? GetModuleTickCount64 () : GetTickCount64_ ();
        }
    };


    //
    // Timing sections of code
    //
    template < int RESERVE_NUM_ITEMS = 20 >
    class CTimeUsMarker
    {
    public:
        struct SItem
        {
            const char* szDesc;
            TIMEUS timeUs;
        };

        CTimeUsMarker ( void )
        {
            itemList.reserve ( RESERVE_NUM_ITEMS );
        }

        void Set ( const char* szDesc )
        {
            itemList.push_back ( SItem () );
            SItem& item = itemList.back ();
            item.timeUs = GetTimeUs ();
            item.szDesc = szDesc;
        }

        SString GetString ( void ) const
        {
            SString strStatus;
            for ( uint i = 1 ; i < itemList.size () ; i++ )
            {
                const SItem& itemPrev = itemList[i-1];
                const SItem& item = itemList[i];
                strStatus += SString ( "[%0.2fms %s] ", ( item.timeUs - itemPrev.timeUs ) / 1000.f, item.szDesc ); 
            }
            return strStatus;
        }

     protected:
        std::vector < SItem > itemList;
    };

}
