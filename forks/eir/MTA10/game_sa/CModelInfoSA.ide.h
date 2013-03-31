/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CModelInfoSA.ide.h
*  PURPOSE:     Header file for model info IDE management
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _MODELINFO_IDE_
#define _MODELINFO_IDE_

// IDE Flags
#define OBJECT_WETEFFECT		0x00000001
#define OBJECT_NIGHTTIME		0x00000002
#define OBJECT_ALPHA1			0x00000004
#define OBJECT_ALPHA2			0x00000008
#define OBJECT_DAYTIME			0x00000010
#define OBJECT_INTERIOR			0x00000020
#define OBJECT_NOSHADOW			0x00000040
#define OBJECT_NOCULL			0x00000080
#define OBJECT_NOLOD			0x00000100
#define OBJECT_BREAKGLASS		0x00000200
#define OBJECT_BREAKGLASS_CRACK	0x00000400
#define OBJECT_GARAGE			0x00000800
#define OBJECT_MULTICLUMP		0x00001000
#define OBJECT_VEGETATION		0x00002000
#define OBJECT_BIG_VEGETATION	0x00004000
#define OBJECT_USE_POLYSHADOW	0x00008000
#define OBJECT_EXPLOSIVE		0x00010000
#define OBJECT_SCM				0x00020000
#define OBJECT_UNKNOWN			0x00040000
#define OBJECT_UNKNOWN_2		0x00080000
#define OBJECT_GRAFFITI			0x00100000
#define OBJECT_NOBACKFACECULL	0x00200000
#define OBJECT_STATUE			0x00400000
#define OBJECT_UNKNOWN_HIGH		0xFF800000

void ModelInfoIDE_Init();
void ModelInfoIDE_Shutdown();

#endif //_MODELINFO_IDE_