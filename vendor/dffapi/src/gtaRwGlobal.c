//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

// affects writing to file
gtaRwPlatformId gtaRwPlatform = PLATFORM_D3D9;
gtaRwUInt32 gtaRwVersion = 0x36003;
gtaRwUInt16 gtaRwBuild = 0xFFFF;
gtaRwBool gtaRwIgnoreUnknownExtensions = rwFALSE;
gtaRwBool gtaRwIgnoreTxdExtensions = rwTRUE;

void gtaRwSetVersion(gtaRwPlatformId platform, gtaRwUInt32 version, gtaRwUInt16 build) {
    gtaRwPlatform = platform;
    gtaRwVersion = version;
    gtaRwBuild = build;
}

void gtaRwSetGame(gtaRwGameVersionId game) {
    switch (game) {
    case gtaGAME_GTA3:
        gtaRwSetVersion(PLATFORM_D3D8, 0x33002, 0);
        break;
    case gtaGAME_GTAVC:
        gtaRwSetVersion(PLATFORM_D3D8, 0x34003, 0);
        break;
    case gtaGAME_GTASA:
        gtaRwSetVersion(PLATFORM_D3D9, 0x36003, 0xFFFF);
        break;
    case gtaGAME_MANHUNT:
        gtaRwSetVersion(PLATFORM_D3D8, 0x36003, 0xFFFF);
        break;
    }
}
