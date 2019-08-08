//
// dffapi
// https://github.com/DK22Pac/dffapi
//

#define rwNULL 0
#define rwFALSE 0
#define rwTRUE 1

typedef struct gtaRwGeometryList    gtaRwGeometryList;
typedef struct gtaRwGeometry        gtaRwGeometry;
typedef struct gtaRwAtomic          gtaRwAtomic;
typedef enum gtaRwStreamType        gtaRwStreamType;
typedef enum gtaRwStreamAccessType  gtaRwStreamAccessType;
typedef struct gtaRwStreamMemory    gtaRwStreamMemory;
typedef union gtaRwStreamFile       gtaRwStreamFile;
typedef struct gtaRwStreamCustom    gtaRwStreamCustom;
typedef union gtaRwStreamUnion      gtaRwStreamUnion;
typedef struct gtaRwStream          gtaRwStream;
typedef struct gtaRwMemory          gtaRwMemory;
typedef struct gtaRwChunkHeaderInfo gtaRwChunkHeaderInfo;
typedef struct gtaRwExtension       gtaRwExtension;
typedef struct gtaRwMesh            gtaRwMesh;
typedef struct gtaRwGeometryBinMesh gtaRwGeometryBinMesh;
typedef struct gtaRwBoneIndices     gtaRwBoneIndices;
typedef struct gtaRwBoneWeights     gtaRwBoneWeights;

typedef long gtaRwInt;
typedef unsigned long gtaRwUInt;
typedef int  gtaRwInt32;
typedef unsigned int gtaRwUInt32;
typedef short gtaRwInt16;
typedef unsigned short gtaRwUInt16;
typedef unsigned char gtaRwUInt8;
typedef signed char gtaRwInt8;
typedef char gtaRwChar;
typedef wchar_t gtaRwWideChar;
typedef float gtaRwReal;
typedef gtaRwInt32 gtaRwBool;

typedef enum gtaRwPlatformID          gtaRwPlatformID;
typedef enum gtaRwPluginVendor        gtaRwPluginVendor;
typedef enum gtaRwCorePluginID        gtaRwCorePluginID;
typedef enum gtaRwTextureFilterMode   gtaRwTextureFilterMode;
typedef enum gtaRwTextureAddressMode  gtaRwTextureAddressMode;
typedef enum gtaRwMatFXMaterialFlags  gtaRwMatFXMaterialFlags;
typedef enum gtaRwGeometryFlag        gtaRwGeometryFlag;
typedef enum gtaRwMeshHeaderFlags     gtaRwMeshHeaderFlags;
typedef enum gtaRwRasterFormat        gtaRwRasterFormat;
typedef enum gtaRwAtomicFlag          gtaRwAtomicFlag;
typedef enum gtaGameVersionId         gtaGameVersionId;
typedef enum gtaPlatformId            gtaPlatformId;
typedef struct gtaRwV2d               gtaRwV2d;
typedef struct gtaRwV3d               gtaRwV3d;
typedef struct gtaRwRGBA              gtaRwRGBA;
typedef struct gtaRwSurfaceProperties gtaRwSurfaceProperties;
typedef struct gtaRwTexCoords         gtaRwTexCoords;
typedef struct gtaRwTriangle          gtaRwTriangle;
typedef struct gtaRwSphere            gtaRwSphere;
typedef struct gtaRwMorphTarget       gtaRwMorphTarget;
typedef struct gtaRwMatrix            gtaRwMatrix;
typedef struct gtaRwGeometryNative    gtaRwGeometryNative;
typedef struct gtaRwMaterialList      gtaRwMaterialList;

enum gtaRwPlatformID {
    rwID_PCD3D7 = 1,
    rwID_PCOGL,
    rwID_MAC,
    rwID_PS2,
    rwID_XBOX,
    rwID_GAMECUBE,
    rwID_SOFTRAS,
    rwID_PCD3D8,
    rwID_PCD3D9
};

enum gtaGameVersionId
{
    gtaGAME_GTA3,
    gtaGAME_GTAVC,
    gtaGAME_GTASA,
    gtaGAME_MANHUNT
};

enum gtaPlatformId
{
    PLATFORM_NOTDEFINED,
    PLATFORM_D3D8,
    PLATFORM_D3D9,
    PLATFORM_PS2,
    PLATFORM_XBOX,
    PLATFORM_OGL,
    PLATFORM_MOBILE = PLATFORM_OGL
};

enum gtaCustomIds
{
    gtaID_PIPELINE = 0x253F2F3,
    gtaID_SPECMAP = 0x253F2F6,
    gtaID_2DEFFECT = 0x253F2F8,
    gtaID_EXTRAVERTCOLOUR = 0x253F2F9,
    gtaID_COLLISIONPLUGIN = 0x253F2FA,
    gtaID_ENVMAP = 0x253F2FC,
    gtaID_BREAKABLE = 0x253F2FD,
    gtaID_NODENAME = 0x253F2FE,
};

enum gtaRwPluginVendor {
    rwVENDORID_CORE = 0x000000L,
    rwVENDORID_CRITERIONTK = 0x000001L,
    rwVENDORID_REDLINERACER = 0x000002L,
    rwVENDORID_CSLRD = 0x000003L,
    rwVENDORID_CRITERIONINT = 0x000004L,
    rwVENDORID_CRITERIONWORLD = 0x000005L,
    rwVENDORID_BETA = 0x000006L,
    rwVENDORID_CRITERIONRM = 0x000007L,
    rwVENDORID_CRITERIONRWA = 0x000008L,
    rwVENDORID_CRITERIONRWP = 0x000009L,
};

#define MAKECHUNKID(vendorID, chunkID) ((((vendorID) & 0xFFFFFF) << 8) | ((chunkID) & 0xFF))

enum gtaRwCorePluginID {
    rwID_NAOBJECT = MAKECHUNKID(rwVENDORID_CORE, 0x00),
    rwID_STRUCT = MAKECHUNKID(rwVENDORID_CORE, 0x01),
    rwID_STRING = MAKECHUNKID(rwVENDORID_CORE, 0x02),
    rwID_EXTENSION = MAKECHUNKID(rwVENDORID_CORE, 0x03),
    rwID_CAMERA = MAKECHUNKID(rwVENDORID_CORE, 0x05),
    rwID_TEXTURE = MAKECHUNKID(rwVENDORID_CORE, 0x06),
    rwID_MATERIAL = MAKECHUNKID(rwVENDORID_CORE, 0x07),
    rwID_MATLIST = MAKECHUNKID(rwVENDORID_CORE, 0x08),
    rwID_ATOMICSECT = MAKECHUNKID(rwVENDORID_CORE, 0x09),
    rwID_PLANESECT = MAKECHUNKID(rwVENDORID_CORE, 0x0A),
    rwID_WORLD = MAKECHUNKID(rwVENDORID_CORE, 0x0B),
    rwID_SPLINE = MAKECHUNKID(rwVENDORID_CORE, 0x0C),
    rwID_MATRIX = MAKECHUNKID(rwVENDORID_CORE, 0x0D),
    rwID_FRAMELIST = MAKECHUNKID(rwVENDORID_CORE, 0x0E),
    rwID_GEOMETRY = MAKECHUNKID(rwVENDORID_CORE, 0x0F),
    rwID_CLUMP = MAKECHUNKID(rwVENDORID_CORE, 0x10),
    rwID_LIGHT = MAKECHUNKID(rwVENDORID_CORE, 0x12),
    rwID_UNICODESTRING = MAKECHUNKID(rwVENDORID_CORE, 0x13),
    rwID_ATOMIC = MAKECHUNKID(rwVENDORID_CORE, 0x14),
    rwID_TEXTURENATIVE = MAKECHUNKID(rwVENDORID_CORE, 0x15),
    rwID_TEXDICTIONARY = MAKECHUNKID(rwVENDORID_CORE, 0x16),
    rwID_ANIMDATABASE = MAKECHUNKID(rwVENDORID_CORE, 0x17),
    rwID_IMAGE = MAKECHUNKID(rwVENDORID_CORE, 0x18),
    rwID_SKINANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x19),
    rwID_GEOMETRYLIST = MAKECHUNKID(rwVENDORID_CORE, 0x1A),
    rwID_ANIMANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x1B),
    rwID_HANIMANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x1B),
    rwID_TEAM = MAKECHUNKID(rwVENDORID_CORE, 0x1C),
    rwID_CROWD = MAKECHUNKID(rwVENDORID_CORE, 0x1D),
    rwID_DMORPHANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x1E),
    rwID_RIGHTTORENDER = MAKECHUNKID(rwVENDORID_CORE, 0x1f),
    rwID_MTEFFECTNATIVE = MAKECHUNKID(rwVENDORID_CORE, 0x20),
    rwID_MTEFFECTDICT = MAKECHUNKID(rwVENDORID_CORE, 0x21),
    rwID_TEAMDICTIONARY = MAKECHUNKID(rwVENDORID_CORE, 0x22),
    rwID_PITEXDICTIONARY = MAKECHUNKID(rwVENDORID_CORE, 0x23),
    rwID_TOC = MAKECHUNKID(rwVENDORID_CORE, 0x24),
    rwID_PRTSTDGLOBALDATA = MAKECHUNKID(rwVENDORID_CORE, 0x25),
    rwID_ALTPIPE = MAKECHUNKID(rwVENDORID_CORE, 0x26),
    rwID_PIPEDS = MAKECHUNKID(rwVENDORID_CORE, 0x27),
    rwID_PATCHMESH = MAKECHUNKID(rwVENDORID_CORE, 0x28),
    rwID_CHUNKGROUPSTART = MAKECHUNKID(rwVENDORID_CORE, 0x29),
    rwID_CHUNKGROUPEND = MAKECHUNKID(rwVENDORID_CORE, 0x2A),
    rwID_UVANIMDICT = MAKECHUNKID(rwVENDORID_CORE, 0x2B),
    rwID_COLLTREE = MAKECHUNKID(rwVENDORID_CORE, 0x2C),
    rwID_ENVIRONMENT = MAKECHUNKID(rwVENDORID_CORE, 0x2D),
    rwID_COREPLUGINIDMAX = MAKECHUNKID(rwVENDORID_CORE, 0x2E),
    rwID_SKYMIPMAP = MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x10),
    rwID_SKIN = MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x16),
    rwID_HANIM = MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x1E),
    rwID_MATFX = MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x20),
    rwID_ANISOT = MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x27),
    rwID_NORMALMAP = MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x33),
    rwID_UVANIM = MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x35),
    rwID_BINMESH = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0xE),
    rwID_NATIVEDATA = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x10)
};

enum gtaRwTextureFilterMode {
    rwFILTERNAFILTERMODE = 0,
    rwFILTERNEAREST,
    rwFILTERLINEAR,
    rwFILTERMIPNEAREST,
    rwFILTERMIPLINEAR,
    rwFILTERLINEARMIPNEAREST,
    rwFILTERLINEARMIPLINEAR
};

enum gtaRwTextureAddressMode {
    rwTEXTUREADDRESSNATEXTUREADDRESS = 0,
    rwTEXTUREADDRESSWRAP,
    rwTEXTUREADDRESSMIRROR,
    rwTEXTUREADDRESSCLAMP,
    rwTEXTUREADDRESSBORDER
};

enum gtaRwMatFXMaterialFlags {
    rwMATFXEFFECTNULL = 0, // No material effect
    rwMATFXEFFECTBUMPMAP = 1, // Bump mapping
    rwMATFXEFFECTENVMAP = 2, // Environment mapping
    rwMATFXEFFECTBUMPENVMAP = 3, // Bump and environment mapping
    rwMATFXEFFECTDUAL = 4, // Dual pass
    rwMATFXEFFECTUVTRANSFORM = 5, // Base UV transform
    rwMATFXEFFECTDUALUVTRANSFORM = 6  // Dual UV transform
};

enum gtaRwGeometryFlag {
    rwGEOMETRYTRISTRIP = 0x00000001, // This geometry's meshes can be rendered as strips.
    rwGEOMETRYPOSITIONS = 0x00000002, // This geometry has positions
    rwGEOMETRYTEXTURED = 0x00000004, // This geometry has only one set of texture coordinates. Texture coordinates are specified on a per vertex basis
    rwGEOMETRYPRELIT = 0x00000008, // This geometry has pre-light colors
    rwGEOMETRYNORMALS = 0x00000010, // This geometry has vertex normals
    rwGEOMETRYLIGHT = 0x00000020, // This geometry will be lit
    rwGEOMETRYMODULATEMATERIALCOLOR = 0x00000040, // Modulate material color with vertex colors (pre-lit + lit)
    rwGEOMETRYTEXTURED2 = 0x00000080, // This geometry has at least 2 sets of texture coordinates.
    rwGEOMETRYNATIVE = 0x01000000,
    rwGEOMETRYNATIVEINSTANCE = 0x02000000
};

// represents the different types of mesh.
enum gtaRwMeshHeaderFlags {
    rwMESHHEADERTRISTRIP = 0x0001, // Render as tristrips
    rwMESHHEADERTRIFAN = 0x0002, // On PlayStation 2 these will be converted to trilists
    rwMESHHEADERLINELIST = 0x0004, // Render as linelists
    rwMESHHEADERPOLYLINE = 0x0008, // On PlayStation 2 these will be converted to linelists
    rwMESHHEADERPOINTLIST = 0x0010, // Pointlists are supported only if rendered by custom pipelines; there is no default RenderWare way to render pointlists.
    rwMESHHEADERPRIMMASK = 0x00FF, // All bits reserved for specifying primitive type
    rwMESHHEADERUNINDEXED = 0x0100, // Topology is defined implicitly by vertex order, ergo the mesh contains no indices
};

enum gtaRwRasterFormat {
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
};

enum gtaRwAtomicFlag {
    rwATOMICCOLLISIONTEST = 0x01, // A generic collision flag to indicate that the atomic should be considered in collision tests.
    rwATOMICRENDER = 0x04         // The atomic is rendered if it is in the view frustum.
};

#define rwHANIMSTREAMCURRENTVERSION 0x100
// Flags for gtaRwHAnimNode
#define rwHANIMPOPPARENTMATRIX      0x01
#define rwHANIMPUSHPARENTMATRIX     0x02

enum gtaRwHAnimHierarchyFlag {
    // creation flags
    rwHANIMHIERARCHYSUBHIERARCHY = 0x01, // This hierarchy is a sub-hierarchy
    rwHANIMHIERARCHYNOMATRICES = 0x02, // This hierarchy has no local matrices
    // update flags
    rwHANIMHIERARCHYUPDATEMODELLINGMATRICES = 0x1000, // This hierarchy updates modeling matrices
    rwHANIMHIERARCHYUPDATELTMS = 0x2000, // This hierarchy updates LTMs
    rwHANIMHIERARCHYLOCALSPACEMATRICES = 0x4000, // This hierarchy calculates matrices in a space
};

struct gtaRwV2d {
    gtaRwReal x;
    gtaRwReal y;
};

struct gtaRwV3d {
    gtaRwReal x;
    gtaRwReal y;
    gtaRwReal z;
};

struct gtaRwRGBA {
    gtaRwUInt8 r;
    gtaRwUInt8 g;
    gtaRwUInt8 b;
    gtaRwUInt8 a;
};

// This type represents the ambient, diffuse and
// specular reflection coefficients of a particular geometry. Each coefficient
// is specified in the range 0.0 (no reflection) to 1.0 (maximum reflection). 
// Note that currently the specular element is not used.
struct gtaRwSurfaceProperties {
    gtaRwReal ambient;  // ambient reflection coefficient
    gtaRwReal specular; // specular reflection coefficient
    gtaRwReal diffuse;  // reflection coefficient
};

// This type represents the u and v texture
// coordinates of a particular vertex.
struct gtaRwTexCoords {
    gtaRwReal u; // U value
    gtaRwReal v; // V value
};

struct gtaRwTriangle {
    gtaRwUInt16 vertB;
    gtaRwUInt16 vertA;
    gtaRwInt16 mtlId;
    gtaRwUInt16 vertC;
};

// This type represents a sphere specified by the position
// of its center and its radius.
struct gtaRwSphere {
    gtaRwV3d center;   // Sphere center
    gtaRwReal radius;  // Sphere radius
};

struct gtaRwMorphTarget {
    gtaRwSphere boundingSphere;
    gtaRwBool hasVerts;
    gtaRwBool hasNormals;
    gtaRwV3d *verts;
    gtaRwV3d *normals;
};

struct gtaRwMatrix {
    gtaRwV3d    right;
    gtaRwUInt32 flags;
    gtaRwV3d    up;
    gtaRwUInt32 pad1;
    gtaRwV3d    at;
    gtaRwUInt32 pad2;
    gtaRwV3d    pos;
    gtaRwUInt32 pad3;
};
