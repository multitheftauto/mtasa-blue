/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.ClassIdent.h
*  PURPOSE:
*  DEVELOPERS:  (\_/)
*               (^_^)
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

namespace SharedUtil
{

    // Macros to implement quick class identification

    // uint64 allows for a total of 64 classes
    typedef uint64 ClassBits;
    typedef uchar ClassId;


    #define DECLARE_BASE_CLASS(cls) \
        public: \
            static ClassId GetClassId ( void ) \
            { \
                return CLASS_##cls; \
            } \
            bool IsA( ClassId classId ) const \
            { \
                return ( ClassHierarchyBits & ( 1ULL << classId ) ) ? true : false; \
            } \
            const char* GetClassName ( void ) \
            { \
                return ClassName; \
            } \
        protected: \
            static const char* StaticGetClassName ( void ) \
            { \
                return #cls; \
            } \
            static ClassBits CalcClassHierarchyBits ( void ) \
            { \
                return ( 1ULL << GetClassId () ); \
            } \
            const char* ClassName; \
            ClassBits ClassHierarchyBits; \
            friend CAutoClassInit < cls >; \
            CAutoClassInit < cls > ClassInit; \
        public: \
            void* operator new ( size_t size )              { void* ptr = ::operator new(size); memset(ptr,0,size); return ptr; } \
            void* operator new ( size_t size, void* where ) { memset(where,0,size); return where; }



    #define DECLARE_CLASS(cls,super) \
        public: \
            static ClassId GetClassId ( void ) \
            { \
                return CLASS_##cls; \
            } \
        protected: \
            static const char* StaticGetClassName ( void ) \
            { \
                return #cls; \
            } \
            static ClassBits CalcClassHierarchyBits ( void ) \
            { \
                return ( 1ULL << GetClassId () ) | super::CalcClassHierarchyBits (); \
            } \
            friend CAutoClassInit < cls >; \
            CAutoClassInit < cls > ClassInit; \
        public: \
            typedef super Super; \
            void* operator new ( size_t size )              { void* ptr = ::operator new(size); memset(ptr,0,size); return ptr; } \
            void* operator new ( size_t size, void* where ) { memset(where,0,size); return where; }


    //
    // Auto init the class bit flags
    //
    template < class T >
    class CAutoClassInit
    {
    public:
        CAutoClassInit ( T* ptr )
        {
            assert ( ptr->GetClassId () < sizeof ( ClassBits ) * 8 );
            ptr->ClassHierarchyBits = ptr->CalcClassHierarchyBits();
            ptr->ClassName = ptr->StaticGetClassName();
        }
    };


    //
    // Dynamic cast to derived class
    //
    template < class T, class U >
    T* DynamicCast ( U* ptr )
    {
        if ( ptr && ptr->IsA( T::GetClassId () ) )
            return static_cast < T* >( ptr );
        return NULL;
    }


    #ifdef WIN32
        #pragma warning( disable : 4355 )   // warning C4355: 'this' : used in base member initializer list
    #endif
}
