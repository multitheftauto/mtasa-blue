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
struct RwFrame;
struct RwScene;
struct RwCamera;
struct RpAtomic;
struct RpLight;
struct RpClump;
struct RwTexture;
struct RpGeometry;
struct RwStream;

typedef RwCamera*   (*RwCameraPreCallback) (RwCamera * camera);
typedef RwCamera*   (*RwCameraPostCallback) (RwCamera * camera);
typedef RpAtomic*   (*RpAtomicCallback) (RpAtomic * atomic);
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

/*==================================================================
    RenderWare Structure Definitions

    All interfaces which extend from RwObject are plugins. Plugins are
    dynamically allocated in the RenderWare system after all extensions
    have registered their data in the RenderWare plugin system. Every
    plugin has got a structure information class pointer in RwInterface
    (ex. m_clumpInfo).
    
    Through the RenderWare API you can extend these structures. Doing so 
    might either append or insert new data chunks into each RwObject.
    Be careful if you do so: RenderWare definitions here are a static
    representation of what is handled through dynamic offsets in the
    engine! Be sure to keep the definitions here up-to-date.

    You may add as many static methods to the structures as you want.
    Adding virtual methods is discouraged due to compatibility reasons.
==================================================================*/

#define RW_OBJ_REGISTERED           0x02
#define RW_OBJ_VISIBLE              0x04
#define RW_OBJ_HIERARCHY_CACHED     0x08

// RenderWare/plugin base types
struct RwObject
{
public:
    eRwType         type;
    unsigned char   subtype;
    unsigned char   flags;
    unsigned char   privateFlags;
    RwFrame*        parent;                 // should be RwFrame if obj -> RpClump

    inline bool         IsVisible( void )               { return IS_FLAG( flags, RW_OBJ_VISIBLE ); }
    inline void         SetVisible( bool vis )          { BOOL_FLAG( flags, RW_OBJ_VISIBLE, vis ); }
};
struct RwVertex
{
    RwV3d        position;
    RwV3d        normal;
    unsigned int color;
    float        u,v;
};

struct RwExtensionInterface
{
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
struct RwExtension
{
    RwExtensionInterface*   extension;      // 0
    unsigned int            count;          // 4
    size_t                  size;           // 8
    unsigned int            unknown;        // 12

    void*                   data;           // 16
    void*                   pInternal;      // 20
};
struct RwRenderLink
{
    unsigned char           flags;        
    BYTE                    pad[3];         // 1
    CVector                 position;       // 4
    void*                   context;        // 16
    int                     id;             // 20
};

#define BONE_ROOT           0x08

/*=========================================================
    RwStaticGeometry

    This class has been found by analyzing the GTA:SA
    CClumpModelInfoSAInterface. Models without animation would
    receive this static geometry data.

    The usage of this class is highly theoretical and open for
    discussion!
=========================================================*/
struct RwStaticGeometry
{
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
        RwRenderLink *link = this->link;

        for ( unsigned int n = 0; n < this->count; link++ )
            callback( link, data );
    }
};
struct RwBoneInfo
{
    unsigned int            index;
    BYTE                    pad[4];
    unsigned int            flags;
    DWORD                   pad2;
};
struct RwAnimInfo    // dynamic
{
    void*                   unknown;        // 0
    CVector                 offset;         // 4
    BYTE                    pad[12];        // 16
};
struct RpAnimation
{
    RwExtension*            ext;
    BYTE                    pad[32];        // 4
    size_t                  infoSize;       // 36
    BYTE                    pad2[36];       // 40

    // Dynamically extended data
    RwAnimInfo              info[32];
};
struct RpSkeleton
{
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
struct RpAnimHierarchy
{
    unsigned int            flags;          // 0
    unsigned int            boneCount;      // 4
    char*                   data;           // 8
    void*                   unknown4;       // 12
    RwBoneInfo*             boneInfo;       // 16
    unsigned int            unknown5;       // 20
    RpAnimHierarchy*        pThis;          // 24
    unsigned char           unknown;        // 28
    unsigned char           unknown2;       // 29
    unsigned short          unknown3;       // 30
    RpAnimation*            anim;           // 32
};
struct RwObjectFrame : public RwObject
{
    typedef void (__cdecl*syncCallback_t)( RwObject *obj );

    RwListEntry <RwObjectFrame>     lFrame;
    syncCallback_t                  callback;

    void                    AddToFrame( RwFrame *frame );
    void                    RemoveFromFrame( void );
};

#include "RenderWare/RwFrame.h"
#include "RenderWare/RwTexDictionary.h"
#include "RenderWare/RwTexture.h"
#include "RenderWare/RwCamera.h"

struct RwRender
{
    unsigned int            m_unknown;
};

#include "RenderWare/RpMaterial.h"

struct RpInterpolation
{
    unsigned int     unknown1;
    unsigned int     unknown2;
    float            unknown3;
    float            unknown4;
};
struct RwPipeline
{
public:

};

template <typename dataType>
struct RwNodeList
{
    RwListEntry <RwNodeList> node;
    dataType data;
};

struct RwSector
{
    typedef RwNodeList <RpLight*> lightNode;

    RwListEntry <RwSector>  node;               // 0
    BYTE                    m_pad[56];          // 8
    RwList <lightNode>      m_localLights;      // 64
};

#include "RenderWare/RpAtomic.h"
#include "RenderWare/RpLight.h"
#include "RenderWare/RpClump.h"

struct RpTriangle
{
    unsigned short v1, v2, v3;
    unsigned short materialId;
};
struct RwEffect //size: 64
{
    BYTE                    pad[36];                // 0
    RwTexture*              primary;                // 36
    RwTexture*              secondary;              // 40
    BYTE                    pad2[20];               // 44
};
struct Rw2dfx
{
    unsigned int            count;
    BYTE                    pad[12];

    inline RwEffect*        GetEffect( unsigned int idx )
    {
        if ( idx >= count )
            return NULL;

        return ( (RwEffect*)( this + 1 ) + idx );
    }
};
struct RpGeomMesh
{
    CVector*                vertice;                // 0, those arrays are allocated by geom->m_verticeSize
    RwSphere                bounds;                 // 4
    CVector*                positions;              // 20               
    CVector*                normals;                // 24
};
struct RwLinkedMaterial
{
    // Appended to RwLinkedMateria per m_count
    void*                   unknown;
    unsigned int            pad;
    RpMaterial*             material;
};
struct RwLinkedMateria
{
    unsigned int            unknown;
    unsigned short          count;
    void*                   unknown2;
    BYTE                    pad[6];

    RwLinkedMaterial*       Get( unsigned int index );

    // dynamic class
};

#include "RenderWare/RpGeometry.h"

#define ALIGN( num, sector, align ) (((num) + (sector) - 1) & (~((align) - 1)))

struct RwFreeListMemBlock
{
    RwListEntry <RwFreeListMemBlock>    list;

    unsigned char*  GetMetaData( void )
    {
        return (unsigned char*)( this + 1 );
    }

    static inline void SetBlockUsed( unsigned char *meta, unsigned int idx, bool used )
    {
        if ( used )
            meta[idx / 8] |= 0x80 >> (idx % 8);
        else
            meta[idx / 8] &= ~(0x80 >> (idx % 8));
    }

    void SetBlockUsed( unsigned int idx, bool used = true )
    {
        SetBlockUsed( GetMetaData(), idx, used );
    }

    void* GetBlockPointer( unsigned int index, size_t blockSize, size_t alignment, size_t metaDataSize )
    {
        return (void*)( ALIGN( (unsigned int)this + alignment + metaDataSize, 8, alignment ) + index * blockSize );
    }
};

#define RWALLOC_SELFCONSTRUCT   0x01
#define RWALLOC_RUNTIME         0x02

struct RwFreeList
{
    size_t                          m_blockSize;    // 0
    unsigned int                    m_blockCount;   // 4
    size_t                          m_metaDataSize; // 8
    size_t                          m_alignment;    // 12
    RwList <RwFreeListMemBlock>     m_memBlocks;    // 16
    unsigned int                    m_flags;        // 24
    RwListEntry <RwFreeList>        m_globalLists;  // 28
};

typedef RwFreeList RwStructInfo;    // I leave this for understanding purposes.

struct RwScene : public RwObject
{
    BYTE                    pad[44];                            // 8
    RwList <RpLight>        localLights;                        // 52
    RwList <RpLight>        globalLights;                       // 60
};

typedef RwScene RpWorld;

struct RtDict
{
public:

};
struct RtDictSchema
{
    const char*             name;                               // 0
    unsigned int            id;                                 // 4

    unsigned int            unk;                                // 8
    unsigned int            flags;                              // 12

    void*                   child;                              // 16
    RtDict*                 current;                            // 20

    void                    (*reference)( void *item );         // 24
    bool                    (*dereference)( void *item );       // 28
    void*                   (*get)( void *item );               // 32
    void                    (*unkCallback)( void *item );       // 36
    void                    (*unkCallback1)( void *item );      // 40
    void                    (*unkCallback2)( void *item );      // 44
    void                    (*unkCallback3)( void *item, unsigned int unk );    // 48
};


typedef RwTexture* (__cdecl *RwScanTexDictionaryStack_t) ( const char *name, const char *secName );
typedef RwTexture* (__cdecl *RwScanTexDictionaryStackRef_t) ( const char *name );

struct RwTextureManager
{
    RwList <RwTexDictionary>        globalTxd;                              // 0
    void*                           pad;                                    // 8
    RwStructInfo*                   txdStruct;                              // 12
    RwTexDictionary*                current;                                // 16
    RwScanTexDictionaryStack_t      findInstance;                           // 20
    RwScanTexDictionaryStackRef_t   findInstanceRef;                        // 24
};
struct RwRenderSystem    // TODO
{
    void*                   unkStruct;                                      // 0
    void*                   callback;                                       // 4
};
struct RwError
{
    int err1;
    unsigned int err2;
};

typedef void*               (__cdecl*RwFileOpen_t) ( const char *path, const char *mode );
typedef int                 (__cdecl*RwFileClose_t) ( void *fp );
typedef long                (__cdecl*RwFileSeek_t) ( void *fp, long offset, int origin );

typedef unsigned int        (__cdecl*RwFileRead_t) ( void *buf, unsigned int elemSize, unsigned int elemCount, void *fp );
typedef unsigned int        (__cdecl*RwFileWrite_t) ( const void *buf, unsigned int elemSize, unsigned int elemCount, void *fp );
typedef int                 (__cdecl*RwFileTell_t) ( void *fp );

typedef int                 (__cdecl*RwReadTexture_t) ( RwStream *stream, RwTexture **out, size_t blockSize );

enum eRwDeviceCmd : unsigned int
{
    RWSTATE_FOGENABLE = 14,
    RWSTATE_FOGCOLOR
};

typedef void*               (__cdecl*RwMemAlloc_t) ( size_t size, unsigned int flags );
typedef void                (__cdecl*RwMemFree_t) ( void *ptr );
typedef void*               (__cdecl*RwMemRealloc_t) ( void *ptr, size_t size, unsigned int flags );
typedef void*               (__cdecl*RwMemCellAlloc_t) ( size_t count, size_t cellSize );

struct RwMemoryDescriptor
{
    RwMemAlloc_t        m_malloc;
    RwMemFree_t         m_free;
    RwMemRealloc_t      m_realloc;
    RwMemCellAlloc_t    m_calloc;
};

//padlevel: 22
struct RwInterface   // size: 1456
{
    RwCamera*               m_renderCam;                                    // 0
    RwScene*                m_currentScene;                                 // 4
    unsigned short          m_renderScanCode;                               // 8
    unsigned short          m_frame;                                        // 10
    
    BYTE                    m_pad11[4];                                     // 12
    RwRenderSystem          m_renderSystem;                                 // 16

    BYTE                    m_pad[8];                                       // 24
    void                    (*m_deviceCommand)( eRwDeviceCmd cmd, int param );  // 32
    void                    (*m_getDeviceCommand)( eRwDeviceCmd cmd, int& param );  // 36
    BYTE                    m_pad20[72];                                    // 40

    RwStructInfo*           m_sceneInfo;                                    // 112

    BYTE                    m_pad14[60];                                    // 116
    RwReadTexture_t         m_readTexture;                                  // 176, actual internal texture reader

    BYTE                    m_pad17[8];                                     // 180
    RwList <RwFrame>        m_nodeRoot;                                     // 188, list of dirty frames

    BYTE                    m_pad6[4];                                      // 196
    RwFileOpen_t            m_fileOpen;                                     // 200
    RwFileClose_t           m_fileClose;                                    // 204
    RwFileRead_t            m_fileRead;                                     // 208
    RwFileWrite_t           m_fileWrite;                                    // 212

    BYTE                    m_pad22[12];                                    // 216
    RwFileSeek_t            m_fileSeek;                                     // 228

    BYTE                    m_pad13[4];                                     // 232
    RwFileTell_t            m_fileTell;                                     // 236

    BYTE                    m_pad21[12];                                    // 240

    void                    (*m_strncpy)( char *buf, const char *dst, size_t cnt ); // 252
    BYTE                    m_pad18[32];                                    // 256

    size_t                  (*m_strlen)( const char *str );                 // 288
    BYTE                    m_pad19[16];                                    // 292

    RwMemoryDescriptor      m_memory;                                       // 308
    void*                   (*m_allocStruct)( RwStructInfo *info, unsigned int flags ); // 324
    void*                   (*m_freeStruct)( RwStructInfo *info, void *ptr );   // 328

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

    BYTE                    m_pad3[20];                                     // 384
    RwStructInfo*           m_streamInfo;                                   // 404
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

    RwStructInfo*           m_atomicInfo;                                   // 1420
    RwStructInfo*           m_clumpInfo;                                    // 1424
    RwStructInfo*           m_lightInfo;                                    // 1428
    void*                   m_unk;                                          // 1432
    RwList <void>           m_unkList;                                      // 1436
    BYTE                    m_pad10[16];                                    // 1440
};

// offset 0x00C9BF00 (1.0 US and 1.0 EU)
//padlevel: 2
struct RwDeviceInformation
{
    BYTE                    pad[108];                                       // 0
    char                    maxAnisotropy;                                  // 108

    BYTE                    pad2[48];                                       // 112
    unsigned int            maxLights;                                      // 160
};

// Special RenderWare namespace.
namespace RenderWare
{
    inline RwInterface*             GetInterface            ( void )    { return *(RwInterface**)0x00C97B24; }
    inline RwDeviceInformation&     GetDeviceInformation    ( void )    { return *(RwDeviceInformation*)0x00C9BF00; }
};

/*****************************************************************************/
/** RenderWare Helper Definitions                                           **/
/*****************************************************************************/

// Include basic dependencies.
#include "RenderWare/RwMath.h"

// Include plugins.
#include "RenderWare/RwStream.h"
#include "RenderWare/RwRenderStates.h"
#include "RenderWare/RwTextureD3D9.h"
#include "RenderWare/RwUtilsD3D9.h"
#include "RenderWare/RpAtomicD3D9.h"

/* RenderWare macros */
inline RwFrame* RpGetFrame( RwObject *obj )                     { return obj->parent; }
inline void RpSetFrame( RwObject *obj, RwFrame *frame )         { obj->parent = frame; }

/*****************************************************************************/
/** RenderWare Plugin System                                                **/
/*****************************************************************************/

// Used for functions that are not yet implemented.
inline void __declspec(naked)    invalid_ptr()
{
    // PoD: throw an exception here? we could catch it during runtime and tell the user that
    // unimplemented functionality was called.
    __asm int 3;
}

#define RW_JUSTDEF( name )          name##_t name = (##name##_t)invalid_ptr
#define RW_JUSTSET( name, ptr )     name = (name##_t)ptr

// RenderWare method helpers
#define RWP_METHOD_NAME( className, methodName )    className##methodName
#define RWP_METHOD_OPEN( className, methodName ) \
    RWP_METHOD_NAME( className, methodName )##_t    RWP_METHOD_NAME( className, methodName )

#define RWP_METHOD_INST( className, methodName ) \
    RWP_METHOD_OPEN( className, methodName ) = (RWP_METHOD_NAME( className, methodName )##_t)invalid_ptr

#define RWP_METHOD_DECLARE( className, methodName ) \
    extern RWP_METHOD_OPEN( className, methodName )

// Plugin functions
#define RWP_UTIL_CONSTRUCTOR( rwobj ) \
    rwobj*              (__cdecl*##rwobj##PluginConstructor)( rwobj *obj, size_t offset )
#define RWP_UTIL_DESTRUCTOR( rwobj ) \
    void                (__cdecl*##rwobj##PluginDestructor)( rwobj *obj, size_t offset )
#define RWP_UTIL_COPYCONSTRUCTOR( rwobj ) \
    rwobj*              (__cdecl*##rwobj##PluginCopyConstructor)( rwobj *dst, const rwobj *src, size_t offset, unsigned int pluginId )

#define RWP_UTIL_REGISTER( rwobj ) \
    typedef RWP_UTIL_CONSTRUCTOR( rwobj ); \
    typedef RWP_UTIL_DESTRUCTOR( rwobj ); \
    typedef RWP_UTIL_COPYCONSTRUCTOR( rwobj ); \
    typedef size_t      (__cdecl*##rwobj##RegisterPlugin_t)( size_t size, unsigned int id, rwobj##PluginConstructor constructor, rwobj##PluginDestructor destructor, rwobj##PluginCopyConstructor copyConstr )

// Stream plugin functions
#define RWP_UTIL_STREAMFUNCS( rwobj ) \
    typedef RwStream*   (__cdecl*##rwobj##PluginStreamRead)( RwStream *stream, size_t size, rwobj *obj ); \
    typedef int         (__cdecl*##rwobj##PluginStreamWrite)( RwStream *stream, size_t size, rwobj *obj ); \
    typedef size_t      (__cdecl*##rwobj##PluginStreamGetSize)( rwobj *obj ); \
    typedef size_t      (__cdecl*##rwobj##RegisterPluginStream_t)( unsigned int id, rwobj##PluginStreamRead readCallback, rwobj##PluginStreamWrite writeCallback, rwobj##PluginStreamGetSize sizeCallback )

// Used to declare a plugin type.
#define RW_PLUGIN_DECLARE( className ) \
    RWP_UTIL_STREAMFUNCS( className ); \
    RWP_UTIL_REGISTER( className )

// Declare all plugins.
RW_PLUGIN_DECLARE( RpAtomic );
RW_PLUGIN_DECLARE( RpClump );
RW_PLUGIN_DECLARE( RpGeometry );
RW_PLUGIN_DECLARE( RpMaterial );
RW_PLUGIN_DECLARE( RwScene );
RW_PLUGIN_DECLARE( RpLight );
RW_PLUGIN_DECLARE( RwFrame );
RW_PLUGIN_DECLARE( RwCamera );
RW_PLUGIN_DECLARE( RwRaster );
RW_PLUGIN_DECLARE( RwTexture );
RW_PLUGIN_DECLARE( RwTexDictionary );

// Used to declare the plugin interface
#define RW_PLUGIN_INTERFACE_INST( className ) \
    RWP_METHOD_INST( className, RegisterPlugin ); \
    RWP_METHOD_INST( className, RegisterPluginStream );

// For headers.
#define RW_PLUGIN_INTERFACE_EXTERN( className ) \
    RWP_METHOD_DECLARE( className, RegisterPlugin ); \
    RWP_METHOD_DECLARE( className, RegisterPluginStream )

// Utilities
#define RW_FUNC_PTR( className, methodName, ptr ) \
    RWP_METHOD_NAME( className, methodName ) = (RWP_METHOD_NAME(className, methodName)##_t)ptr

// * Plugin interface manager
template <class RwTemplate>
struct RwPluginDescriptor
{
    RW_PLUGIN_DECLARE( RwTemplate );

    size_t                              m_offset;               // 0
    unsigned int                        m_pluginId;             // 4
    BYTE                                m_pad[32];              // 8
    RwTemplatePluginCopyConstructor     m_copyConstructor;      // 40
    RwTemplatePluginDestructor          m_destructor;           // 44
    
    RwPluginDescriptor*                 m_next;                 // 48
};
template <class RwTemplate>
struct RwPluginRegistry
{
    typedef RwPluginDescriptor <RwTemplate> RwTemplateDescriptor;

    BYTE                                m_pad[16];              // 0
    RwTemplateDescriptor*               m_descriptor;           // 16

    // Plugin registry functions.
    inline void CloneObject( RwTemplate *dst, const RwTemplate *src )
    {
        for ( RwTemplateDescriptor *info = m_descriptor; info != NULL; info = info->m_next )
        {
            // Copy constructors have to be defined for every plugin.
            info->m_copyConstructor( dst, src, info->m_offset, info->m_pluginId );
        }
    }
};

// Macro to get a plugin struct from a RenderWare object.
template <typename structType, typename rwobjType>
structType* RW_PLUGINSTRUCT( rwobjType *obj, size_t pluginOffset )
{
    return (structType*)( (char*)obj + pluginOffset );
}

#endif //__RENDERWARE_COMPAT
