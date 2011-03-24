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

//
// WITH_ALLOC_TRACKING
//
// Various unhealthy hacks to minimize source changes.
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
