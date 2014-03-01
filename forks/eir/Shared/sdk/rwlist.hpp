/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/rwlist.hpp
*  PURPOSE:     RenderWare list export for multiple usage
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// Macros used by RW, optimized for usage by the engine (inspired by S2 Games' macros)
#ifdef USE_MACRO_LIST

#define LIST_ISVALID(item) ( (item).next->prev == &(item) && (item).prev->next == &(item) )
#define LIST_VALIDATE(item) ( assert( LIST_ISVALID( (item) ) ) )
#define LIST_APPEND(link, item) ( (item).next = &(link), (item).prev = (link).prev, (item).prev->next = &(item), (item).next->prev = &(item) )
#define LIST_INSERT(link, item) ( (item).next = (link).next, (item).prev = &(link), (item).prev->next = &(item), (item).next->prev = &(item) )
#define LIST_REMOVE(link) ( (link).prev->next = (link).next, (link).next->prev = (link).prev )
#define LIST_CLEAR(link) ( (link).prev = &(link), (link).next = &(link) )
#define LIST_INITNODE(link) ( (link).prev = NULL, (link).next = NULL )
#ifdef _DEBUG
#define LIST_EMPTY(link) ( (link).prev == &(link) && (link).next == &(link) )
#else
#define LIST_EMPTY(link) ( (link).next == &(link) )
#endif //_DEBUG

#else

// Optimized versions.
// Not prone to bugs anymore.
template <typename listType>    inline bool LIST_ISVALID( listType& item )                      { return item.next->prev == &item && item.prev->next == &item; }
template <typename listType>    inline void LIST_VALIDATE( listType& item )                     { return assert( LIST_ISVALID( item ) ); }
template <typename listType>    inline void LIST_APPEND( listType& link, listType& item )       { item.next = &link; item.prev = link.prev; item.prev->next = &item; item.next->prev = &item; }
template <typename listType>    inline void LIST_INSERT( listType& link, listType& item )       { item.next = link.next; item.prev = &link; item.prev->next = &item; item.next->prev = &item; }
template <typename listType>    inline void LIST_REMOVE( listType& link )                       { link.prev->next = link.next; link.next->prev = link.prev; }
template <typename listType>    inline void LIST_CLEAR( listType& link )                        { link.prev = &link; link.next = &link; }
template <typename listType>    inline void LIST_INITNODE( listType& link )                     { link.prev = NULL; link.next = NULL; }
template <typename listType>    inline bool LIST_EMPTY( listType& link )                        { return link.prev == &link && link.next == &link; }

#endif

// These have to be macros unfortunately.
// Special macros used by RenderWare only.
#define LIST_GETITEM(type, item, node) ( (type*)( (unsigned int)(item) - offsetof(type, node) ) )
#define LIST_FOREACH_BEGIN(type, root, node) for ( RwListEntry <type> *iter = (root).next, *niter; iter != &(root); iter = niter ) { type *item = LIST_GETITEM(type, iter, node); niter = iter->next;
#define LIST_FOREACH_END }

template < class type >
struct RwListEntry
{
    RwListEntry <type> *next, *prev;
};
template < class type >
struct RwList
{
    RwListEntry <type> root;
};