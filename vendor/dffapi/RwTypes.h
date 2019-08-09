#pragma once

#define MAKECHUNKID(vendorID, chunkID) (((vendorID & 0xFFFFFF) << 8) | (chunkID & 0xFF))

typedef long gtaRwFixed;
typedef int  gtaRwInt32;
typedef unsigned int gtaRwUInt32;
typedef short gtaRwInt16;
typedef unsigned short gtaRwUInt16;
typedef unsigned char gtaRwUInt8;
typedef signed char gtaRwInt8;
typedef char gtaRwChar;
typedef float gtaRwReal;
typedef gtaRwInt32 gtaRwBool;

enum gtaRwPlatformID
{
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

enum gtaRwPluginVendor
{
	rwVENDORID_CORE             = 0x000000L,
	rwVENDORID_CRITERIONTK      = 0x000001L,
	rwVENDORID_REDLINERACER     = 0x000002L,
	rwVENDORID_CSLRD            = 0x000003L,
	rwVENDORID_CRITERIONINT     = 0x000004L,
	rwVENDORID_CRITERIONWORLD   = 0x000005L,
	rwVENDORID_BETA             = 0x000006L,
	rwVENDORID_CRITERIONRM      = 0x000007L,
	rwVENDORID_CRITERIONRWA     = 0x000008L,
	rwVENDORID_CRITERIONRWP     = 0x000009L,
};

enum gtaRwCorePluginID
{
	rwID_NAOBJECT               = MAKECHUNKID(rwVENDORID_CORE, 0x00),
	rwID_STRUCT                 = MAKECHUNKID(rwVENDORID_CORE, 0x01),
	rwID_STRING                 = MAKECHUNKID(rwVENDORID_CORE, 0x02),
	rwID_EXTENSION              = MAKECHUNKID(rwVENDORID_CORE, 0x03),
	rwID_CAMERA                 = MAKECHUNKID(rwVENDORID_CORE, 0x05),
	rwID_TEXTURE                = MAKECHUNKID(rwVENDORID_CORE, 0x06),
	rwID_MATERIAL               = MAKECHUNKID(rwVENDORID_CORE, 0x07),
	rwID_MATLIST                = MAKECHUNKID(rwVENDORID_CORE, 0x08),
	rwID_ATOMICSECT             = MAKECHUNKID(rwVENDORID_CORE, 0x09),
	rwID_PLANESECT              = MAKECHUNKID(rwVENDORID_CORE, 0x0A),
	rwID_WORLD                  = MAKECHUNKID(rwVENDORID_CORE, 0x0B),
	rwID_SPLINE                 = MAKECHUNKID(rwVENDORID_CORE, 0x0C),
	rwID_MATRIX                 = MAKECHUNKID(rwVENDORID_CORE, 0x0D),
	rwID_FRAMELIST              = MAKECHUNKID(rwVENDORID_CORE, 0x0E),
	rwID_GEOMETRY               = MAKECHUNKID(rwVENDORID_CORE, 0x0F),
	rwID_CLUMP                  = MAKECHUNKID(rwVENDORID_CORE, 0x10),
	rwID_LIGHT                  = MAKECHUNKID(rwVENDORID_CORE, 0x12),
	rwID_UNICODESTRING          = MAKECHUNKID(rwVENDORID_CORE, 0x13),
	rwID_ATOMIC                 = MAKECHUNKID(rwVENDORID_CORE, 0x14),
	rwID_TEXTURENATIVE          = MAKECHUNKID(rwVENDORID_CORE, 0x15),
	rwID_TEXDICTIONARY          = MAKECHUNKID(rwVENDORID_CORE, 0x16),
	rwID_ANIMDATABASE           = MAKECHUNKID(rwVENDORID_CORE, 0x17),
	rwID_IMAGE                  = MAKECHUNKID(rwVENDORID_CORE, 0x18),
	rwID_SKINANIMATION          = MAKECHUNKID(rwVENDORID_CORE, 0x19),
	rwID_GEOMETRYLIST           = MAKECHUNKID(rwVENDORID_CORE, 0x1A),
	rwID_ANIMANIMATION          = MAKECHUNKID(rwVENDORID_CORE, 0x1B),
	rwID_HANIMANIMATION         = MAKECHUNKID(rwVENDORID_CORE, 0x1B),
	rwID_TEAM                   = MAKECHUNKID(rwVENDORID_CORE, 0x1C),
	rwID_CROWD                  = MAKECHUNKID(rwVENDORID_CORE, 0x1D),
	rwID_DMORPHANIMATION        = MAKECHUNKID(rwVENDORID_CORE, 0x1E),
	rwID_RIGHTTORENDER          = MAKECHUNKID(rwVENDORID_CORE, 0x1f),
	rwID_MTEFFECTNATIVE         = MAKECHUNKID(rwVENDORID_CORE, 0x20),
	rwID_MTEFFECTDICT           = MAKECHUNKID(rwVENDORID_CORE, 0x21),
	rwID_TEAMDICTIONARY         = MAKECHUNKID(rwVENDORID_CORE, 0x22),
	rwID_PITEXDICTIONARY        = MAKECHUNKID(rwVENDORID_CORE, 0x23),
	rwID_TOC                    = MAKECHUNKID(rwVENDORID_CORE, 0x24),
	rwID_PRTSTDGLOBALDATA       = MAKECHUNKID(rwVENDORID_CORE, 0x25),
	rwID_ALTPIPE                = MAKECHUNKID(rwVENDORID_CORE, 0x26),
	rwID_PIPEDS                 = MAKECHUNKID(rwVENDORID_CORE, 0x27),
	rwID_PATCHMESH              = MAKECHUNKID(rwVENDORID_CORE, 0x28),
	rwID_CHUNKGROUPSTART        = MAKECHUNKID(rwVENDORID_CORE, 0x29),
	rwID_CHUNKGROUPEND          = MAKECHUNKID(rwVENDORID_CORE, 0x2A),
	rwID_UVANIMDICT             = MAKECHUNKID(rwVENDORID_CORE, 0x2B),
	rwID_COLLTREE               = MAKECHUNKID(rwVENDORID_CORE, 0x2C),
	rwID_ENVIRONMENT            = MAKECHUNKID(rwVENDORID_CORE, 0x2D),
	rwID_COREPLUGINIDMAX        = MAKECHUNKID(rwVENDORID_CORE, 0x2E),
	rwID_SKYMIPMAP              = MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x10),
	rwID_SKIN                  = MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x16),
	rwID_HANIM                  = MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x1E),
	rwID_MATFX                  = MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x20),
	rwID_ANISOT                 = MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x27),
	rwID_NORMALMAP              = MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x33),
	rwID_UVANIM                 = MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x35),
	rwID_BINMESH                = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0xE),
	rwID_NATIVEDATA             = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x10)
};

enum gtaRwTextureFilterMode
{
	rwFILTERNAFILTERMODE = 0,
	rwFILTERNEAREST,
	rwFILTERLINEAR,
	rwFILTERMIPNEAREST,
	rwFILTERMIPLINEAR,
	rwFILTERLINEARMIPNEAREST,
	rwFILTERLINEARMIPLINEAR
};

enum gtaRwTextureAddressMode
{
	rwTEXTUREADDRESSNATEXTUREADDRESS = 0,
	rwTEXTUREADDRESSWRAP,
	rwTEXTUREADDRESSMIRROR,
	rwTEXTUREADDRESSCLAMP,
	rwTEXTUREADDRESSBORDER
};

enum RpMatFXMaterialFlags
{
	rpMATFXEFFECTNULL            = 0, // No material effect
	rpMATFXEFFECTBUMPMAP         = 1, // Bump mapping
	rpMATFXEFFECTENVMAP          = 2, // Environment mapping
	rpMATFXEFFECTBUMPENVMAP      = 3, // Bump and environment mapping
	rpMATFXEFFECTDUAL            = 4, // Dual pass
	rpMATFXEFFECTUVTRANSFORM     = 5, // Base UV transform
	rpMATFXEFFECTDUALUVTRANSFORM = 6  // Dual UV transform
};

enum gtaRwGeometryFlag
{
	rpGEOMETRYTRISTRIP  = 0x00000001, // This geometry's meshes can be rendered as strips.
    rpGEOMETRYPOSITIONS = 0x00000002, // This geometry has positions
    rpGEOMETRYTEXTURED  = 0x00000004, // This geometry has only one set of texture coordinates. Texture coordinates are specified on a per vertex basis
    rpGEOMETRYPRELIT    = 0x00000008, // This geometry has pre-light colors
    rpGEOMETRYNORMALS   = 0x00000010, // This geometry has vertex normals
    rpGEOMETRYLIGHT     = 0x00000020, // This geometry will be lit
    rpGEOMETRYMODULATEMATERIALCOLOR = 0x00000040, // Modulate material color with vertex colors (pre-lit + lit)
    rpGEOMETRYTEXTURED2 = 0x00000080, // This geometry has at least 2 sets of texture coordinates.
    rpGEOMETRYNATIVE            = 0x01000000,
    rpGEOMETRYNATIVEINSTANCE    = 0x02000000
};

// represents the different types of mesh.
enum gtaRwMeshHeaderFlags
{
	rpMESHHEADERTRISTRIP  = 0x0001, // Render as tristrips
    rpMESHHEADERTRIFAN    = 0x0002, // On PlayStation 2 these will be converted to trilists
    rpMESHHEADERLINELIST  = 0x0004, // Render as linelists
    rpMESHHEADERPOLYLINE  = 0x0008, // On PlayStation 2 these will be converted to linelists
    rpMESHHEADERPOINTLIST = 0x0010, // Pointlists are supported only if rendered by custom pipelines; there is no default RenderWare way to render pointlists.
    rpMESHHEADERPRIMMASK  = 0x00FF, // All bits reserved for specifying primitive type
    rpMESHHEADERUNINDEXED = 0x0100, // Topology is defined implicitly by vertex order, ergo the mesh contains no indices
};

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
    rwRASTERFORMATMASK = 0xff00     /**<The whole format */ ,
};


struct gtaRwV2d
{
	gtaRwReal x;
	gtaRwReal y;
};

struct gtaRwV3d
{
	gtaRwReal x;
	gtaRwReal y;
	gtaRwReal z;
};

struct gtaRwRGBA
{
	gtaRwUInt8 r;
	gtaRwUInt8 g;
	gtaRwUInt8 b;
	gtaRwUInt8 a;
};

// This type represents the ambient, diffuse and
// specular reflection coefficients of a particular geometry. Each coefficient
// is specified in the range 0.0 (no reflection) to 1.0 (maximum reflection). 
// Note that currently the specular element is not used.
struct gtaRwSurfaceProperties
{
	gtaRwReal ambient;  // ambient reflection coefficient
	gtaRwReal specular; // specular reflection coefficient
	gtaRwReal diffuse;  // reflection coefficient
};

// This type represents the u and v texture
// coordinates of a particular vertex.
struct gtaRwTexCoords
{
	gtaRwReal u; // U value
	gtaRwReal v; // V value
};

struct gtaRwTriangle
{
	gtaRwUInt16 vertB;
	gtaRwUInt16 vertA;
	gtaRwInt16 mtlId;
	gtaRwUInt16 vertC;
};

// This type represents a sphere specified by the position
// of its center and its radius.
struct gtaRwSphere
{
    gtaRwV3d center;   // Sphere center
    gtaRwReal radius;  // Sphere radius
};

struct gtaRwMorphTarget
{
	gtaRwSphere boundingSphere;
	gtaRwBool hasVerts;
	gtaRwBool hasNormals;
	gtaRwV3d *verts;
	gtaRwV3d *normals;
};

struct gtaRwMatrix
{
	gtaRwV3d    right;
	gtaRwUInt32 flags;
	gtaRwV3d    up;
	gtaRwUInt32 pad1;
	gtaRwV3d    at;
	gtaRwUInt32 pad2;
	gtaRwV3d    pos;
	gtaRwUInt32 pad3;
};