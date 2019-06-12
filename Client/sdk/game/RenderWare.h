/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/RenderWare.h
 *  PURPOSE:     RenderWare-compatible definitions for
 *               Grand Theft Auto: San Andreas
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *  RenderWare is © Criterion Software
 *
 *****************************************************************************/

#pragma once

#include <d3d9.h>


/*****************************************************************************/
/** RenderWare rendering types                                              **/
/*****************************************************************************/


// RenderWare definitions
#if (!defined(RWFORCEENUMSIZEINT))
#define RWFORCEENUMSIZEINT ((int)((~((unsigned int)0))>>1))
#endif /* (!defined(RWFORCEENUMSIZEINT)) */

#define RASTEREXTFROMRASTER(raster) \
((_rwD3D9RasterExt *)(((RwUInt8 *)(raster)) + CRenderWareSA::_RwD3D9RasterExtOffset))
#define RW_STRUCT_ALIGN           ((int)((~((unsigned int)0))>>1))
#define RW_TEXTURE_NAME_LENGTH    32
#define RW_FRAME_NAME_LENGTH      23
#define RW_MAX_TEXTURE_COORDS     8

typedef float  RwReal;
typedef long RwFixed;
typedef int  RwInt32;
typedef unsigned int RwUInt32;
typedef short RwInt16;
typedef unsigned short RwUInt16;
typedef unsigned char RwUInt8;
typedef signed char RwInt8;

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


typedef struct _rwD3D9Palette _rwD3D9Palette;
struct _rwD3D9Palette
{
    PALETTEENTRY    entries[256];
    RwInt32     globalindex;
};

typedef LPDIRECT3DSURFACE9 LPSURFACE;
typedef LPDIRECT3DTEXTURE9 LPTEXTURE;

typedef struct _rwD3D9RasterExt _rwD3D9RasterExt;
struct _rwD3D9RasterExt
{
    LPTEXTURE               texture;
    _rwD3D9Palette          *palette;
    RwUInt8                 alpha;              // This texture has alpha 
    RwUInt8                 cube : 4;           // This texture is a cube texture 
    RwUInt8                 face : 4;           // The active face of a cube texture 
    RwUInt8                 automipmapgen : 4;  // This texture uses automipmap generation 
    RwUInt8                 compressed : 4;     // This texture is compressed 
    RwUInt8                 lockedMipLevel;
    LPSURFACE               lockedSurface;
    D3DLOCKED_RECT          lockedRect;
    D3DFORMAT               d3dFormat;          // D3D format 
    LPDIRECT3DSWAPCHAIN9    swapChain;
    HWND                    window;
};

typedef struct _rwD3D9RasterConvData _rwD3D9RasterConvData;
struct _rwD3D9RasterConvData
{
    D3DFORMAT   format;
    RwUInt8     depth;
    RwUInt8     alpha;
};

typedef struct _rwD3D9FormatInfo _rwD3D9FormatInfo;
struct _rwD3D9FormatInfo
{
    RwUInt8 alpha;
    RwUInt8 depth;
    RwUInt16 rwFormat;
};

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
{                                  // 16-byte padded
    RwV3d        right;            // 0
    unsigned int flags;            // 12
    RwV3d        up;               // 16
    unsigned int pad1;             // 28
    RwV3d        at;               // 32
    unsigned int pad2;             // 44
    RwV3d        pos;              // 48
    unsigned int pad3;             // 60
};

// RenderWare enumerations
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

/**
* \ingroup rwraster
* \ref RwRasterLockMode represents the options available for locking
* a raster so that it may be modified (see API function \ref RwRasterLock). An
* application may wish to write to the raster, read from the raster or
* simultaneously write and read a raster (rwRASTERLOCKWRITE | rwRASTERLOCKREAD).
*/
enum RwRasterLockMode
{
    rwRASTERLOCKWRITE = 0x01,   /**<Lock for writing */
    rwRASTERLOCKREAD = 0x02,    /**<Lock for reading */
    rwRASTERLOCKNOFETCH = 0x04, /**<When used in combination with
                                *  rwRASTERLOCKWRITE, asks the driver not to
                                *  fetch the pixel data. This is only useful
                                *  if it is known that ALL the raster data is
                                *  going to be overwritten before the raster
                                *  is unlocked, i.e. from an
                                *  \ref RwRasterSetFromImage call. This flag
                                *  is not supported by all drivers. */
    rwRASTERLOCKRAW = 0x08,    /**<When used in combination with
                               rwRASTERLOCKWRITE or rwRASTERLOCKREAD
                               allows access to the raw platform specific
                               pixel format */
    rwRASTERLOCKMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};

/**
* \ingroup rwraster
*  RwRasterType
*  This type represents the options available for creating a new
* raster (se API function \ref RwRasterCreate)*/
enum RwRasterType
{
    rwRASTERTYPENORMAL = 0x00,          /**<Normal */
    rwRASTERTYPEZBUFFER = 0x01,         /**<Z Buffer */
    rwRASTERTYPECAMERA = 0x02,          /**<Camera */
    rwRASTERTYPETEXTURE = 0x04,         /**<Texture */
    rwRASTERTYPECAMERATEXTURE = 0x05,   /**<Camera texture */
    rwRASTERTYPEMASK = 0x07,            /**<Mask for finding type */

    rwRASTERPALETTEVOLATILE = 0x40,        /**<If set, hints that the palette will change often */
    rwRASTERDONTALLOCATE = 0x80,        /**<If set the raster is not allocated */
    rwRASTERTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwRasterType RwRasterType;

/**
* \ingroup rwraster
* \ref RwRasterFormat is a set of values and flags which may be combined to
* specify a raster format. The format chosen for a particular raster depends
* on the hardware device and the raster type specified at creation time
* (see API function \ref RwRasterCreate). The format may be retrieved using
* API function \ref RwRasterGetFormat.
*
* The raster format is a packed set of bits which contains the following
* four pieces of information (these may be combined with bitwise OR):
*
* <ol>
* <li> The pixel color format corresponding to one of the following values:
*      <ul>
*      <li> rwRASTERFORMAT1555
*      <li> rwRASTERFORMAT565
*      <li> rwRASTERFORMAT4444
*      <li> rwRASTERFORMATLUM8
*      <li> rwRASTERFORMAT8888
*      <li> rwRASTERFORMAT888
*      <li> rwRASTERFORMAT16
*      <li> rwRASTERFORMAT24
*      <li> rwRASTERFORMAT32
*      <li> rwRASTERFORMAT555
*      </ul>
*      This value may be masked out of the raster format using
*      rwRASTERFORMATPIXELFORMATMASK.
* <li> The palette depth if the raster is palettized:
*      <ul>
*      <li> rwRASTERFORMATPAL4
*      <li> rwRASTERFORMATPAL8
*      </ul>
*      In these cases, the color format refers to that of the palette.
* <li> Flag rwRASTERFORMATMIPMAP. Set if the raster contains mipmap levels.
* <li> Flag rwRASTERFORMATAUTOMIPMAP. Set if the mipmap levels were generated
*      automatically by RenderWare.
* </ol>
*/
enum RwRasterFormat
{
    rwRASTERFORMATDEFAULT = 0x0000, /* Whatever the hardware likes best */

    rwRASTERFORMAT1555 = 0x0100,    /**<16 bits - 1 bit alpha, 5 bits red, green and blue */
    rwRASTERFORMAT565 = 0x0200,     /**<16 bits - 5 bits red and blue, 6 bits green */
    rwRASTERFORMAT4444 = 0x0300,    /**<16 bits - 4 bits per component */
    rwRASTERFORMATLUM8 = 0x0400,    /**<Gray scale */
    rwRASTERFORMAT8888 = 0x0500,    /**<32 bits - 8 bits per component */
    rwRASTERFORMAT888 = 0x0600,     /**<24 bits - 8 bits per component */
    rwRASTERFORMAT16 = 0x0700,      /**<16 bits - undefined: useful for things like Z buffers */
    rwRASTERFORMAT24 = 0x0800,      /**<24 bits - undefined: useful for things like Z buffers */
    rwRASTERFORMAT32 = 0x0900,      /**<32 bits - undefined: useful for things like Z buffers */
    rwRASTERFORMAT555 = 0x0a00,     /**<16 bits - 5 bits red, green and blue */

    rwRASTERFORMATAUTOMIPMAP = 0x1000, /**<RenderWare generated the mip levels */

    rwRASTERFORMATPAL8 = 0x2000,    /**<8 bit palettised */
    rwRASTERFORMATPAL4 = 0x4000,    /**<4 bit palettised */

    rwRASTERFORMATMIPMAP = 0x8000,  /**<Mip mapping on */

    rwRASTERFORMATPIXELFORMATMASK = 0x0f00, /**<The pixel color format
                                            *  (excluding palettised bits) */
    rwRASTERFORMATMASK = 0xff00     /**<The whole format */,
    rwRASTERFORMATFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwRasterFormat RwRasterFormat;

/**
* \ingroup rpgeometry
* RpGeometryLockMode
* Geometry lock flags
*/
enum RpGeometryLockMode
{
    rpGEOMETRYLOCKPOLYGONS = 0x01, /**<Lock the polygons (triangle list) */
    rpGEOMETRYLOCKVERTICES = 0x02, /**<Lock the vertex positional data */
    rpGEOMETRYLOCKNORMALS = 0x04, /**<Lock the vertex normal data */
    rpGEOMETRYLOCKPRELIGHT = 0x08, /**<Lock the pre-light values */
    rpGEOMETRYLOCKTEXCOORDS = 0x10, /**<Lock the texture coordinates set 1*/
    rpGEOMETRYLOCKTEXCOORDS1 = 0x10, /**<Lock the texture coordinates set 1*/
    rpGEOMETRYLOCKTEXCOORDS2 = 0x20, /**<Lock the texture coordinates set 2*/
    rpGEOMETRYLOCKTEXCOORDS3 = 0x40, /**<Lock the texture coordinates set 3*/
    rpGEOMETRYLOCKTEXCOORDS4 = 0x80, /**<Lock the texture coordinates set 4*/
    rpGEOMETRYLOCKTEXCOORDS5 = 0x0100, /**<Lock the texture coordinates set 5*/
    rpGEOMETRYLOCKTEXCOORDS6 = 0x0200, /**<Lock the texture coordinates set 6*/
    rpGEOMETRYLOCKTEXCOORDS7 = 0x0400, /**<Lock the texture coordinates set 7*/
    rpGEOMETRYLOCKTEXCOORDS8 = 0x0800, /**<Lock the texture coordinates set 8*/
    rpGEOMETRYLOCKTEXCOORDSALL = 0x0ff0, /**<Lock all texture coordinate sets*/
    rpGEOMETRYLOCKALL = 0x0fff, /**<Combination of all the above */

    rpGEOMETRYLOCKMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpGeometryLockMode RpGeometryLockMode;

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

struct RwSurfaceProperties
{
    RwReal ambient;   /**< ambient reflection coefficient */
    RwReal specular;  /**< specular reflection coefficient */
    RwReal diffuse;   /**< reflection coefficient */
};

struct RwRGBA
{
    RwUInt8 red;    /**< red component */
    RwUInt8 green;  /**< green component */
    RwUInt8 blue;   /**< blue component */
    RwUInt8 alpha;  /**< alpha component */
};

struct RwRaster
{
    RwRaster           *parent;               /* Top level raster if a sub raster */
    RwUInt8            *cpPixels;             /* Pixel pointer when locked */
    RwUInt8            *palette;              /* Raster palette */
    RwInt32             width, height, depth; /* Dimensions of raster */
    RwInt32             stride;               /* Lines bytes of raster */
    RwInt16             nOffsetX, nOffsetY;   /* Sub raster offset */
    RwUInt8             cType;                /* Type of raster */
    RwUInt8             cFlags;               /* Raster flags */
    RwUInt8             privateFlags;         /* Raster private flags */
    RwUInt8             cFormat;              /* Raster format */

    RwUInt8            *originalPixels;
    RwInt32             originalWidth;
    RwInt32             originalHeight;
    RwInt32             originalStride;
    void*               renderResource;     // RwD3D9Raster continues from here
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
struct RpInterpolation
{
    unsigned int unknown1;
    unsigned int unknown2;
    float        unknown3;
    float        unknown4;
    float        unknown5;
};
struct RpAtomic
{
    RwObjectFrame    object;
    void*            info;
    RpGeometry*      geometry;
    RwSphere         bsphereLocal;
    RwSphere         bsphereWorld;
    RpClump*         clump;
    RwListEntry      globalClumps;
    RpAtomicCallback renderCallback;
    RpInterpolation  interpolation;
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
    unsigned short vertIndex[3];
    unsigned short matIndex;
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
    void*                 unknown3;
};

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

// RenderWare base types
struct RwBuffer
{
    void*        ptr;
    unsigned int size;
};
union RwStreamTypeData {
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
/** RenderWare Plugins (extensions)                                         **/
/*****************************************************************************/

typedef struct RwPluginRegistry RwPluginRegistry;
typedef struct RwPluginRegEntry RwPluginRegEntry;

struct RwPluginRegistry
{
    unsigned int      sizeOfStruct;
    unsigned int      origSizeOfStruct;
    unsigned int      maxSizeOfStruct;
    unsigned int      staticAlloc;
    RwPluginRegEntry *firstRegEntry;
    RwPluginRegEntry *lastRegEntry;
};

typedef RwStream *(*RwPluginDataChunkWriteCallBack)(RwStream *stream, int binaryLength, const void *object, int offsetInObject, int sizeInObject);
typedef RwStream *(*RwPluginDataChunkReadCallBack)(RwStream *stream, int binaryLength, void *object, int offsetInObject, int sizeInObject);
typedef int(*RwPluginDataChunkGetSizeCallBack)(const void *object, int offsetInObject, int sizeInObject);
typedef bool(*RwPluginDataChunkAlwaysCallBack)(void *object, int offsetInObject, int sizeInObject);
typedef bool(*RwPluginDataChunkRightsCallBack)(void *object, int offsetInObject, int sizeInObject, int extraData);
typedef void *(*RwPluginObjectConstructor)(void *object, int offsetInObject, int sizeInObject);
typedef void *(*RwPluginObjectCopy)(void *dstObject, const void *srcObject, int offsetInObject, int sizeInObject);
typedef void *(*RwPluginObjectDestructor)(void *object, int offsetInObject, int sizeInObject);
typedef void *(*RwPluginErrorStrCallBack)(void *);

struct RwPluginRegEntry
{
    int             offset;
    int             size;
    unsigned int    pluginID;
    RwPluginDataChunkReadCallBack readCB;
    RwPluginDataChunkWriteCallBack writeCB;
    RwPluginDataChunkGetSizeCallBack getSizeCB;
    RwPluginDataChunkAlwaysCallBack alwaysCB;
    RwPluginDataChunkRightsCallBack rightsCB;
    RwPluginObjectConstructor constructCB;
    RwPluginObjectDestructor destructCB;
    RwPluginObjectCopy copyCB;
    RwPluginErrorStrCallBack errStrCB;
    RwPluginRegEntry *nextRegEntry;
    RwPluginRegEntry *prevRegEntry;
    RwPluginRegistry *parentRegistry;
};
