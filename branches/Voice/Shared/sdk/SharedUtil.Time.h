/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.AllocTracking.h
*  PURPOSE:
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

namespace SharedUtil
{
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


    //
    // Simple class to measure time passing
    // Main feature is the limit on how much time is allowed to pass between each Get()
    //
    class CElapsedTime
    {
        long long   m_llUpdateTime;
        long        m_lElapsedTime;
        long        m_lMaxIncrement;
    public:

        CElapsedTime ( long lMaxIncrement = 500 )
            : m_lMaxIncrement ( lMaxIncrement )
        {
            Reset ();
        }

        void Reset ( void )
        {
            m_llUpdateTime = GetTickCount64_ ();
            m_lElapsedTime = 0;
        }

        long Get ( void )
        {
            long long llTime = GetTickCount64_ ();
            m_lElapsedTime += Min ( m_lMaxIncrement, static_cast < long > ( llTime - m_llUpdateTime ) );
            m_llUpdateTime = llTime;
            return m_lElapsedTime;
        }
    };

}
