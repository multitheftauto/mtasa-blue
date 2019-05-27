//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef enum gtaRwGameVersionId gtaRwGameVersionId;
typedef enum gtaRwPlatformId gtaRwPlatformId;
typedef enum gtaRwCustomIds gtaRwCustomIds;

enum gtaRwGameVersionId {
    gtaGAME_GTA3,
    gtaGAME_GTAVC,
    gtaGAME_GTASA,
    gtaGAME_MANHUNT
};

enum gtaRwPlatformId {
    PLATFORM_D3D8,
    PLATFORM_D3D9
};

enum gtaRwCustomIds {
    gtaID_PIPELINE = 0x253F2F3,
    gtaID_SPECMAP = 0x253F2F6,
    gtaID_2DEFFECT = 0x253F2F8,
    gtaID_EXTRAVERTCOLOUR = 0x253F2F9,
    gtaID_COLLISIONPLUGIN = 0x253F2FA,
    gtaID_ENVMAP = 0x253F2FC,
    gtaID_BREAKABLE = 0x253F2FD,
    gtaID_NODENAME = 0x253F2FE,
};

// affects writing to file
extern gtaRwPlatformId gtaRwPlatform;
extern gtaRwUInt32 gtaRwVersion;
extern gtaRwUInt16 gtaRwBuild;
// affects file reading
extern gtaRwBool gtaRwIgnoreUnknownExtensions;
extern gtaRwBool gtaRwIgnoreTxdExtensions;

void gtaRwSetVersion(gtaRwPlatformId platform, gtaRwUInt32 version, gtaRwUInt16 build);
void gtaRwSetGame(gtaRwGameVersionId game);
