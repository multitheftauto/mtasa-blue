/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

namespace
{
    typedef void *(*PFN_ThreadStart) (void *);

    // Wrap pthread
    class CThreadHandle
    {
        int res;
        pthread_t handle;
    public:
        CThreadHandle ( PFN_ThreadStart threadStart, void *arg )
        {
            res = pthread_create ( &handle, NULL, threadStart, arg );
        }

        static void AllowASyncCancel ( void )
        {
            pthread_setcancelstate ( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );
        }

        bool IsValid ( void ) const
        {
            return res == 0;
        }

        void Cancel ( void )
        {
            pthread_cancel ( handle );
        }
    };


    //
    // Mutex with wait
    //
    class CComboMutex
    {
        pthread_mutex_t     mutex;
        pthread_cond_t      cond;
    public:
        CComboMutex ( void )
        {
            pthread_mutex_init ( &mutex, NULL );
            pthread_cond_init ( &cond, NULL );
        }

        ~CComboMutex ( void )
        {
            pthread_mutex_destroy ( &mutex );
            pthread_cond_destroy ( &cond );
        }

        void Lock ( void )
        {
            pthread_mutex_lock ( &mutex );
        }

        void Unlock ( void )
        {
            pthread_mutex_unlock ( &mutex );
        }

        // unlock - wait for signal - lock
        // Returns ETIMEDOUT if timeout period expired
        int Wait ( uint uiTimeout )
        {
            // Handle the different timeout requirements
            if ( uiTimeout == 0 )
                return 0;
            if ( uiTimeout == (uint)-1 )
                pthread_cond_wait ( &cond, &mutex );
            else
            {
                // Get time now
                struct timeval tv;
#ifdef WIN32
                _timeb timeb;
                _ftime(&timeb);
                tv.tv_sec = static_cast < ulong > ( timeb.time );
                tv.tv_usec = timeb.millitm * 1000;
#else
                gettimeofday ( &tv , NULL );
#endif
                // Add the timeout length
                tv.tv_sec += uiTimeout / 1000;
                tv.tv_usec += ( uiTimeout % 1000 ) * 1000;
                // Carry over seconds
                tv.tv_sec += tv.tv_usec / 1000000;
                tv.tv_usec %= 1000000;
                // Convert to timespec
                timespec t;
                t.tv_sec = tv.tv_sec;
                t.tv_nsec = tv.tv_usec * 1000;
                return pthread_cond_timedwait ( &cond, &mutex, &t );
            }
            return 0;
        }

        void Signal ( void )
        {
            pthread_cond_signal ( &cond );
        }
    };
}
