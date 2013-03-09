/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare.h
*  PURPOSE:     RenderWare definitions
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#ifndef __RENDERWARE_COMPAT
#define __RENDERWARE_COMPAT

#include <RenderWare_shared.h>

// We need direct3D 9
#include <d3d9.h>

/*****************************************************************************/
/** RenderWare rendering types                                              **/
/*****************************************************************************/

// RenderWare definitions
#define RW_STRUCT_ALIGN             ((int)((~((unsigned int)0))>>1))
#define RW_TEXTURE_NAME_LENGTH      32
#define RW_MAX_TEXTURE_COORDS       8
#define RW_RENDER_UNIT              215.25f

#define VAR_ATOMIC_RENDER_OFFSET    0x00C88024

// RenderWare extensions
#define RW_EXTENSION_HANIM  0x253F2FB

// Yet to analyze
struct RwScene;
struct RwCamera;
struct RpAtomic;
struct RpClump;
struct RwTexture;
struct RpGeometry;

typedef RwCamera*   (*RwCameraPreCallback) (RwCamera * camera);
typedef RwCamera*   (*RwCameraPostCallback) (RwCamera * camera);
typedef bool        (*RpAtomicCallback) (RpAtomic * atomic);
typedef RpClump*    (*RpClumpCallback) (RpClump * clump, void *data);

// RenderWare enumerations
enum RwPrimitiveType : unsigned int
{
    PRIMITIVE_NULL,
    PRIMITIVE_LINE_SEGMENT,
    PRIMITIVE_LINE_SEGMENT_CONNECTED,
    PRIMITIVE_TRIANGLE,
    PRIMITIVE_TRIANGLE_STRIP,
    PRIMITIVE_TRIANGLE_FAN,
    PRIMITIVE_POINT
};
enum RwCameraType : unsigned int
{
    RW_CAMERA_NULL = 0,
    RW_CAMERA_PERSPECTIVE = 1,
    RW_CAMERA_ORTHOGRAPHIC = 2
};
enum RpAtomicFlags : unsigned int
{
    ATOMIC_COLLISION = 1,
    ATOMIC_VISIBLE = 4
};
enum RwRasterLockFlags : unsigned int
{
    RASTER_LOCK_WRITE = 1,
    RASTER_LOCK_READ = 2
};
enum RwTransformOrder : unsigned int
{
    TRANSFORM_INITIAL = 0,
    TRANSFORM_BEFORE = 1,
    TRANSFORM_AFTER = 2
};
enum RpLightFlags : unsigned int
{
    LIGHT_ILLUMINATES_ATOMICS = 1,
    LIGHT_ILLUMINATES_GEOMETRY = 2
};
enum RwRasterType : unsigned int
{
    RASTER_DEPTHBUFFER = 1,
    RASTER_RENDERTARGET,
    RASTER_TEXTURE = 4
};

#define RW_OBJ_REGISTERED           0x02
#define RW_OBJ_VISIBLE              0x04
#define RW_OBJ_HIERARCHY_CACHED     0x10

// RenderWare/plugin base types
struct RwObject
{
    eRwType         type;
    unsigned char   subtype;
    unsigned char   flags;
    unsigned char   privateFlags;
    struct RwFrame* parent;

    inline bool         IsVisible()                     { return IS_FLAG( flags, RW_OBJ_VISIBLE ); }
    inline void         SetVisible( bool vis )          { BOOL_FLAG( flags, RW_OBJ_VISIBLE, vis ); }
};
struct RwVertex
{
    RwV3d        position;
    RwV3d        normal;
    unsigned int color;
    float        u,v;
};

class RwExtensionInterface
{
public:
    unsigned int            id;           
    unsigned int            unknown;            // 4
    unsigned int            structSize;         // 8
    void*                   apply;              // 12
    void*                   callback2;          // 16
    void*                   interpolate;        // 20
    void*                   callback4;          // 24
    void*                   callback5;          // 28
    void*                   callback6;          // 32
    void*                   callback7;          // 36
    void*                   callback8;          // 40
    size_t                  internalSize;       // 44
};
class RwExtension
{
public:
    RwExtensionInterface*   extension;      // 0
    unsigned int            count;          // 4
    size_t                  size;           // 8
    unsigned int            unknown;        // 12

    void*                   data;           // 16
    void*                   _internal;      // 20
};
class RwRenderLink
{
public:
    unsigned char           flags;        
    BYTE                    pad[3];         // 1
    CVector                 position;       // 4
    void*                   context;        // 16
    int                     id;             // 20
};

#define BONE_ROOT           0x08

class RwStaticGeometry
{
public:
    RwStaticGeometry();

    RwObject*               unknown2;       // 0
    unsigned int            unknown;        // 4
    unsigned int            count;          // 8
    unsigned int            unknown3;       // 12
    RwRenderLink*           link;           // 16

    RwRenderLink*           AllocateLink( unsigned int count );
    template <class type>
    void                    ForAllLinks( void (*callback)( RwRenderLink *link, type data ), type data )
    {
        RwRenderLink *_link = link;
        unsigned int n;

        for ( n = 0; n < count; _link++ )
            callback( _link, data );
    }
};
class RwBoneInfo
{
public:
    unsigned int            index;
    BYTE                    pad[4];
    unsigned int            flags;
    DWORD                   pad2;
};
class RwAnimInfo    // dynamic
{
public:
    void*                   unknown;        // 0
    CVector                 offset;         // 4
    BYTE                    pad[12];        // 16
};
class RpAnimation
{
public:
    RwExtension*            ext;            // 0
    BYTE                    pad[32];        // 4
    size_t                  infoSize;       // 36
    BYTE                    pad2[36];       // 40

    // Dynamically extended data
    RwAnimInfo              info[32];
};
class RpSkeleton
{
public:
    unsigned int            boneCount;      // 0
    unsigned int            unknown;        // 4
    void*                   unknown2;       // 8
    RwMatrix*               boneMatrices;   // 12
    unsigned int            unknown3;       // 16
    void*                   unknown5;       // 20
    RwV4d*                  vertexInfo;     // 24
    unsigned int            unknown6;       // 28
    RpAnimation*            curAnim;        // 32, ???
    BYTE                    pad[28];        // 36
    unsigned char*          boneParent2;    // 64
};
class RpAnimHierarchy
{
public:
    unsigned int            flags;          // 0
    unsigned int            boneCount;      // 4
    char*                   data;           // 8
    void*                   unknown4;       // 12
    RwBoneInfo*             boneInfo;       // 16
    unsigned int            unknown5;       // 20
    RpAnimHierarchy*        _this;          // 24
    unsigned char           unknown;        // 28
    unsigned char           unknown2;       // 29
    unsigned short          unknown3;       // 30
    RpAnimation*            anim;           // 32
};
struct RwObjectFrame : public RwObject
{
    RwListEntry <RwObjectFrame>     lFrame;
    void*                           callback;

    void                    AddToFrame( RwFrame *frame );
    void                    RemoveFromFrame();
};

// Private flags
#define RW_FRAME_DIRTY  0x01

struct RwFrame : public RwObject
{
    RwListEntry <RwFrame>   nodeRoot;           // 8
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

    void                    SetModelling( const RwMatrix& mat );
    const RwMatrix&         GetModelling() const        { return modelling; }
    void                    SetPosition( const CVector& pos );
    const CVector&          GetPosition() const         { return modelling.pos; }

    const RwMatrix&         GetLTM() const;

    void                    Link( RwFrame *frame );
    void                    Unlink();
    void                    SetRootForHierarchy( RwFrame *root );
    unsigned int            CountChildren();
    template <class type>
    bool                    ForAllChildren( bool (*callback)( RwFrame *frame, type *data ), type *data )
    {
        for ( RwFrame *_child = child; _child; _child = _child->next )
        {
            if ( !callback( _child, data ) )
                return false;
        }

        return true;
    }
    RwFrame*                GetFirstChild();
    RwFrame*                FindFreeChildByName( const char *name );
    RwFrame*                FindChildByName( const char *name );
    RwFrame*                FindChildByHierarchy( unsigned int id );

    RwFrame*                CloneRecursive() const;

    template <class type>
    bool                    ForAllObjects( bool (*callback)( RwObject *object, type data ), type data )
    {
        RwListEntry <RwObjectFrame> *child;

        for ( child = objects.root.next; child != &objects.root; child = child->next )
        {
            if ( !callback( (RwObject*)( (unsigned int)child - offsetof(RwObjectFrame, lFrame) ), data ) )
                return false;
        }

        return true;
    }
    RwObject*               GetFirstObject();
    RwObject*               GetFirstObject( unsigned char type );
    RwObject*               GetObjectByIndex( unsigned char type, unsigned int idx );
    unsigned int            CountObjectsByType( unsigned char type );
    RwObject*               GetLastVisibleObject();
    RwObject*               GetLastObject();

    template <class type>
    struct _rwObjectGetAtomic
    {
        bool                (*routine)( RpAtomic *atomic, type data );
        type                data;
    };

    template <class type>
    static bool RwObjectDoAtomic( RwObject *child, _rwObjectGetAtomic <type> *info )
    {
        // Check whether the object is a atomic
        if ( child->type != 0x14 )
            return true;

        return info->routine( (RpAtomic*)child, info->data );
    }

    template <class type>
    bool                    ForAllAtomics( bool (*callback)( RpAtomic *atomic, type data ), type data )
    {
        _rwObjectGetAtomic <type> info;

        info.routine = callback;
        info.data = data;

        return ForAllObjects( RwObjectDoAtomic <type>, &info );
    }
    RpAtomic*               GetFirstAtomic();
    void                    SetAtomicVisibility( unsigned short flags );
    void                    FindVisibilityAtomics( RpAtomic **primary, RpAtomic **secondary );

    RpAnimHierarchy*        GetAnimHierarchy();
    void                    RegisterRoot();
    void                    UnregisterRoot();
};
struct RwTexDictionary : public RwObject
{
    RwList <RwTexture>              textures;
    RwListEntry <RwTexDictionary>   globalTXDs;
    RwTexDictionary*                m_parentTxd;

    template <class type>
    bool                    ForAllTextures( bool (*callback)( RwTexture *tex, type ud ), type ud )
    {
        RwListEntry <RwTexture> *child;

        for ( child = textures.root.next; child != &textures.root; child = child->next )
        {
            if ( !callback( (RwTexture*)( (unsigned int)child - offsetof(RwTexture, TXDList) ), ud ) )
                return false;
        }

        return true;
    }

    template <class type>
    bool                    ForAllTexturesSafe( bool (*callback)( RwTexture *tex, type ud ), type ud )
    {
        RwListEntry <RwTexture> *child = textures.root.next;

        while ( child != &textures.root )
        {
            RwListEntry <RwTexture> *nchild = child->next;

            if ( !callback( LIST_GETITEM( RwTexture, child, TXDList ), ud ) )
                return false;

            LIST_VALIDATE( *nchild );

            child = nchild;
        }

        return true;
    }

    RwTexture*              GetFirstTexture();
    RwTexture*              FindNamedTexture( const char *name );
};
struct RwRaster
{
    RwRaster*               parent;                             // 0
    unsigned char*          pixels;                             // 4
    unsigned char*          palette;                            // 8
    int                     width, height, depth;               // 12, 16 / 0x10, 20
    int                     stride;                             // 24 / 0x18
    short                   u, v;                               // 28
    unsigned char           type;                               // 32
    unsigned char           flags;                              // 33
    unsigned char           privateFlags;                       // 34
    unsigned char           format;                             // 35
    unsigned char*          origPixels;                         // 36
    int                     origWidth, origHeight, origDepth;   // 40
    IDirect3DBaseTexture9*  renderResource;                     // 52, Direct3D texture resource
};
struct RwTexture
{
    RwRaster*                   raster;                         // 0
    RwTexDictionary*            txd;                            // 4
    RwListEntry <RwTexture>     TXDList;                        // 8
    char                        name[RW_TEXTURE_NAME_LENGTH];   // 16
    char                        mask[RW_TEXTURE_NAME_LENGTH];   // 48
    unsigned int                flags;                          // 80
    unsigned int                refs;                           // 84

    void                        AddToDictionary( RwTexDictionary *txd );
    void                        RemoveFromDictionary();
};
struct RwTextureCoordinates
{
    float u,v;
};
struct RwCameraFrustum
{
    RwPlane       plane;
    unsigned char x,y,z;
    unsigned char unknown1;
};
struct RwCamera : public RwObjectFrame   //size: 428
{
    RwCameraType            camType;            // 20
    RwCameraPreCallback     preCallback;        // 24
    RwCameraPostCallback    postCallback;       // 28
    RwMatrix                matrix;             // 32
    RwRaster*               rendertarget;       // 96
    RwRaster*               bufferDepth;        // 100
    RwV2d                   screen;             // 104
    RwV2d                   screenInverse;      // 112
    RwV2d                   screenOffset;       // 120
    float                   nearplane;          // 128
    float                   farplane;           // 132
    float                   fog;                // 136
    float                   unknown1;           // 140
    float                   unknown2;           // 144
    RwCameraFrustum         frustum4D[6];       // 148
    RwBBox                  viewBBox;           // 268
    RwV3d                   frustum3D[8];       // 292
    BYTE                    unk[28];            // 388
    RpClump*                clump;              // 416
    RwListEntry <RwCamera>  clumpCameras;       // 420

    void                    BeginUpdate();
    void                    EndUpdate();

    void                    AddToClump( RpClump *clump );
    void                    RemoveFromClump();
};
class RwRender
{
public:
    unsigned int            m_unknown;
};
struct RpMaterialLighting
{
    float ambient, specular, diffuse;
};
class RpMaterial
{
public:
    RwTexture*          texture;      // 0
    RwColor             color;        // 4
    void*               render;       // 8
    RpMaterialLighting  lighting;     // 12
    unsigned short      refs;         // 24
    short               id;           // 26
    void*               unknown;      // 28
};
class RpMaterials
{
public:
                    RpMaterials( unsigned int count );
                    ~RpMaterials();

    bool            Add( RpMaterial *mat );

    RpMaterial**    m_data;
    unsigned int    m_entries;
    unsigned int    m_max;
};
struct RpInterpolation
{
    unsigned int     unknown1;
    unsigned int     unknown2;
    float            unknown3;
    float            unknown4;
};
class RwPipeline
{
public:

};

typedef unsigned int    (__cdecl*RpAtomicInstanceCallback_t)( RpAtomic *atom, RwObject *unk, void *unk2 );
typedef bool            (__cdecl*RpAtomicReinstanceCallback_t)( RpAtomic *atom, RwObject *unk, void *unk2 );
typedef void*           (__cdecl*RpAtomicLightingCallback_t)( RpAtomic *atom );
typedef void            (__cdecl*RpAtomicRenderCallback_t)( RpAtomic *atom );

struct RpAtomicPipelineInfo
{
    RpAtomicInstanceCallback_t      m_instance;
    RpAtomicReinstanceCallback_t    m_reinstance;
    RpAtomicLightingCallback_t      m_lighting;
    RpAtomicRenderCallback_t        m_render;
};

#define RW_ATOMIC_RENDER_REFLECTIVE         0x53F20098
#define RW_ATOMIC_RENDER_VEHICLE            0x53F2009A
#define RW_ATOMIC_RENDER_NIGHT              0x53F2009C

struct RpAtomic : public RwObjectFrame
{
public:
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

    unsigned short          frame;              // 96
    unsigned short          unknown7;           // 98
    RwList <void>           sectors;            // 100
    RwPipeline*             render;             // 108

    RwScene*                scene;              // 112
    RpAtomic*               (*syncCallback)( RpAtomic *atom );      // 116

    RpAnimHierarchy*        anim;               // 120

    unsigned short          modelId;            // 124
    unsigned short          structureId;        // 126, used for components (ok/dam)
    unsigned char           matrixFlags;        // 128
    unsigned char           renderFlags;        // 129
    BYTE                    pad3[6];            // 130
    unsigned int            pipeline;           // 136

    bool                    IsNight();

    void                    AddToClump( RpClump *clump );
    void                    RemoveFromClump();

    void                    SetRenderCallback( RpAtomicCallback callback );

    void                    ApplyVisibilityFlags( unsigned short flags );
    void                    RemoveVisibilityFlags( unsigned short flags );
    unsigned short          GetVisibilityFlags();

    void                    SetExtendedRenderFlags( unsigned short flags );

    void                    FetchMateria( RpMaterials& mats );
};
class RwAtomicZBufferEntry
{
public:
    RpAtomic*               m_atomic;
    RpAtomicCallback        m_render;
    float                   m_distance;
};
class RwAtomicRenderChain
{
public:
    RwAtomicZBufferEntry                m_entry;
    RwListEntry <RwAtomicRenderChain>   list;
};
class RwAtomicRenderChainInterface
{
public:
    RwAtomicRenderChain     m_root;
    RwAtomicRenderChain     m_rootLast;
    RwAtomicRenderChain     m_renderStack;
    RwAtomicRenderChain     m_renderLast;

    bool                    PushRender( RwAtomicZBufferEntry *level );
};

extern RwAtomicRenderChainInterface *const rwRenderChains;

class RpLight : public RwObjectFrame
{
public:
    float                   radius;             // 20
    RwColorFloat            color;              // 24
    float                   coneAngle;          // 40
    RwList <void>           sectors;            // 44
    RwListEntry <RpLight>   sceneLights;        // 52
    unsigned short          frame;              // 60
    unsigned short          unknown2;           // 62
    RwScene*                scene;              // 64
    float                   unk;                // 68
    RpClump*                clump;              // 72
    RwListEntry <RpLight>   clumpLights;        // 76

    // Start of D3D9Light plugin
    unsigned int            lightIndex;         // 84, may be 0-7
    CVector                 attenuation;        // 88

    void                    SetLightIndex( unsigned int idx );
    unsigned int            GetLightIndex() const           { return lightIndex; }

    void                    AddToClump( RpClump *clump );
    void                    RemoveFromClump();

    void                    AddToScene_Local( RwScene *scene );
    void                    AddToScene_Global( RwScene *scene );

    void                    AddToScene( RwScene *scene );
    void                    RemoveFromScene();

    void                    SetColor( const RwColorFloat& color );
};
struct RpClump : public RwObject
{   // RenderWare (plugin) Clump (used by GTA)
    RwList <RpAtomic>       atomics;            // 8
    RwList <RpLight>        lights;             // 16
    RwList <RwCamera>       cameras;            // 24
    RwListEntry <RpClump>   globalClumps;       // 32
    RpClumpCallback         callback;           // 40

    BYTE                    pad[8];             // 44
    unsigned int            renderFlags;        // 52
    BYTE                    pad2[4];            // 56

    RwStaticGeometry*       m_static;           // 60

    void                    Render();

    void                    InitStaticSkeleton();
    RwStaticGeometry*       CreateStaticGeometry();

    RpAnimHierarchy*        GetAtomicAnimHierarchy();
    RpAnimHierarchy*        GetAnimHierarchy();

    void                    ScanAtomicHierarchy( RwFrame **atomics, size_t max );

    RpAtomic*               GetFirstAtomic();
    RpAtomic*               GetLastAtomic();
    RpAtomic*               FindNamedAtomic( const char *name );
    RpAtomic*               Find2dfx();

    void                    SetupAtomicRender();
    void                    RemoveAtomicVisibilityFlags( unsigned short flags );
    void                    FetchMateria( RpMaterials& mats );

    template <class type>
    bool                    ForAllAtomics( bool (*callback)( RpAtomic *child, type data ), type data )
    {
        RwListEntry <RpAtomic> *child = atomics.root.next;

        for ( ; child != &atomics.root; child = child->next )
        {
            if ( !callback( LIST_GETITEM( RpAtomic, child, atomics ), data ) )
                return false;
        }

        return true;
    }

    void                    GetBoneTransform( CVector *offset );
};
struct RpTriangle
{
    unsigned short v1, v2, v3;
    unsigned short materialId;
};
struct RwEffect //size: 64
{
    BYTE                    m_pad[36];              // 0
    RwTexture*              m_primary;              // 36
    RwTexture*              m_secondary;            // 40
    BYTE                    m_pad2[20];             // 44
};
class Rw2dfx
{
public:
    unsigned int            m_count;
    BYTE                    m_pad[12];

    inline RwEffect*        GetEffect( unsigned int idx )
    {
        if ( idx >= m_count )
            return NULL;

        return ( (RwEffect*)( this + 1 ) + idx );
    }
};
class RpGeomMesh
{
public:
    CVector*                m_vertice;                          // 0
    RwSphere                m_bounds;                           // 4
    CVector*                m_positions;                        // 20               
    CVector*                m_normals;                          // 24
};
class RwLinkedMaterial
{
    // Appended to RwLinkedMateria per m_count
public:
    void*                   m_unknown;
    unsigned int            m_pad;
    RpMaterial*             m_material;
};
class RwLinkedMateria
{
public:
    unsigned int            m_unknown;
    unsigned short          m_count;
    void*                   m_unknown2;
    BYTE                    m_pad[6];

    RwLinkedMaterial*       Get( unsigned int index );

    // dynamic class
};

#define RW_STREAMLINE_TASK_INIT_NORMALS         0x00000001

// MTA extensions
struct RpGeomStreamline
{
    unsigned int                m_tasks;
    RwListEntry <RpGeometry>    m_managerNode;
};

#define RW_GEOMETRY_NO_SKIN         0x00000001
#define RW_GEOMETRY_NORMALS         0x00000010
#define RW_GEOMETRY_GLOBALLIGHT     0x00000020

struct RpGeometry : public RwObject
{
public:
    unsigned int            flags;                              // 8
    unsigned short          unknown1;                           // 12
    unsigned short          refs;                               // 14

    unsigned int            triangleSize;                       // 16
    unsigned int            verticeSize;                        // 20
    unsigned int            numMeshes;                          // 24
    unsigned int            texcoordSize;                       // 28

    RpMaterials             materials;                          // 32
    RpTriangle*             triangles;                          // 44
    RwColor*                colors;                             // 48
    RwTextureCoordinates*   texcoords[RW_MAX_TEXTURE_COORDS];   // 52
    RwLinkedMateria*        linkedMateria;                      // 84
    void*                   info;                               // 88
    RpGeomMesh*             meshes;                             // 92, allocated by count in array
    unsigned int            usageFlag;                          // 96
    RpSkeleton*             skeleton;                           // 100
    RwColor*                nightColor;                         // 104
    BYTE                    pad[12];                            // 108
    Rw2dfx*                 m_2dfx;                             // 120

    // Our own extension
    RpGeomStreamline        streamline;                         // 124

    template <class type>
    bool                    ForAllMateria( bool (*callback)( RpMaterial *mat, type data ), type data )
    {
        unsigned int n;

        for ( n=0; n<materials.m_entries; n++ )
        {
            if ( !callback( materials.m_data[n], data ) )
                return false;
        }

        return true;
    }
    bool                    IsAlpha();
    void                    UnlinkFX();
};

typedef RpGeometry RwGeometry;

class RwStructInfo
{
public:
    size_t                  m_size;
};
struct RwScene : public RwObject
{
public:
    BYTE                    m_pad[44];                          // 8
    RwList <RpLight>        m_localLights;                      // 52
    RwList <RpLight>        m_globalLights;                     // 60
};

typedef RwScene RpWorld;

class RtDict
{
public:

};
struct RtDictSchema
{
public:
    const char*             m_name;                             // 0
    unsigned int            m_id;                               // 4

    unsigned int            m_unk;                              // 8
    unsigned int            m_flags;                            // 12

    void*                   m_child;                            // 16
    RtDict*                 m_current;                          // 20

    void                    (*m_reference)( void *item );       // 24
    bool                    (*m_dereference)( void *item );     // 28
    void*                   (*m_get)( void *item );             // 32
    void                    (*m_unkCallback)( void *item );     // 36
    void                    (*m_unkCallback1)( void *item );    // 40
    void                    (*m_unkCallback2)( void *item );    // 44
    void                    (*m_unkCallback3)( void *item, unsigned int unk );  // 48
};


typedef RwTexture* (__cdecl *RwScanTexDictionaryStack_t) ( const char *name, const char *secName );
typedef RwTexture* (__cdecl *RwScanTexDictionaryStackRef_t) ( const char *name );

class RwTextureManager
{
public:
    RwList <RwTexDictionary>        m_globalTxd;                            // 0
    void*                           m_pad;                                  // 8
    RwStructInfo*                   m_txdStruct;                            // 12
    RwTexDictionary*                m_current;                              // 16
    RwScanTexDictionaryStack_t      m_findInstance;                         // 20
    RwScanTexDictionaryStackRef_t   m_findInstanceRef;                      // 24
};
class RwRenderSystem    // TODO
{
public:
    void*                   m_unkStruct;                                    // 0
};
struct RwError
{
    int err1;
    unsigned int err2;
};

typedef void*               (__cdecl*RwFileOpen_t) ( const char *path, const char *mode );
typedef void                (__cdecl*RwFileClose_t) ( void *fp );
typedef long                (__cdecl*RwFileSeek_t) ( void *fp, long offset, int origin );

class RwInterface   // size: 1456
{
public:
    RwCamera*               m_renderCam;                                    // 0
    RwScene*                m_currentScene;                                 // 4
    BYTE                    m_pad8[2];                                      // 6
    unsigned short          m_frame;                                        // 10
    
    BYTE                    m_pad11[4];                                     // 12
    RwRenderSystem          m_renderSystem;                                 // 16

    BYTE                    m_pad[92];                                      // 20
    RwStructInfo*           m_sceneInfo;                                    // 112

    BYTE                    m_pad14[72];                                    // 116
    RwList <RwFrame>        m_nodeRoot;                                     // 188

    BYTE                    m_pad6[24];                                     // 196
    
    RwFileOpen_t            m_fileOpen;                                     // 220
    RwFileClose_t           m_fileClose;                                    // 224
    RwFileSeek_t            m_fileSeek;                                     // 228

    BYTE                    m_pad13[76];                                    // 232

    void*                   (*m_malloc)( size_t size );                     // 308
    void                    (*m_free)( void *data );                        // 312
    void*                   (*m_realloc)( void *data, size_t size );        // 316
    void*                   (*m_calloc)( unsigned int count, size_t size ); // 320
    void*                   (*m_allocStruct)( RwStructInfo *info, unsigned int flags ); // 324
    void*                   m_callback2;                                    // 328

    BYTE                    m_pad2[12];                                     // 332
    RwError                 m_errorInfo;                                    // 344

    BYTE                    m_pad9[4];                                      // 352
    void*                   m_callback3;                                    // 356
    void*                   m_callback4;                                    // 360
    void                    (*m_matrixTransform3)( CVector *dst, const CVector *point, unsigned int count, const RwMatrix *matrices );  // 364
    void*                   m_callback6;                                    // 368

    void*                   m_unknown;                                      // 372
    RwStructInfo*           m_matrixInfo;                                   // 376

    void*                   m_callback7;                                    // 380

    BYTE                    m_pad3[24];                                     // 384
    RwStructInfo*           m_cameraInfo;                                   // 408

    BYTE                    m_pad12[8];                                     // 412
    char                    m_charTable[256];                               // 420
    char                    m_charTable2[256];                              // 676

    float                   m_unknown3;                                     // 680
    
    BYTE                    m_pad5[120];                                    // 936

    RwTextureManager        m_textureManager;                               // 1056
    BYTE                    m_pad7[24];                                     // 1084

    // Render extension
    RwRender*               m_renderData;                                   // 1108
    BYTE                    m_pad4[56];                                     // 1112

    RwPipeline*             m_defaultAtomicPipeline;                        // 1168
    BYTE                    m_pad15[20];                                    // 1172

    RwPipeline*             m_atomicPipeline;                               // 1192
    BYTE                    m_pad16[224];                                   // 1196

    RwStructInfo*           m_clumpInfo;                                    // 1420
    void*                   m_unk2;                                         // 1424
    RwStructInfo*           m_lightInfo;                                    // 1428
    void*                   m_unk;                                          // 1432
    RwList <void>           m_unkList;                                      // 1436
    BYTE                    m_pad10[16];                                    // 1440
};

extern RwInterface **ppRwInterface;
#define pRwInterface (*ppRwInterface)

/*****************************************************************************/
/** RenderWare Helper Classes                                               **/
/*****************************************************************************/

// RenderWare type definitions
typedef int             (* RwIOCallbackClose) (void *data);
typedef size_t          (* RwIOCallbackRead)  (void *data, void *buffer, size_t length);
typedef size_t          (* RwIOCallbackWrite) (void *data, const void *buffer, size_t length);
typedef void*           (* RwIOCallbackSeek)  (void *data, unsigned int offset);

// Swap the current txd with another
class RwTxdStack
{
public:
    RwTxdStack( RwTexDictionary *txd )
    {
        m_txd = (*ppRwInterface)->m_textureManager.m_current;
        (*ppRwInterface)->m_textureManager.m_current = txd;
    }

    ~RwTxdStack()
    {
        (*ppRwInterface)->m_textureManager.m_current = m_txd;
    }

private:
    RwTexDictionary*    m_txd;
};

/*****************************************************************************/
/** RenderWare I/O                                                          **/
/*****************************************************************************/

// RenderWare enumerations
enum RwStreamType : unsigned int
{
    STREAM_TYPE_NULL = 0,
    STREAM_TYPE_FILE = 1,
    STREAM_TYPE_FILENAME = 2,
    STREAM_TYPE_BUFFER = 3,
    STREAM_TYPE_CALLBACK = 4
};
enum RwStreamMode : unsigned int
{
    STREAM_MODE_NULL = 0,
    STREAM_MODE_READ = 1,
    STREAM_MODE_WRITE = 2,
    STREAM_MODE_APPEND = 3
};

// RenderWare base types
struct RwBuffer
{
    void*           ptr;
    unsigned int    size;
};
union RwStreamTypeData
{
    struct
    {
        unsigned int        position;
        unsigned int        size;
        void*               ptr_file;
    };
    struct
    {
        void*               file;
    };
    struct
    {
        RwIOCallbackClose   callbackClose;
        RwIOCallbackRead    callbackRead;
        RwIOCallbackWrite   callbackWrite;
        RwIOCallbackSeek    callbackSeek;
        void*               ptr_callback;
    };
};
struct RwStream
{
    RwStreamType        type;
    RwStreamMode        mode;
    int                 pos;
    RwStreamTypeData    data;
    int                 id;
};
struct RwBlocksInfo
{
    unsigned short      count;
    unsigned short      unk;
};
struct RwChunkHeader
{
    unsigned int        id;
    size_t              size;
    unsigned int        unk2;
    unsigned int        unk3;
    unsigned int        isComplex;
};

// Plugin functionss
typedef RpGeometry*     (__cdecl*RpGeometryPluginConstructor)( RpGeometry *geom, size_t offset );
typedef void            (__cdecl*RpGeometryPluginDestructor)( RpGeometry *geom, size_t offset );
typedef RpGeometry*     (__cdecl*RpGeometryPluginCopyConstructor)( RpGeometry *dst, RpGeometry *src, size_t offset );

#endif
