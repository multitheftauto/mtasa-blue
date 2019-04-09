//
//   Author: Michael Chourdakis
//   URL: https://www.codeproject.com/Articles/28071/Toggle-hardware-data-read-execute-breakpoints-prog
//

#pragma once
#ifdef WIN32
enum HWBRK_TYPE
	{
	HWBRK_TYPE_CODE,
	HWBRK_TYPE_READWRITE,
	HWBRK_TYPE_WRITE,
	};

enum HWBRK_SIZE
	{
	HWBRK_SIZE_1,
	HWBRK_SIZE_2,
	HWBRK_SIZE_4,
	HWBRK_SIZE_8,
	};

HANDLE SetHardwareBreakpoint(HANDLE hThread,HWBRK_TYPE Type,HWBRK_SIZE Size,void* s);
bool RemoveHardwareBreakpoint(HANDLE hBrk);
#endif