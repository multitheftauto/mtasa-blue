/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.h
*  PURPOSE:
*  DEVELOPERS:  ccw <chris@codewave.co.uk>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

//
// System wide defines
//
#define WITH_VEHICLE_HANDLING 0

// Enable WITH_ALLOC_TRACKING to monitor module memory usage. *Has a negative performance impact*
#define WITH_ALLOC_TRACKING 0

//
// _vsnprintf with buffer full check
//
#define _VSNPRINTF( buffer, count, format, argptr ) \
    { \
        int iResult = _vsnprintf ( buffer, count, format, argptr ); \
        if( iResult == -1 || iResult == (count) ) \
            (buffer)[(count)-1] = 0; \
    }

//
// strncpy with null termination
//
#ifndef STRNCPY
    #define STRNCPY( dest, source, count ) \
        { \
            strncpy( dest, source, (count)-1 ); \
            (dest)[(count)-1] = 0; \
        }
#endif

#ifndef WIN32
    #define _vsnprintf vsnprintf
    #define _isnan isnan
#endif

//
// Macro for counting the number of elements in a static array
//
#ifndef NUMELMS     // from DShow.h
    #define NUMELMS(aa) (sizeof(aa)/sizeof((aa)[0]))
#endif


//
//
// WITH_ALLOC_TRACKING
// Don't look!
//
#if WITH_ALLOC_TRACKING

    #include <cstdlib>
    #include <exception>
    #include <new>
    #ifdef WIN32
        #include <xutility>
    #endif

    void*   myMalloc    ( size_t Count, const char* Tag = NULL, int ElementSize=-1 );
    void*   myCalloc    ( size_t Count, size_t elsize, const char* Tag = NULL, int ElementSize=-1 );
    void*   myRealloc   ( void* Original, size_t Count, const char* Tag = NULL, int ElementSize=-1 );
    void    myFree      ( void* Original );
    void    myDelete    ( void* Original );
    void*   myNew       ( std::size_t size, const char* Tag = NULL, int ElementSize = -1 );

    #ifdef WIN32
        #define STD_BAD_ALLOC
    #else
        #define STD_BAD_ALLOC std::bad_alloc
    #endif
    inline void* operator new(std::size_t size, int, const char* Tag, int ElementSize)                          { return myNew( size, Tag, ElementSize );  }
    inline void* operator new(std::size_t size, int, const char* Tag)                                           { return myNew( size, Tag );  }
    inline void* operator new(std::size_t size) throw (STD_BAD_ALLOC)                                           { return myNew( size );  }
    inline void* operator new[](std::size_t size, int, const char* Tag, int ElementSize) throw (STD_BAD_ALLOC)  { return myNew( size, Tag, ElementSize );  }
    inline void* operator new[](std::size_t size, int, const char* Tag) throw (STD_BAD_ALLOC)                   { return myNew( size, Tag );  }
    inline void* operator new[](std::size_t size) throw (STD_BAD_ALLOC)                                         { return myNew( size );  }
    inline void operator delete(void* ptr) throw()                                                              { myDelete(ptr); }
    inline void operator delete[](void* ptr) throw()                                                            { myDelete(ptr); }

    inline void* operator new(std::size_t size, const std::nothrow_t&) throw()                                  { return myNew( size );  }
    inline void* operator new[](std::size_t size, const std::nothrow_t&) throw()                                { return myNew( size );  }
    inline void operator delete(void* ptr, const std::nothrow_t&) throw()                                       { myDelete(ptr); }

    inline void operator delete[](void* ptr, const std::nothrow_t&) throw()                                     { myDelete(ptr); }

    #ifdef WIN32
        inline void* _malloc_   ( size_t Count )                        { return myMalloc( Count );  }
        inline void* _calloc_   ( size_t Count, size_t elsize )         { return myCalloc( Count, elsize );  }
        inline void* _realloc_  ( void* Original, size_t Count )        { return myRealloc( Original, Count );  }
        inline void  _free_     ( void* Original )                      { return myFree( Original );  }

        #define malloc _malloc_
        #define calloc _calloc_
        #define realloc _realloc_
        #define free _free_
    #endif

    #include <list>
    #include <vector>
    #include <map>
    #include <deque>
    #include <algorithm>
    #include <limits>
    #include <assert.h>

    namespace std
    {
    #undef max
    #undef min

        template<typename T>
        class Allocator {
        public : 
            //    typedefs

            typedef T value_type;
            typedef value_type* pointer;
            typedef const value_type* const_pointer;
            typedef value_type& reference;
            typedef const value_type& const_reference;
            typedef std::size_t size_type;
            typedef std::ptrdiff_t difference_type;

        public : 
            //    convert an allocator<T> to allocator<U>

            template<typename U>
            struct rebind {
                typedef Allocator<U> other;
            };

        public : 
            inline explicit Allocator() {}
            inline ~Allocator() {}
            inline explicit Allocator(Allocator const&) {}
            template<typename U>
            inline Allocator(const Allocator<U>& ) {}

            //    address

            inline pointer address(reference r) { return &r; }
            inline const_pointer address(const_reference r) { return &r; }

            //    memory allocation

            inline pointer allocate(size_type cnt, 
               typename std::allocator<void>::const_pointer = 0) { 
              return reinterpret_cast<pointer>(::operator new(cnt * sizeof (T),1,"allocater allocate",sizeof (T))); 
            }
            inline void deallocate(pointer p, size_type) { 
                ::operator delete(p); 
            }

            //    size

            inline size_type max_size() const { 
                return std::numeric_limits<size_type>::max() / sizeof(T);
         }

            //    construction/destruction

            inline void construct(pointer p, const T& t) {
                new(p) T(t);
            }
            inline void destroy(pointer p) { p->~T(); }

            inline bool operator==(Allocator const&) { return true; }
            inline bool operator!=(Allocator const& a) { return !operator==(a); }
        };    //    end of class Allocator 


        template < class _Ty >
        class CArray : public std::vector < _Ty, Allocator < _Ty > >
        {
        public:
            CArray ( void ) : std::vector < _Ty, Allocator < _Ty > > () {}
            CArray ( int size ) : std::vector < _Ty, Allocator < _Ty > > ( size ) {}
            CArray ( int size, int fill ) : std::vector < _Ty, Allocator < _Ty > > ( size, fill ) {}
        };

        template < class _Ty >
        class CList : public std::list < _Ty, Allocator < _Ty > >
        {
        public:
        };

        template < class _Kty, class _Ty, class _Pr = std::less<_Kty> >
        class CMap : public std::map < _Kty, _Ty, _Pr, Allocator < std::pair<const _Kty, _Ty> > >
        {
        public:
        };

        template < class _Ty >
        class CDeque : public std::deque < _Ty, Allocator < _Ty > >
        {
        public:
        };
    }

    // Replace std classes
    // Not safe - do not enable this in distributed builds
    #define vector  CArray
    #define list    CList
    #define map     CMap
    #define deque   CDeque

#else

    #define myMalloc(a,tag)     malloc(a)
    #define myCalloc(a,tag)     calloc(a)
    #define myRealloc(a,b,tag)  realloc(a,b)
    #define myFree              free

#endif  // WITH_ALLOC_TRACKING


#include <list>
#include <vector>
#include <map>
#include <deque>
#include <algorithm>
#include <limits>
#include <assert.h>
#include "SString.h"

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

    //
    // For tracking results of new features
    //
    void AddReportLog ( const SString& strText );

#endif

    //
    // Retrieves the number of milliseconds that have elapsed since the system was started.
    //
    // GetTickCount64() exists on Vista and up and is like GetTickCount() except it returns
    // an __int64 and will effectively never wrap. This is an emulated version for XP and down.
    // Note: Wrap around issue is only defeated if the gap between calls is less than 24 days.
    //
    long long GetTickCount64_ ( void );

    //
    // Retrieves the number of seconds that have elapsed since the system was started.
    //
    double GetSecondCount ( void );

    //
    // Get the local time in a string.
    // Set bDate to include the date, bMs to include milliseconds
    //
    SString GetLocalTimeString ( bool bDate = false, bool bMilliseconds = false );

    //
    // Output timestamped line into the debugger
    //
    void OutputDebugLine ( const char* szMessage );

    //
    // Returns true if the file exists
    //
    bool FileExists ( const SString& strFilename );

    //
    // Load binary data from a file into an array
    //
    bool FileLoad ( const SString& strFilename, std::vector < char >& buffer );

    //
    // Save binary data to a file
    //
    bool FileSave ( const SString& strFilename, void* pBuffer, unsigned long ulSize );

    //
    // Append binary data to a file
    //
    bool FileAppend ( const SString& strFilename, void* pBuffer, unsigned long ulSize );

    //
    // Ensure all directories exist to the file
    //
    void MakeSureDirExists ( const char* szPath );

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
    };


};

using namespace SharedUtil;

