//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef enum gtaRwGameVersionId gtaRwGameVersionId;
typedef enum gtaRwPlatformId gtaRwPlatformId;
typedef enum gtaRwCustomIds gtaRwCustomIds;

// affects writing to file
extern gtaRwPlatformId gtaRwPlatform;
extern gtaRwUInt32 gtaRwVersion;
extern gtaRwUInt16 gtaRwBuild;

extern gtaGameVersionId gtaGameVersion;
// affects writing to file
extern gtaPlatformId gtaPlatform;
// affects reading of file
extern gtaRwBool gtaIsMobile;

// affects file reading
extern gtaRwBool gtaRwIgnoreUnknownExtensions;
extern gtaRwBool gtaRwIgnoreTxdExtensions;

void gtaRwSetVersion(gtaRwPlatformId platform, gtaRwUInt32 version, gtaRwUInt16 build);
void gtaRwSetGame(gtaRwGameVersionId game);
