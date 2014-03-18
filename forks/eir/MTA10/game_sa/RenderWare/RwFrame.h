/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwFrame.h
*  PURPOSE:     RenderWare native function implementations
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#ifndef _RENDERWARE_FRAME_
#define _RENDERWARE_FRAME_

// Private flags
#define RW_FRAME_DIRTY          0x01    // needs updating in RwFrameSyncDirty
#define RW_FRAME_UPDATEMATRIX   0x04    // modelling will be copied to LTM
#define RW_FRAME_UPDATEFLAG     ( RW_FRAME_DIRTY | 2 )

struct RwFrame : public RwObject
{
    RwListEntry <RwFrame>   nodeRoot;           // 8, node in dirty list
    RwMatrix                modelling;          // 16
    RwMatrix                ltm;                // 80
    RwList <RwObjectFrame>  objects;            // 144
    RwFrame*                child;              // 152
    RwFrame*                next;               // 156
    RwFrame*                root;               // 160

    // Rockstar Frame extension (0x253F2FE) (24 bytes)
    RpAnimHierarchy*        anim;               // 164
    BYTE                    pluginData[4];      // 168

    char                    szName[16];         // 172

    BYTE                    pad3[8];            // 188
    unsigned int            hierarchyId;        // 196

    // Methods.
    void                    SetModelling            ( const RwMatrix& mat );
    const RwMatrix&         GetModelling            ( void ) const          { return modelling; }
    void                    SetPosition             ( const CVector& pos );
    const CVector&          GetPosition             ( void ) const          { return modelling.vPos; }

    const RwMatrix&         GetLTM                  ( void );

    void                    Link                    ( RwFrame *frame );
    void                    Unlink                  ( void );
    void                    SetRootForHierarchy     ( RwFrame *root );
    unsigned int            CountChildren           ( void );
    template <class type, typename returnType>
    bool                    ForAllChildren( returnType (__cdecl*callback)( RwFrame *frame, type *data ), type *data )
    {
        for ( RwFrame *child = this->child; child; child = child->next )
        {
            if ( !callback( child, data ) )
                return false;
        }

        return true;
    }
    RwFrame*                GetFirstChild           ( void );
    RwFrame*                FindFreeChildByName     ( const char *name );
    RwFrame*                FindChildByName         ( const char *name );
    RwFrame*                FindChildByHierarchy    ( unsigned int id );

    RwFrame*                CloneRecursive          ( void ) const;

    template <class type, typename returnType>
    bool                    ForAllObjects( returnType (__cdecl*callback)( RwObject *object, type data ), type data )
    {
        LIST_FOREACH_BEGIN( RwObjectFrame, objects.root, lFrame )
            if ( !callback( item, data ) )
                return false;
        LIST_FOREACH_END

        return true;
    }

    RwObject*               GetFirstObject          ( void );
    RwObject*               GetFirstObject          ( unsigned char type );
    RwObject*               GetObjectByIndex        ( unsigned char type, unsigned int idx );
    unsigned int            CountObjectsByType      ( unsigned char type );
    RwObject*               GetLastVisibleObject    ( void );
    RwObject*               GetLastObject           ( void );

    template <class type>
    struct _rwObjectGetAtomic
    {
        int                 (*routine)( RpAtomic *atomic, type data );
        type                data;
    };

    template <class type>
    static int RwObjectDoAtomic( RwObject *child, _rwObjectGetAtomic <type> *info )
    {
        // Check whether the object is a atomic
        if ( child->type != RW_ATOMIC )
            return true;

        return info->routine( (RpAtomic*)child, info->data );
    }

    template <class type, typename returnType>
    bool                    ForAllAtomics( returnType (__cdecl*callback)( RpAtomic *atomic, type data ), type data )
    {
        _rwObjectGetAtomic <type> info;

        info.routine = callback;
        info.data = data;

        return ForAllObjects( RwObjectDoAtomic <type>, &info );
    }
    RpAtomic*               GetFirstAtomic          ( void );
    void                    SetAtomicComponentFlags ( unsigned short flags );
    void                    FindComponentAtomics    ( RpAtomic **okay, RpAtomic **damaged );

    bool                    IsWaitingForUpdate      ( void ) const          { return IS_ANY_FLAG( privateFlags, RW_FRAME_UPDATEFLAG ); }
    void                    SetUpdating             ( bool flag )           { BOOL_FLAG( privateFlags, RW_FRAME_UPDATEFLAG, flag ); }

    RpAnimHierarchy*        GetAnimHierarchy        ( void );
    void                    _Update                 ( RwList <RwFrame>& list );
    void                    Update                  ( void );
    void                    UpdateMTA               ( void );
    void                    ThrowUpdate             ( void );
};

// Matrix copying
inline void RwFrameCopyMatrix ( RwFrame * dst, RwFrame * src )
{
    dst->SetModelling( src->modelling );
    dst->ltm = src->ltm;    // ltm is not supposed to be copied.
}

typedef RwObject* (__cdecl*frameObjectIterator_t)( RwObject *obj, void *data );

// API exports.
RwFrame* __cdecl RwFrameLink            ( RwFrame *frame, RwFrame *child );
RwFrame* __cdecl RwFrameAddChild        ( RwFrame *frame, RwFrame *child );
RwFrame* __cdecl RwFrameRemoveChild     ( RwFrame *child );
RwFrame* __cdecl RwFrameForAllObjects   ( RwFrame *frame, frameObjectIterator_t callback, void *data );
RwFrame* __cdecl RwFrameForAllObjects   ( RwFrame *frame, void *callback, void *data );
RwFrame* __cdecl RwFrameFindFrame       ( RwFrame *frame, const char *name );

#endif //_RENDERWARE_FRAME_