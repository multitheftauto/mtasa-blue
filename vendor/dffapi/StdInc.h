#pragma once
#include "RwTypes.h"

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
	gtaID_PIPELINE        = 0x253F2F3,
	gtaID_SPECMAP         = 0x253F2F6,
	gtaID_2DEFFECT        = 0x253F2F8,
	gtaID_EXTRAVERTCOLOUR = 0x253F2F9,
	gtaID_COLLISIONPLUGIN = 0x253F2FA,
	gtaID_ENVMAP          = 0x253F2FC,
	gtaID_BREAKABLE       = 0x253F2FD,
	gtaID_NODENAME        = 0x253F2FE,
};

extern gtaGameVersionId gtaGameVersion;
// affects writing to file
extern gtaPlatformId gtaPlatform;
extern gtaRwUInt32 gtaRwVersion;
extern gtaRwUInt32 gtaRwBuild;
// affects reading of file
extern gtaRwBool gtaIsMobile;