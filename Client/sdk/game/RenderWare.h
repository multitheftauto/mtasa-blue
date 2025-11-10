/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/RenderWare.h
 *  PURPOSE:     RenderWare-compatible definitions for
 *               Grand Theft Auto: San Andreas
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *  RenderWare is © Criterion Software
 *
 *****************************************************************************/

#pragma once

/*****************************************************************************/
/** RenderWare rendering types                                              **/
/*****************************************************************************/

// RenderWare definitions
#if (!defined(RWFORCEENUMSIZEINT))
    #define RWFORCEENUMSIZEINT ((std::int32_t)((~((std::uint32_t)0)) >> 1))
#endif
#define RWPLUGINOFFSET(_type, _base, _offset) ((_type*)((std::uint8_t*)(_base) + (_offset)))
#define RWSRCGLOBAL(variable)                 ((*(RwGlobals**)0xC97B24)->variable) // 0xC97B24 = RwEngineInstance pointer
#define RWRSTATE(a)                           (reinterpret_cast<void*>(a))
#define RW_STRUCT_ALIGN           ((int)((~((unsigned int)0))>>1))
#define RW_TEXTURE_NAME_LENGTH    32
#define RW_FRAME_NAME_LENGTH      23
#define RW_MAX_TEXTURE_COORDS     8

#define RwRenderStateGet(_state, _value) RwRenderStateGetMacro(_state, _value)
#define RwRenderStateSet(_state, _value) RwRenderStateSetMacro(_state, _value)

#define RwRenderStateGetMacro(_state, _value) (RWSRCGLOBAL(dOpenDevice).fpRenderStateGet(_state, _value))
#define RwRenderStateSetMacro(_state, _value) (RWSRCGLOBAL(dOpenDevice).fpRenderStateSet(_state, _value))

/* Type IDs */

#define RP_TYPE_ATOMIC 1
#define RP_TYPE_CLUMP  2

typedef struct RwV2d                RwV2d;
typedef struct RwV3d                RwV3d;
typedef struct RwPlane              RwPlane;
typedef struct RwBBox               RwBBox;
typedef struct RpGeometry           RpGeometry;
typedef void                        RpWorld;
typedef struct RpClump              RpClump;
typedef struct RwRaster             RwRaster;
typedef struct RpMaterialLighting   RpMaterialLighting;
typedef struct RpMaterialList       RpMaterialList;
typedef struct RpMaterial           RpMaterial;
typedef struct RpTriangle           RpTriangle;
typedef struct RwTextureCoordinates RwTextureCoordinates;
typedef struct RwColor              RwColor;
typedef struct RwColorFloat         RwColorFloat;
typedef struct RwObjectFrame        RwObjectFrame;
typedef struct RpAtomic             RpAtomic;
typedef struct RwCamera             RwCamera;
typedef struct RpLight              RpLight;

typedef RwCamera* (*RwCameraPreCallback)(RwCamera* camera);
typedef RwCamera* (*RwCameraPostCallback)(RwCamera* camera);
typedef RpAtomic* (*RpAtomicCallback)(RpAtomic* atomic);
typedef RpClump* (*RpClumpCallback)(RpClump* clump, void* data);

// RenderWare primitive types
struct RwV2d
{            // 8-byte
    float x, y;
};
struct RwV3d
{            // 12-byte
    float x, y, z;
};
struct RwPlane
{
    RwV3d normal;
    float length;
};
struct RwBBox
{
    RwV3d max;
    RwV3d min;
};
struct RwSphere
{
    RwV3d position;
    float radius;
};

struct RwMatrix
{
    /* These are padded to be 16 byte quantities per line */
    RwV3d         right;
    std::uint32_t flags;
    RwV3d         up;
    std::uint32_t pad1;
    RwV3d         at;
    std::uint32_t pad2;
    RwV3d         pos;
    std::uint32_t pad3;
};

struct RtQuat
{
    RwV3d imag; /**< The imaginary part(s) */
    float real; /**< The real part */
};

struct RwFrame;
struct RtAnimInterpolator;

struct RpHAnimNodeInfo
{
    int      nodeID;    /**< User defined ID for this node  */
    int      nodeIndex; /**< Array index of node  */
    int      flags;     /**< Matrix push/pop flags  */
    RwFrame* pFrame;    /**< Pointer to an attached RwFrame (see \ref RpHAnimHierarchyAttach) */
};

struct RpHAnimHierarchy
{
    int flags;    /**< Flags for the hierarchy  */
    int numNodes; /**< Number of nodes in the hierarchy  */

    RwMatrix* pMatrixArray;             /**< Pointer to node matrices*/
    void*     pMatrixArrayUnaligned;    /**< Pointer to memory used for node matrices
                                         * from which the aligned pMatrixArray is allocated */
    RpHAnimNodeInfo* pNodeInfo;         /**< Array of node information (push/pop flags etc) */
    RwFrame*         parentFrame;       /**< Pointer to the Root RwFrame of the hierarchy this
                                         * RpHAnimHierarchy represents */
    RpHAnimHierarchy* parentHierarchy;  /**< Internal use */
    int               rootParentOffset; /**< Internal use */

    RtAnimInterpolator* currentAnim; /**< Internal use */
};

// RenderWare enumerations
enum RwOpCombineType
{
    rwCOMBINEREPLACE = 0, /**<Replace -
                              all previous transformations are lost */
    rwCOMBINEPRECONCAT,   /**<Pre-concatenation -
                              the given transformation is applied
                              before all others */
    rwCOMBINEPOSTCONCAT,  /**<Post-concatenation -
                              the given transformation is applied
                              after all others */
    rwOPCOMBINETYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};

enum RwPrimitiveType
{
    PRIMITIVE_NULL = 0,
    PRIMITIVE_LINE_SEGMENT = 1,
    PRIMITIVE_LINE_SEGMENT_CONNECTED = 2,
    PRIMITIVE_TRIANGLE = 3,
    PRIMITIVE_TRIANGLE_STRIP = 4,
    PRIMITIVE_TRIANGLE_FAN = 5,
    PRIMITIVE_POINT = 6,
    PRIMITIVE_LAST = RW_STRUCT_ALIGN
};
enum RwCameraType
{
    RW_CAMERA_NULL = 0,
    RW_CAMERA_PERSPECTIVE = 1,
    RW_CAMERA_ORTHOGRAPHIC = 2,
    RW_CAMERA_LAST = RW_STRUCT_ALIGN
};
enum RpAtomicFlags
{
    ATOMIC_COLLISION = 1,
    ATOMIC_VISIBLE = 4,
    ATOMIC_LAST = RW_STRUCT_ALIGN
};
enum RwRasterLockFlags
{
    RASTER_LOCK_WRITE = 1,
    RASTER_LOCK_READ = 2,
    RASTER_LOCK_LAST = RW_STRUCT_ALIGN
};
enum RwTransformOrder
{
    TRANSFORM_INITIAL = 0,
    TRANSFORM_BEFORE = 1,
    TRANSFORM_AFTER = 2,
    TRANSFORM_LAST = RW_STRUCT_ALIGN
};
enum RpLightType
{
    LIGHT_TYPE_NULL = 0,

    LIGHT_TYPE_DIRECTIONAL = 1,
    LIGHT_TYPE_AMBIENT = 2,

    LIGHT_TYPE_POINT = 0x80,
    LIGHT_TYPE_SPOT_1 = 0x81,
    LIGHT_TYPE_SPOT_2 = 0x82,

    LIGHT_TYPE_LAST = RW_STRUCT_ALIGN
};
enum RpLightFlags
{
    LIGHT_ILLUMINATES_ATOMICS = 1,
    LIGHT_ILLUMINATES_GEOMETRY = 2,
    LIGHT_FLAGS_LAST = RW_STRUCT_ALIGN
};

// RenderWare/plugin base types
struct RwObject
{
    unsigned char type;
    unsigned char subtype;
    unsigned char flags;
    unsigned char privateFlags;
    void*         parent;            // should be RwFrame with RpClump
};
struct RwVertex
{
    RwV3d        position;
    RwV3d        normal;
    unsigned int color;
    float        u, v;
};
struct RwListEntry
{
    RwListEntry *next, *prev;
};
struct RwList
{
    RwListEntry root;
};
struct RwFrame
{
    RwObject        object;                 // 0
    void *          pad1, *pad2;            // 8
    RwMatrix        modelling;              // 16
    RwMatrix        ltm;                    // 32
    RwList          objects;                // 48
    struct RwFrame* child;                  // 56
    struct RwFrame* next;                   // 60
    struct RwFrame* root;                   // 64

    // Rockstar Frame extension (0x253F2FE) (24 bytes)
    unsigned char pluginData[8];                               // padding
    char          szName[RW_FRAME_NAME_LENGTH + 1];            // name (as stored in the frame extension)
};
struct RwTexDictionary
{
    RwObject    object;
    RwList      textures;
    RwListEntry globalTXDs;
};
struct RwTexture
{
    RwRaster*        raster;
    RwTexDictionary* txd;
    RwListEntry      TXDList;
    char             name[RW_TEXTURE_NAME_LENGTH];
    char             mask[RW_TEXTURE_NAME_LENGTH];
    unsigned int     flags;
    int              refs;
};
struct RwTextureCoordinates
{
    float u, v;
};
struct RwRaster
{
    RwRaster*      parent;                          // 0
    unsigned char* pixels;                          // 4
    unsigned char* palette;                         // 8
    int            width, height, depth;            // 12, 16 / 0x10, 20
    int            numLevels;                       // 24 / 0x18
    short          u, v;
    unsigned char  type;
    unsigned char  flags;
    unsigned char  privateFlags;
    unsigned char  format;
    unsigned char* origPixels;
    int            origWidth, origHeight, origDepth;
    void*          renderResource;            // RwD3D9Raster continues from here
};
struct RwColorFloat
{
    float r, g, b, a;
};
struct RwColor
{
    unsigned char r, g, b, a;
};
struct RwObjectFrame
{
    RwObject    object;
    RwListEntry lFrame;
    void*       callback;
};
struct RwCameraFrustum
{
    RwPlane       plane;
    unsigned char x, y, z;
    unsigned char unknown1;
};
struct RwCamera
{
    RwObjectFrame        object;
    RwCameraType         type;
    RwCameraPreCallback  preCallback;
    RwCameraPostCallback postCallback;
    RwMatrix             matrix;
    RwRaster*            bufferColor;
    RwRaster*            bufferDepth;
    RwV2d                screen;
    RwV2d                screenInverse;
    RwV2d                screenOffset;
    float                nearplane;
    float                farplane;
    float                fog;
    float                unknown1;
    float                unknown2;
    RwCameraFrustum      frustum4D[6];
    RwBBox               viewBBox;
    RwV3d                frustum3D[8];
};
struct RwGeometry
{
    unsigned char  unknown1[14];
    unsigned short refs;
};

/* Interpolator flags */
enum RpInterpolatorFlag : int32_t
{
    rpINTERPOLATORDIRTYINSTANCE = 0x01,
    rpINTERPOLATORDIRTYSPHERE = 0x02,
    rpINTERPOLATORNOFRAMEDIRTY = 0x04,
};
struct RpInterpolator
{
    int32_t flags;
    int16_t startMorphTarget;
    int16_t endMorphTarget;
    float   time;
    float   recipTime;
    float   position;
};

struct RpAtomic
{
    RwObjectFrame    object;
    void*            info;
    RpGeometry*      geometry;
    RwSphere         boundingSphere;
    RwSphere         worldBoundingSphere;
    RpClump*         clump;
    RwListEntry      globalClumps;
    RpAtomicCallback renderCallback;
    RpInterpolator   interpolator;
    unsigned short   frame;
    unsigned short   unknown7;
    RwList           sectors;
    void*            render;
};
struct RpAtomicContainer
{
    RpAtomic* atomic;
    char      szName[17];
};
struct RpLight
{
    RwObjectFrame  object;
    float          radius;
    RwColorFloat   color;
    float          unknown1;
    RwList         sectors;
    RwListEntry    globalLights;
    unsigned short frame;
    unsigned short unknown2;
};
struct RpClump
{            // RenderWare (plugin) Clump (used by GTA)
    RwObject        object;
    RwList          atomics;
    RwList          lights;
    RwList          cameras;
    RwListEntry     globalClumps;
    RpClumpCallback callback;
};
struct RpMaterialLighting
{
    float ambient, specular, diffuse;
};
struct RpMaterial
{
    RwTexture*         texture;
    RwColor            color;
    void*              render;
    RpMaterialLighting lighting;
    short              refs;
    short              id;
};
struct RpMaterials
{
    RpMaterial** materials;
    int          entries;
    int          unknown;
};
struct RpTriangle
{
    unsigned short verts[3];
    unsigned short materialId;
};
struct RpMorphTarget
{
    RpGeometry* parentGeom;
    RwSphere   boundingSphere;
    RwV3d* verts;
    RwV3d* normals;
};
struct RpGeometry
{
    RwObject       object;
    unsigned int   flags;
    unsigned short unknown1;
    short          refs;

    int triangles_size;
    int vertices_size;
    int unknown_size;
    int texcoords_size;

    RpMaterials           materials;
    RpTriangle*           triangles;
    RwColor*              colors;
    RwTextureCoordinates* texcoords[RW_MAX_TEXTURE_COORDS];
    void*                 unknown2;
    void*                 info;
    RpMorphTarget*        morph_target;
};

inline auto rwObjectGetParent(RwObject* o) {
    return (RwObject*)o->parent;
}

inline auto RpAtomicGetFrame(RpAtomic* atomic) {
    return (RwFrame*)atomic->object.object.parent;
}

inline auto RwFrameGetParent(RwFrame* f) {
    return (RwFrame*)rwObjectGetParent((RwObject*)f);
}

inline RwMatrix* RwFrameGetMatrix(RwFrame* f) {
    return &f->modelling;
}

inline void _rpAtomicResyncInterpolatedSphere(RpAtomic* atomic) {
    reinterpret_cast<void(__cdecl*)(RpAtomic*)>(0x7491F0)(atomic);
}

/* NB "RpAtomicGetBoundingSphere(atomic++)" will break it */
#define RpAtomicGetBoundingSphere(_atomic)                              \
    ((((_atomic)->interpolator.flags & rpINTERPOLATORDIRTYSPHERE)?      \
      _rpAtomicResyncInterpolatedSphere(_atomic), 0: 0),                \
      &((_atomic)->boundingSphere))

/*****************************************************************************/
/** RenderWare I/O                                                          **/
/*****************************************************************************/

// RenderWare type definitions
typedef int (*RwIOCallbackClose)(void* data);
typedef unsigned int (*RwIOCallbackRead)(void* data, void* buffer, unsigned int length);
typedef int (*RwIOCallbackWrite)(void* data, const void* buffer, unsigned int length);
typedef int (*RwIOCallbackOther)(void* data, unsigned int offset);

// RenderWare enumerations
enum RwStreamType
{
    STREAM_TYPE_NULL = 0,
    STREAM_TYPE_FILE = 1,
    STREAM_TYPE_FILENAME = 2,
    STREAM_TYPE_BUFFER = 3,
    STREAM_TYPE_CALLBACK = 4,
    STREAM_TYPE_LAST = RW_STRUCT_ALIGN
};
enum RwStreamMode
{
    STREAM_MODE_NULL = 0,
    STREAM_MODE_READ = 1,
    STREAM_MODE_WRITE = 2,
    STREAM_MODE_APPEND = 3,
    STREAM_MODE_LAST = RW_STRUCT_ALIGN
};

enum RwRenderState
{
    rwRENDERSTATENARENDERSTATE = 0,

    rwRENDERSTATETEXTURERASTER,
    /**<Raster used for texturing (normally used in immediate mode).
     *  The value is a pointer to an \ref RwRaster.
     * Default: NULL.
     */
    rwRENDERSTATETEXTUREADDRESS,
    /**<\ref RwTextureAddressMode: wrap, clamp, mirror or border.
     * Default: rwTEXTUREADDRESSWRAP.
     */
    rwRENDERSTATETEXTUREADDRESSU,
    /**<\ref RwTextureAddressMode in u only.
     * Default: rwTEXTUREADDRESSWRAP.
     */
    rwRENDERSTATETEXTUREADDRESSV,
    /**<\ref RwTextureAddressMode in v only.
     * Default: rwTEXTUREADDRESSWRAP.
     */
    rwRENDERSTATETEXTUREPERSPECTIVE,
    /**<Perspective correction on/off (always enabled on many platforms).
     */
    rwRENDERSTATEZTESTENABLE,
    /**<Z-buffer test on/off.
     * Default: TRUE.
     */
    rwRENDERSTATESHADEMODE,
    /**<\ref RwShadeMode: flat or gouraud shading.
     * Default: rwSHADEMODEGOURAUD.
     */
    rwRENDERSTATEZWRITEENABLE,
    /**<Z-buffer write on/off.
     * Default: TRUE.
     */
    rwRENDERSTATETEXTUREFILTER,
    /**<\ref RwTextureFilterMode: point sample, bilinear, trilinear, etc.
     * Default: rwFILTERLINEAR.
     */
    rwRENDERSTATESRCBLEND,
    /**<\ref RwBlendFunction used to modulate the source pixel color
     *  when blending to the frame buffer.
     * Default: rwBLENDSRCALPHA.
     */
    rwRENDERSTATEDESTBLEND,
    /**<\ref RwBlendFunction used to modulate the destination pixel
     *  color in the frame buffer when blending. The resulting pixel
     *  color is given by the formula
     *  (SRCBLEND * srcColor + DESTBLEND * destColor) for each RGB
     *  component. For a particular platform, not all combinations
     *  of blend function are allowed (see platform specific
     *  restrictions).
     * Default: rwBLENDINVSRCALPHA.
     */
    rwRENDERSTATEVERTEXALPHAENABLE,
    /**<Alpha blending on/off (always enabled on some platforms).
     *  This is normally used in immediate mode to enable alpha blending
     *  when vertex colors or texture rasters have transparency. Retained
     *  mode pipelines will usually set this state based on material colors
     *  and textures.
     * Default: FALSE.
     */
    rwRENDERSTATEBORDERCOLOR,
    /**<Border color for \ref RwTextureAddressMode
     *  \ref rwTEXTUREADDRESSBORDER. The value should be a packed
     *  RwUInt32 in a platform specific format. The macro
     *  RWRGBALONG(r, g, b, a) may be used to construct this using
     *  8-bit color components.
     * Default: RWRGBALONG(0, 0, 0, 0).
     */
    rwRENDERSTATEFOGENABLE,
    /**<Fogging on/off (all polygons will be fogged).
     * Default: FALSE.
     */
    rwRENDERSTATEFOGCOLOR,
    /**<Color used for fogging. The value should be a packed RwUInt32
     *  in a platform specific format. The macro RWRGBALONG(r, g, b, a)
     *  may be used to construct this using 8-bit color components.
     * Default: RWRGBALONG(0, 0, 0, 0).
     */
    rwRENDERSTATEFOGTYPE,
    /**<\ref RwFogType, the type of fogging to use.
     * Default: rwFOGTYPELINEAR.
     */
    rwRENDERSTATEFOGDENSITY,
    /**<Fog density for \ref RwFogType of
     *  \ref rwFOGTYPEEXPONENTIAL or \ref rwFOGTYPEEXPONENTIAL2.
     *  The value should be a pointer to an RwReal in the
     *  range 0 to 1.
     * Default: 1.
     */
    rwRENDERSTATECULLMODE = 20,
    /**<\ref RwCullMode, for selecting front/back face culling, or
     *  no culling.
     * Default: rwCULLMODECULLBACK.
     */
    rwRENDERSTATESTENCILENABLE,
    /**<Stenciling on/off.
     *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
     * Default: FALSE.
     */
    rwRENDERSTATESTENCILFAIL,
    /**<\ref RwStencilOperation used when the stencil test passes.
     *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
     * Default: rwSTENCILOPERATIONKEEP.
     */
    rwRENDERSTATESTENCILZFAIL,
    /**<\ref RwStencilOperation used when the stencil test passes and
     *  the depth test (z-test) fails.
     *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
     * Default: rwSTENCILOPERATIONKEEP.
     */
    rwRENDERSTATESTENCILPASS,
    /**<\ref RwStencilOperation used when both the stencil and the depth
     *  (z) tests pass.
     *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
     * Default: rwSTENCILOPERATIONKEEP.
     */
    rwRENDERSTATESTENCILFUNCTION,
    /**<\ref RwStencilFunction for the stencil test.
     *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
     * Default: rwSTENCILFUNCTIONALWAYS.
     */
    rwRENDERSTATESTENCILFUNCTIONREF,
    /**<Integer reference value for the stencil test.
     *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
     * Default: 0.
     */
    rwRENDERSTATESTENCILFUNCTIONMASK,
    /**<Mask applied to the reference value and each stencil buffer
     *  entry to determine the significant bits for the stencil test.
     *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
     * Default: 0xffffffff.
     */
    rwRENDERSTATESTENCILFUNCTIONWRITEMASK,
    /**<Write mask applied to values written into the stencil buffer.
     *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
     * Default: 0xffffffff.
     */
    rwRENDERSTATEALPHATESTFUNCTION,
    /**<\ref RwAlphaTestFunction for the alpha test. When a pixel fails,
     * neither the frame buffer nor the Z-buffer are updated.
     * Default: rwALPHATESTFUNCTIONGREATER (GameCube, Xbox, D3D8, D3D9
     * and OpenGL). The default PS2 behaviour is to always update the
     * frame buffer and update the Z-buffer only if a greater than or
     * equal test passes.
     */
    rwRENDERSTATEALPHATESTFUNCTIONREF,
    /**<Integer reference value for the alpha test.
     *  <i> Range is 0 to 255, mapped to the platform's actual range </i>
     * Default: 128 (PS2) 0 (GameCube, Xbox, D3D8, D3D9 and OpenGL).
     */

    rwRENDERSTATEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};

enum RwCullMode
{
    rwCULLMODENACULLMODE = 0,

    rwCULLMODECULLNONE,
    /**<Both front and back-facing triangles are drawn. */
    rwCULLMODECULLBACK,
    /**<Only front-facing triangles are drawn */
    rwCULLMODECULLFRONT,
    /**<Only back-facing triangles are drawn */

    rwCULLMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};

// RenderWare base types
struct RwBuffer
{
    void*        ptr;
    unsigned int size;
};
union RwStreamTypeData
{
    struct
    {
        unsigned int position;
        unsigned int size;
        void*        ptr_file;
    };
    struct
    {
        void* file;
    };
    struct
    {
        RwIOCallbackClose callbackClose;
        RwIOCallbackRead  callbackRead;
        RwIOCallbackWrite callbackWrite;
        RwIOCallbackOther callbackOther;
        void*             ptr_callback;
    };
};
struct RwStream
{
    RwStreamType     type;
    RwStreamMode     mode;
    int              pos;
    RwStreamTypeData data;
    int              id;
};
struct RwError
{
    int err1, err2;
};

/*****************************************************************************/
/** RenderWare Globals                                                      **/
/*****************************************************************************/

typedef bool (*RwSystemFunc)(std::int32_t, void*, void*, std::int32_t);
using RwRenderStateSetFunction = bool(*)(RwRenderState, void*);
using RwRenderStateGetFunction = bool(*)(RwRenderState, void*);

struct RwDevice
{
    float        gammaCorrection;
    RwSystemFunc fpSystem;
    float        zBufferNear;
    float        zBufferFar;
    RwRenderStateSetFunction             fpRenderStateSet;
    RwRenderStateGetFunction             fpRenderStateGet;
    // RwIm2DRenderLineFunction             fpIm2DRenderLine;
    // RwIm2DRenderTriangleFunction         fpIm2DRenderTriangle;
    // RwIm2DRenderPrimitiveFunction        fpIm2DRenderPrimitive;
    // RwIm2DRenderIndexedPrimitiveFunction fpIm2DRenderIndexedPrimitive;
    // RwIm3DRenderLineFunction             fpIm3DRenderLine;
    // RwIm3DRenderTriangleFunction         fpIm3DRenderTriangle;
    // RwIm3DRenderPrimitiveFunction        fpIm3DRenderPrimitive;
    // RwIm3DRenderIndexedPrimitiveFunction fpIm3DRenderIndexedPrimitive;
};
// static_assert(sizeof(RwDevice) == 0x38, "Incorrect class size: RwDevice");

typedef bool (*RwStandardFunc)(void*, void*, std::int32_t);
struct RwGlobals
{
    void*               curCamera;
    void*               curWorld;
    std::uint16_t       renderFrame;
    std::uint16_t       lightFrame;
    std::uint16_t       pad[2];
    RwDevice            dOpenDevice;
    RwStandardFunc      stdFunc[29];
    // RwLinkList          dirtyFrameList;
    // RwFileFunctions     fileFuncs;
    // RwStringFunctions   stringFuncs;
    // RwMemoryFunctions   memoryFuncs;
    // RwMemoryAllocFn     memoryAlloc;
    // RwMemoryFreeFn      memoryFree;
    // RwMetrics*          metrics;
    // RwEngineStatus      engineStatus;
    // RwUInt32            resArenaInitSize;
};
//static_assert(sizeof(RwGlobals) == 0x158, "Incorrect class size: RwGlobals");
