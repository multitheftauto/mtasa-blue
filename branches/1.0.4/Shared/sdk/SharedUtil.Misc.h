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
    void AddReportLog ( uint uiId, const SString& strText );
    void SetReportLogContents ( const SString& strText );
    SString GetReportLogContents ( void );

#endif


    //
    // Output timestamped line into the debugger
    //
    void OutputDebugLine ( const char* szMessage );

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
    void MultiFind ( const std::multimap < T, V, TR >& collection, T2& key, std::vector < V >* pResult )
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
        collection.insert ( std::pair < T2, T > ( key, value ) );
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
        std::multimap < SString, SString > m_Map;
        SString m_strArgSep;
        SString m_strPartsSep;
    public:
        CArgMap ( const SString& strArgSep, const SString& strPartsSep )
            : m_strArgSep ( strArgSep )
            , m_strPartsSep ( strPartsSep )
        {}

        void Merge ( const CArgMap& other )
        {
            MergeFromString ( other.ToString () );
        }

        void SetFromString ( const SString& strLine )
        {
            m_Map.clear ();
            MergeFromString ( strLine );
        }

        void MergeFromString ( const SString& strLine )
        {
            std::vector < SString > parts;
            strLine.Split( m_strPartsSep, parts );
            for ( uint i = 0 ; i < parts.size () ; i++ )
            {
                SString strCmd, strArg;
                parts[i].Split ( m_strArgSep, &strCmd, &strArg );
                Set ( strCmd, strArg );
            }
        }

        SString ToString ( void ) const
        {
            SString strResult;
            for ( std::multimap < SString, SString >::const_iterator iter = m_Map.begin () ; iter != m_Map.end () ; ++iter )
            {
                if ( strResult.length () )
                    strResult += m_strPartsSep;
                strResult += iter->first + m_strArgSep + iter->second;
            }
            return strResult;
        }

        // Set a unique key string value
        void Set ( const SString& strCmd, const SString& strValue )
        {
            m_Map.erase ( strCmd );
            Insert ( strCmd, strValue );
        }

        // Set a unique key int value
        void Set ( const SString& strCmd, int iValue )
        {
            m_Map.erase ( strCmd );
            Insert ( strCmd, iValue );
        }

        // Insert a key int value
        void Insert ( const SString& strCmd, int iValue )
        {
            Insert ( strCmd, SString ( "%d", iValue ) );
        }

        // Insert a key string value
        void Insert ( const SString& strCmd, const SString& strValue )
        {
            if ( strCmd.length () ) // Key can not be empty
                MapInsert ( m_Map, strCmd, strValue );
        }


        // Test if key exists
        bool Contains ( const SString& strCmd  ) const
        {
            return MapFind ( m_Map, strCmd ) != NULL;
        }

        // First result as string
        bool Get ( const SString& strCmd, SString& strOut, const char* szDefault = "" ) const
        {
            assert ( szDefault );
            if ( const SString* pResult = MapFind ( m_Map, strCmd ) )
            {
                strOut = *pResult;
                return true;
            }
            strOut = szDefault;
            return false;
        }

        // First result as string
        SString Get ( const SString& strCmd ) const
        {
            SString strResult;
            Get ( strCmd, strResult );
            return strResult;
        }

        // All results as strings
        bool Get ( const SString& strCmd, std::vector < SString >& outList ) const
        {
            std::vector < SString > newItems;
            MultiFind ( m_Map, strCmd, &newItems );
            ListAppend ( outList, newItems );
            return newItems.size () > 0;
        }

        // First result as int
        bool Get ( const SString& strCmd, int& iValue, int iDefault = 0 ) const
        {
            if ( const SString* pResult = MapFind ( m_Map, strCmd ) )
            {
                iValue = atoi ( *pResult );
                return true;
            }
            iValue = iDefault;
            return false;
        }
    };

};

using namespace SharedUtil;

