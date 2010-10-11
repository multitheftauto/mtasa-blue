/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Misc.h
*  PURPOSE:
*  DEVELOPERS:  ccw <chris@codewave.co.uk>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


namespace SharedUtil
{
    class CArgMap;
#ifdef WIN32

    //
    // Get startup directory as saved in the registry by the launcher
    // Used in the Win32 Client only
    //
    SString GetMTASABaseDir();

    //
    // Turns a relative MTASA path i.e. "MTA\file.dat"
    // into an absolute MTASA path i.e. "C:\Program Files\MTA San Andreas\MTA\file.dat"
    //
    SString CalcMTASAPath ( const SString& strPath );

    //
    // Run ShellExecute with these parameters after exit
    //
    void SetOnQuitCommand ( const SString& strOperation, const SString& strFile = "", const SString& strParameters = "", const SString& strDirectory = "", const SString& strShowCmd = "" );

    //
    // What to do on next restart
    //
    void SetOnRestartCommand ( const SString& strOperation, const SString& strFile = "", const SString& strParameters = "", const SString& strDirectory = "", const SString& strShowCmd = "" );
    bool GetOnRestartCommand ( SString& strOperation, SString& strFile, SString& strParameters, SString& strDirectory, SString& strShowCmd );

    //
    // For tracking results of new features
    //
    void            AddReportLog                    ( uint uiId, const SString& strText );
    void            SetReportLogContents            ( const SString& strText );
    SString         GetReportLogContents            ( void );

    void            SetApplicationSetting           ( const SString& strPath, const SString& strKey, const SString& strValue );
    void            SetApplicationSetting           ( const SString& strPathKey, const SString& strValue );
    void            SetApplicationSettingInt        ( const SString& strPath, const SString& strKey, int iValue );
    void            SetApplicationSettingInt        ( const SString& strPathKey, int iValue );
    SString         GetApplicationSetting           ( const SString& strPath, const SString& strKey );
    SString         GetApplicationSetting           ( const SString& strPathKey );
    int             GetApplicationSettingInt        ( const SString& strPath, const SString& strKey );
    int             GetApplicationSettingInt        ( const SString& strPathKey );

    void            WatchDogReset                   ( void );
    bool            WatchDogIsSectionOpen           ( const SString& str );
    void            WatchDogIncCounter              ( const SString& str );
    int             WatchDogGetCounter              ( const SString& str );
    void            WatchDogClearCounter            ( const SString& str );
    void            WatchDogBeginSection            ( const SString& str );
    void            WatchDogCompletedSection        ( const SString& str );
    bool            WatchDogWasUncleanStop          ( void );
    void            WatchDogSetUncleanStop          ( bool bOn );

    void            BrowseToSolution                ( const SString& strType, bool bAskQuestion = false, bool bTerminateProcess = false );
#endif

    SString         EscapeString                    ( const SString& strText, const SString& strDisallowedChars, char cSpecialChar = '#' );
    SString         UnescapeString                  ( const SString& strText, char cSpecialChar = '#' );

    SString         ExpandEnvString                 ( const SString& strInput );

    //
    // Output timestamped line into the debugger
    //
    #ifdef MTA_DEBUG
        void OutputDebugLine ( const char* szMessage );
    #else
        #define OutputDebugLine(x) {}
    #endif

    //
    // Return true if supplied string adheres to the new version format
    //
    bool IsValidVersionString ( const SString& strVersion );

    //
    // Try to make a path relative to the 'resources/' directory
    //
    SString ConformResourcePath ( const char* szRes );

    //
    // Some templates
    //
    template < class T >
    T Min ( const T& a, const T& b )
    {
        return a < b ? a : b;
    }

    template < class T >
    T Max ( const T& a, const T& b )
    {
        return a > b ? a : b;
    }

    // Clamps a value between two other values ( min < a < max )
    template < class T >
    T Clamp ( const T& min, const T& a, const T& max )
    {
        return a < min ? min : a > max ? max : a;
    }

    // Lerps between two values depending on the weight
    template< class T >
    T Lerp ( const T& from, float fAlpha, const T& to )
    {
        return ( to - from ) * fAlpha + from;
    }

    // Find the relative position of Pos between From and To
    inline const float Unlerp ( const double dFrom, const double dPos, const double dTo )
    {
        // Avoid dividing by 0 (results in INF values)
        if ( dFrom == dTo ) return 1.0f;

        return static_cast < float > ( ( dPos - dFrom ) / ( dTo - dFrom ) );
    }

    // Unlerp avoiding extrapolation
    inline const float UnlerpClamped ( const double dFrom, const double dPos, const double dTo )
    {
        return Clamp ( 0.0f, Unlerp( dFrom, dPos, dTo ), 1.0f );
    }

    template < class T >
    int Round ( T value )
    {
        return static_cast < int > ( floor ( value + 0.5f ) );
    }



    //
    // Container helpers for std::list/vector/map
    //

    //
    // std::list helpers
    //

    // Returns true if the item is in the itemList
    template < class T >
    bool ListContains ( const std::list < T >& itemList, const T& item )
    {
        typename std::list < T > ::const_iterator it = itemList.begin ();
        for ( ; it != itemList.end () ; ++it )
            if ( item == *it )
                return true;
        return false;
    }


    //
    // std::vector helpers
    //

    // Returns true if the item is in the itemList
    template < class T >
    bool ListContains ( const std::vector < T >& itemList, const T& item )
    {
        typename std::vector < T > ::const_iterator it = itemList.begin ();
        for ( ; it != itemList.end () ; ++it )
            if ( item == *it )
                return true;
        return false;
    }

    // Remove first occurrence of item from itemList
    template < class T >
    void ListRemove ( std::vector < T >& itemList, const T& item )
    {
        typename std::vector < T > ::iterator it = itemList.begin ();
        for ( ; it != itemList.end () ; ++it )
            if ( item == *it )
            {
                itemList.erase ( it );
                break;
            }
    }

    // Remove item at index from itemList
    template < class T >
    void ListRemoveIndex ( std::vector < T >& itemList, uint index )
    {
        if ( index >=0 && index < itemList.size () )
            itemList.erase ( itemList.begin () + index );
    }

    // Append one list onto another
    template < class T >
    void ListAppend ( std::vector < T >& itemList, const std::vector < T >& other )
    {
        itemList.insert( itemList.end(), other.begin(), other.end() );
    }



    //
    // std::map helpers
    //

    // Update or add a value for a key
    template < class T, class V, class TR, class T2 >
    void MapSet ( std::map < T, V, TR >& collection, const T2& key, const V& value )
    {
        collection[ key ] = value;
    }

    // Returns true if the item is in the collection
    template < class T, class V, class TR, class T2 >
    bool MapContains ( const std::map < T, V, TR >& collection, const T2& key )
    {
        return collection.find ( key ) != collection.end ();
    }

    // Remove key from collection
    template < class T, class V, class TR, class T2 >
    void MapRemove ( std::map < T, V, TR >& collection, const T2& key )
    {
        typename std::map < T, V, TR > ::iterator it = collection.find ( key );
        if ( it != collection.end () )
            collection.erase ( it );
    }

    // Find value in collection
    template < class T, class V, class TR, class T2 >
    V* MapFind ( std::map < T, V, TR >& collection, const T2& key )
    {
        typename std::map < T, V, TR > ::iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return &it->second;
    }

    // Find value in const collection
    template < class T, class V, class TR, class T2 >
    const V* MapFind ( const std::map < T, V, TR >& collection, const T2& key )
    {
        typename std::map < T, V, TR > ::const_iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return &it->second;
    }


    //
    // std::multimap helpers
    //

    // Find values in const collection
    template < class T, class V, class TR, class T2 >
    void MultiFind ( const std::multimap < T, V, TR >& collection, const T2& key, std::vector < V >* pResult )
    {
        typedef typename std::multimap < T, V, TR > ::const_iterator const_iter_t;
        std::pair < const_iter_t, const_iter_t > itp = collection.equal_range ( key );
        for ( const_iter_t it = itp.first ; it != itp.second ; ++it )
            pResult->push_back ( it->second );
    }

    // Find first value in const collection
    template < class T, class V, class TR, class T2 >
    const V* MapFind ( const std::multimap < T, V, TR >& collection, const T2& key )
    {
        typename std::multimap < T, V, TR > ::const_iterator it = collection.find ( key );
        if ( it == collection.end () )
            return NULL;
        return &it->second;
    }

    // Add a value for a key
    template < class T, class V, class TR, class T2 >
    void MapInsert ( std::multimap < T, V, TR >& collection, const T2& key, const V& value )
    {
        collection.insert ( std::pair < T2, V > ( key, value ) );
    }




    //
    // SColor
    //
    // Encapsulates the most common usage of 4 byte color storage.
    // Casts to and from a DWORD as 0xAARRGGBB
    //
    class SColor
    {
        // No shifting allowed to access the color channel information
        void operator >> ( int ) const;
        void operator << ( int ) const;
        void operator >>= ( int );
        void operator <<= ( int );
    public:
        union
        {
            struct
            {
                unsigned char B, G, R, A;
            };
            unsigned long ulARGB;
        };

        SColor () {}
        SColor ( unsigned long ulValue )
        {
            ulARGB = ulValue;
        }

        operator unsigned long () const
        {
            return ulARGB;
        }
    };


    //
    // SColorARGB
    //
    // Make an SColor from A,R,G,B
    //
    class SColorARGB : public SColor
    {
    public:
        SColorARGB ( unsigned char ucA, unsigned char ucR, unsigned char ucG, unsigned char ucB )
        {
            A = ucA; R = ucR; G = ucG; B = ucB;
        }
    };


    //
    // SColorRGBA
    //
    // Make an SColor from R,G,B,A
    //
    class SColorRGBA : public SColor
    {
    public:
        SColorRGBA ( unsigned char ucR, unsigned char ucG, unsigned char ucB, unsigned char ucA )
        {
            A = ucA; R = ucR; G = ucG; B = ucB;
        }
    };


    //
    // Things to make it simpler to use SColor with the source code as it stands
    //
    typedef SColor RGBA;

    inline unsigned char COLOR_RGBA_R ( SColor color ) { return color.R; }
    inline unsigned char COLOR_RGBA_G ( SColor color ) { return color.G; }
    inline unsigned char COLOR_RGBA_B ( SColor color ) { return color.B; }
    inline unsigned char COLOR_RGBA_A ( SColor color ) { return color.A; }
    inline unsigned char COLOR_ARGB_A ( SColor color ) { return color.A; }

    inline SColor COLOR_RGBA ( unsigned char R, unsigned char G, unsigned char B, unsigned char A ) { return SColorRGBA ( R, G, B, A ); }
    inline SColor COLOR_ARGB ( unsigned char A, unsigned char R, unsigned char G, unsigned char B ) { return SColorRGBA ( R, G, B, A ); }
    inline SColor COLOR_ABGR ( unsigned char A, unsigned char B, unsigned char G, unsigned char R ) { return SColorRGBA ( R, G, B, A ); }


    //
    // Cross platform critical section
    //
    class CCriticalSection
    {
    public:
                    CCriticalSection        ( void );
                    ~CCriticalSection       ( void );
        void        Lock                    ( void );
        void        Unlock                  ( void );

    private:
        void*       m_pCriticalSection;
    };


    //
    // Expiry stuff
    //
    int GetBuildAge ( void );
    int GetDaysUntilExpire ( void );

    //
    // string stuff
    //
    std::string RemoveColorCode ( const char* szString );


    //
    // ID 'stack'
    //
    // Note: IDs run from 1 to Capacity
    //
    template < typename T, unsigned long INITIAL_MAX_STACK_SIZE, T INVALID_STACK_ID >
    class CStack
    {
    public:
        CStack ( void )
        {
            m_ulCapacity = 0;
            ExpandBy ( INITIAL_MAX_STACK_SIZE - 1 );
        }

        unsigned long GetCapacity ( void ) const
        {
            return m_ulCapacity;
        }

        unsigned long GetUnusedAmount ( void ) const
        {
            return m_Queue.size ();
        }

        void ExpandBy ( unsigned long ulAmount )
        {
            const unsigned long ulOldSize = m_ulCapacity;
            const unsigned long ulNewSize = m_ulCapacity + ulAmount;

            // Add ID's for new items
            for ( T ID = ulOldSize + 1; ID <= ulNewSize; ++ID )
            {
                m_Queue.push_front( ID );
            }
            m_ulCapacity = ulNewSize;
        }

        T Pop ( void )
        {
            // Got any items? Pop from the back
            if ( m_Queue.size () > 0 )
            {
                T ID = m_Queue.back();
                m_Queue.pop_back ();
                return ID;
            }

            // No IDs left
            return INVALID_STACK_ID;
        }

        void Push ( T ID )
        {
            assert ( m_Queue.size () < m_ulCapacity );
            assert ( ID != INVALID_STACK_ID );
            // Push to the front
            return m_Queue.push_front ( ID );
        }

    private:
        unsigned long       m_ulCapacity;
        std::deque < T >    m_Queue;
    };


    //
    // Fixed sized string buffer
    //
    template < int MAX_LENGTH >
    class CStaticString
    {
        char szData [ MAX_LENGTH + 1 ];
    public:
        CStaticString ( void )
        {
            szData[0] = 0;
        }

        // In  
        CStaticString& operator= ( const char* szOther )
        {
            STRNCPY( szData, szOther, MAX_LENGTH + 1 );
            return *this;
        }

        // Out  
        operator const char*() const
        {
            return szData;
        }

        // Shake it all about
        void Encrypt ();
    };


    ///////////////////////////////////////////////////////////////
    //
    // CArgMap
    //
    // For manipulation and packing of key/values <=> string
    // "key1=value1&key2=value2"
    //
    ///////////////////////////////////////////////////////////////
    class CArgMap
    {
        std::multimap < SString, SString >  m_Map;
        SString                             m_strArgSep;
        SString                             m_strPartsSep;
        SString                             m_strDisallowedChars;
    public:
                    CArgMap             ( const SString& strArgSep = "=", const SString& strPartsSep = "&", const SString& strExtraDisallowedChars = "" );
        void        Merge               ( const CArgMap& other, bool bAllowMultiValues = false );
        void        SetFromString       ( const SString& strLine, bool bAllowMultiValues = false );
        void        MergeFromString     ( const SString& strLine, bool bAllowMultiValues = false );
        SString     ToString            ( void ) const;
        bool        HasMultiValues      ( void ) const;
        void        RemoveMultiValues   ( void );
        SString     Escape              ( const SString& strIn ) const;
        SString     Unescape            ( const SString& strIn ) const;
        void        Set                 ( const SString& strInCmd, const SString& strInValue );                             // Set a unique key string value
        void        Set                 ( const SString& strInCmd, int iValue );                                            // Set a unique key int value
        void        Insert              ( const SString& strInCmd, int iValue );                                            // Insert a key int value
        void        Insert              ( const SString& strInCmd, const SString& strInValue );                             // Insert a key string value
        bool        Contains            ( const SString& strInCmd  ) const;                                                 // Test if key exists
        bool        Get                 ( const SString& strInCmd, SString& strOut, const char* szDefault = "" ) const;     // First result as string
        SString     Get                 ( const SString& strInCmd ) const;                                                  // First result as string
        bool        Get                 ( const SString& strInCmd, std::vector < SString >& outList ) const;                // All results as strings
        bool        Get                 ( const SString& strInCmd, int& iValue, int iDefault = 0 ) const;                   // First result as int
    };


};

using namespace SharedUtil;

