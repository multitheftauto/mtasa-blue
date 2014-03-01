/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RpAtomic.h
*  PURPOSE:     RenderWare native function implementations
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#ifndef _RENDERWARE_ATOMIC_
#define _RENDERWARE_ATOMIC_

typedef unsigned int    (__cdecl*RpAtomicInstanceCallback_t)( RpAtomic *atom, RwObject *unk, void *unk2 );
typedef bool            (__cdecl*RpAtomicReinstanceCallback_t)( RpAtomic *atom, RwObject *unk, void *unk2 );
typedef void*           (__cdecl*RpAtomicLightingCallback_t)( RpAtomic *atom );
typedef void            (__cdecl*RpAtomicRenderCallback_t)( RpAtomic *atom );

struct RpAtomicPipelineInfo
{
    RpAtomicInstanceCallback_t      instance;
    RpAtomicReinstanceCallback_t    reinstance;
    RpAtomicLightingCallback_t      lighting;
    RpAtomicRenderCallback_t        render;
};

struct CEnvMapAtomicSA   //size: 12 bytes
{
    CEnvMapAtomicSA( float unk1, float xMod, float yMod )
    {
        m_unk1 = unk1;
        m_xMod = xMod;
        m_yMod = yMod;
    }

    float                   m_unk1;             // 0
    float                   m_xMod;             // 4
    float                   m_yMod;             // 8

    void* operator new ( size_t );
    void operator delete ( void *ptr );
};

#define RW_ATOMIC_RENDER_REFLECTIVE         0x53F20098
#define RW_ATOMIC_RENDER_VEHICLE            0x53F2009A
#define RW_ATOMIC_RENDER_NIGHT              0x53F2009C

struct RpAtomic : public RwObjectFrame
{
    typedef RwNodeList <RwSector*> sectorNode;

    RpAtomicPipelineInfo*   info;               // 20

    RpGeometry*             geometry;           // 24
    RwSphere                bsphereLocal;       // 28
    RwSphere                bsphereWorld;       // 44

    RpClump*                clump;              // 60
    RwListEntry <RpAtomic>  atomics;            // 64

    RpAtomicCallback        renderCallback;     // 72
    RpInterpolation         interpolation;      // 76

    unsigned short          normalOffset;       // 92
    BYTE                    pad4[2];            // 94

    unsigned short          frame;              // 96, begin of a substructure
    unsigned short          unknown7;           // 98
    RwList <sectorNode>     sectors;            // 100
    RwPipeline*             render;             // 108

    RwScene*                scene;              // 112
    RpAtomic*               (*syncCallback)( RpAtomic *atom );  // 116

    RpAnimHierarchy*        anim;               // 120

    unsigned short          modelId;            // 124
    unsigned short          componentFlags;     // 126, used for components (ok/dam)
    BYTE                    pad3[4];            // 128
    CEnvMapAtomicSA*        envMap;             // 132, atomic environment map plugin, allocated from pool (on demand)
    unsigned int            pipeline;           // 136

    // Methods.
    const RwSphere&         GetWorldBoundingSphere      ( void );

    bool                    IsNight                     ( void );

    void                    AddToClump                  ( RpClump *clump );
    void                    RemoveFromClump             ( void );

    void                    SetRenderCallback           ( RpAtomicCallback callback );

    void                    FetchMateria                ( RpMaterials& mats );
};

struct RpAtomicContainer
{
    RpAtomic*   atomic;
    char        szName[17];
};

#endif //_RENDERWARE_ATOMIC_