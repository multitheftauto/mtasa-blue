/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/Platform.cpp
*  PURPOSE:     Platform-specific defines and methods
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PLATFORM_H
#define __PLATFORM_H

extern "C" bool g_bSilent;

/** Operating system define **/
#ifdef WIN32
    #define MTA_OS_STRING "Windows"
#endif

#ifdef LINUX
    #define MTA_OS_STRING "Linux"
#endif

#ifdef FREEBSD
    #define MTA_OS_STRING "FreeBSD"
#endif

#ifdef OPENBSD
    #define MTA_OS_STRING "OpenBSD"
#endif

/** Multi-platform defines **/
#ifdef WIN32
	// Define includes
	#include <conio.h>
	#include <direct.h>
	#include <windows.h>

	// Define functions
	#define Print printf

	// Define types
	#define socklen_t int

	// Define keys
	#define KEY_BACKSPACE	0x08
	#define KEY_EXTENDED	0xE0
	#define KEY_LEFT		0x4B
	#define KEY_RIGHT		0x4D
	#define KEY_UP			0x48
	#define KEY_DOWN		0x50
#else
	// Define includes
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <string.h>
	#include <fcntl.h>
	#include <curses.h>
	#include <dlfcn.h>
	#include <sys/time.h>

	// Define functions
	#define MAX_PATH 255

	#define Print printw

	#define _vsnprintf vsnprintf
	#define _snprintf snprintf

	#ifndef stricmp
	#define stricmp strcasecmp
	#endif
	#ifndef strcmpi
	#define strcmpi strcasecmp
	#endif

	#define _copysign copysign

	#ifndef Sleep
	#define Sleep(duration) usleep(duration * 1000)
	#endif

	// Win32 wrapper function
	unsigned long GetTickCount ( void );

	// No execryptor, so use dummy defines
	#define CRYPT_START
	#define CRYPT_END

	// Itoa replacement function
	char* itoa ( int value, char* result, int base );
#endif

// This function should be used instead of mkdir to preserve multiplatform
// compatibility
extern int mymkdir ( const char* dirname );

// s_systemtypes stuff
// Setup the Operating System
#define MTAS_LINUX 1
#define MTAS_WINDOWS 2

#ifdef __GNUC__
        #define MTAS_SYSTEM MTAS_LINUX
#else
        #define MTAS_SYSTEM MTAS_WINDOWS
#endif

// Setup the types that the non-Windows Operating System needs
#if MTAS_SYSTEM == MTAS_LINUX
	#define BOOL bool
	#define BYTE unsigned char
	#define DWORD unsigned long
	#define FLOAT float
	#ifndef INT
		#define INT int
	#endif
	#ifndef UINT
		#define UINT unsigned int
	#endif
	#define ULONG unsigned long
	#define WORD unsigned short

	#define DOUBLE double

	#define SHORT short
	#define USHORT unsigned short

	#ifndef VOID
		#define VOID void
	#endif
	#define PVOID VOID*

	#define CHAR char
	#define TCHAR char
	#define PCHAR char*

	#ifndef FALSE
		#define FALSE false
	#endif

	#ifndef TRUE
		#define TRUE true
	#endif

//	#define NULL 0
#else
	#include <windows.h>
#endif

#if MTAS_SYSTEM == MTAS_WINDOWS
	#define MTAEXPORT extern "C" __declspec(dllexport)
#else
	#define MTAEXPORT extern "C"
#endif

#endif
