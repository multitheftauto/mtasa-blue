/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RpClump.h
*  PURPOSE:     RenderWare native function implementations
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#ifndef _RENDERWARE_CLUMP_
#define _RENDERWARE_CLUMP_

struct RpClump : public RwObject
{   // RenderWare (plugin) Clump (used by GTA)
    RwList <RpAtomic>       atomics;            // 8
    RwList <RpLight>        lights;             // 16
    RwList <RwCamera>       cameras;            // 24
    RwListEntry <RpClump>   globalClumps;       // 32
    RpClumpCallback         callback;           // 40

    BYTE                    pad[8];             // 44
    unsigned int            renderFlags;        // 52
    unsigned int            alpha;              // 56

    RwStaticGeometry*       pStatic;            // 60

    // Methods.
    void                    Render                      ( void );

    void                    InitStaticSkeleton          ( void );
    RwStaticGeometry*       CreateStaticGeometry        ( void );

    RpAnimHierarchy*        GetAtomicAnimHierarchy      ( void );
    RpAnimHierarchy*        GetAnimHierarchy            ( void );

    void                    ScanAtomicHierarchy         ( RwFrame **atomics, size_t max );

    RpAtomic*               GetFirstAtomic              ( void );
    RpAtomic*               GetLastAtomic               ( void );
    RpAtomic*               FindNamedAtomic             ( const char *name );
    RpAtomic*               Find2dfx                    ( void );

    void                    SetupAtomicRender           ( void );
    void                    RemoveAtomicComponentFlags  ( unsigned short flags );
    void                    FetchMateria                ( RpMaterials& mats );

    template <class type>
    bool                    ForAllAtomics               ( bool (*callback)( RpAtomic *child, type data ), type data )
    {
        RwListEntry <RpAtomic> *child = atomics.root.next;

        for ( ; child != &atomics.root; child = child->next )
        {
            if ( !callback( LIST_GETITEM( RpAtomic, child, atomics ), data ) )
                return false;
        }

        return true;
    }

    void                    GetBoneTransform            ( CVector *offset );
};

// Clump API.
RpClump*            RpClumpCreate( void );      // US exe: 0x0074A290

#endif //_RENDERWARE_CLUMP_