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

// Macros used by RW, taken from SGU :)
#define LIST_VALIDATE(item) ( assert( (item).next->prev == &(item) && (item).prev->next == &(item) ) )
#define LIST_APPEND(link, item) ( (item).next = &(link), (item).prev = (link).prev, (item).prev->next = &(item), (item).next->prev = &(item) )
#define LIST_INSERT(link, item) ( (item).next = (link).next, (item).prev = &(link), (item).prev->next = &(item), (item).next->prev = &(item) )
#define LIST_REMOVE(link) ( (link).prev->next = (link).next, (link).next->prev = (link).prev )
#define LIST_CLEAR(link) ( (link).prev = &(link), (link).next = &(link) )
#define LIST_INITNODE(link) ( (link).prev = NULL, (link).next = NULL )
#define LIST_EMPTY(link) ( (link).prev == &(link) && (link).next == &(link) )
#define LIST_GETITEM(type, item, node) ( (type*)( (unsigned int)(item) - offsetof(type, node) ) )
#define LIST_FOREACH_BEGIN(type, root, node) for ( RwListEntry <type> *iter = (root).next; iter != &(root); iter = iter->next ) { type *item = LIST_GETITEM(type, iter, node);
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