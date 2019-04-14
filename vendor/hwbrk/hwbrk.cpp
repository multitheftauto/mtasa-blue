//
//   Author: Michael Chourdakis
//   URL: https://www.codeproject.com/Articles/28071/Toggle-hardware-data-read-execute-breakpoints-prog
//   Hardware Breakpoint Functions
//

#ifdef WIN32
#include <windows.h>
#include "hwbrk.h"

class HWBRK
	{
	public:
		void* a;
		HANDLE hT;
		HWBRK_TYPE Type;
		HWBRK_SIZE Size;
		HANDLE hEv;
		int iReg;
		int Opr;
		bool SUCC;

		HWBRK()
			{
			Opr = 0;
			a = 0;
			hT = 0;
			hEv = 0;
			iReg = 0;
			SUCC = false;
			}
	};


void SetBits(DWORD_PTR& dw, int lowBit, int bits, int newValue)
	{
	DWORD_PTR mask = (1 << bits) - 1; 
	dw = (dw & ~(mask << lowBit)) | (newValue << lowBit);
	}

static DWORD WINAPI th(LPVOID lpParameter)
	{
	HWBRK* h = (HWBRK*)lpParameter;
	int j = 0;
	int y = 0;

	j = SuspendThread(h->hT);
    y = GetLastError();

	CONTEXT ct = {0};
	ct.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	j = GetThreadContext(h->hT,&ct);
    y = GetLastError();

	int FlagBit = 0;

	bool Dr0Busy = false;
	bool Dr1Busy = false;
	bool Dr2Busy = false;
	bool Dr3Busy = false;
	if (ct.Dr7 & 1)
		Dr0Busy = true;
	if (ct.Dr7 & 4)
		Dr1Busy = true;
	if (ct.Dr7 & 16)
		Dr2Busy = true;
	if (ct.Dr7 & 64)
		Dr3Busy = true;

	if (h->Opr == 1)
		{
		// Remove
		if (h->iReg == 0)
			{
			FlagBit = 0;
			ct.Dr0 = 0;
			Dr0Busy = false;
			}
		if (h->iReg == 1)
			{
			FlagBit = 2;
			ct.Dr1 = 0;
			Dr1Busy = false;
			}
		if (h->iReg == 2)
			{
			FlagBit = 4;
			ct.Dr2 = 0;
			Dr2Busy = false;
			}
		if (h->iReg == 3)
			{
			FlagBit = 6;
			ct.Dr3 = 0;
			Dr3Busy = false;
			}

		ct.Dr7 &= ~(1 << FlagBit);
		}
	else
		{
		if (!Dr0Busy)
			{
			h->iReg = 0;
			ct.Dr0 = (DWORD_PTR)h->a;
			Dr0Busy = true;
			}
		else
		if (!Dr1Busy)
			{
			h->iReg = 1;
			ct.Dr1 = (DWORD_PTR)h->a;
			Dr1Busy = true;
			}
		else
		if (!Dr2Busy)
			{
			h->iReg = 2;
			ct.Dr2 = (DWORD_PTR)h->a;
			Dr2Busy = true;
			}
		else
		if (!Dr3Busy)
			{
			h->iReg = 3;
			ct.Dr3 = (DWORD_PTR)h->a;
			Dr3Busy = true;
			}
		else
			{
			h->SUCC = false;
			j = ResumeThread(h->hT);
			y = GetLastError();
			SetEvent(h->hEv);
			return 0;
			}
		ct.Dr6 = 0;
		int st = 0;
		if (h->Type == HWBRK_TYPE_CODE)
			st = 0;
		if (h->Type == HWBRK_TYPE_READWRITE)
			st = 3;
		if (h->Type == HWBRK_TYPE_WRITE)
			st = 1;
		int le = 0;
		if (h->Size == HWBRK_SIZE_1)
			le = 0;
		if (h->Size == HWBRK_SIZE_2)
			le = 1;
		if (h->Size == HWBRK_SIZE_4)
			le = 3;
		if (h->Size == HWBRK_SIZE_8)
			le = 2;

		SetBits(ct.Dr7, 16 + h->iReg*4, 2, st);
		SetBits(ct.Dr7, 18 + h->iReg*4, 2, le);
		SetBits(ct.Dr7, h->iReg*2,1,1);
		}



	ct.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	j = SetThreadContext(h->hT,&ct);
    y = GetLastError();

	ct.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	j = GetThreadContext(h->hT,&ct);
    y = GetLastError();

	j = ResumeThread(h->hT);
    y = GetLastError();

	h->SUCC = true;

	SetEvent(h->hEv);
	return 0;
	}

HANDLE SetHardwareBreakpoint(HANDLE hThread,HWBRK_TYPE Type,HWBRK_SIZE Size,void* s)
	{
	HWBRK* h = new HWBRK;
	h->a = s;
	h->Size = Size;
	h->Type = Type;
	h->hT = hThread;


	if (hThread == GetCurrentThread())
		{
		DWORD pid = GetCurrentThreadId();
		h->hT = OpenThread(THREAD_ALL_ACCESS,0,pid);
		}

	h->hEv = CreateEvent(0,0,0,0);
	h->Opr = 0; // Set Break
	HANDLE hY = CreateThread(0,0,th,(LPVOID)h,0,0);
	WaitForSingleObject(h->hEv,INFINITE);
	CloseHandle(h->hEv);
	h->hEv = 0;

	if (hThread == GetCurrentThread())
		{
		CloseHandle(h->hT);
		}
	h->hT = hThread;

	if (!h->SUCC)
		{
		delete h;
		return 0;
		}


	return (HANDLE)h;
	}






bool RemoveHardwareBreakpoint(HANDLE hBrk)
	{
	HWBRK* h = (HWBRK*)hBrk;
	if (!h)
		return false;

	bool C = false;
	if (h->hT == GetCurrentThread())
		{
		DWORD pid = GetCurrentThreadId();
		h->hT = OpenThread(THREAD_ALL_ACCESS,0,pid);
		C = true;
		}

	h->hEv = CreateEvent(0,0,0,0);
	h->Opr = 1; // Remove Break
	HANDLE hY = CreateThread(0,0,th,(LPVOID)h,0,0);
	WaitForSingleObject(h->hEv,INFINITE);
	CloseHandle(h->hEv);
	h->hEv = 0;

	if (C)
		{
		CloseHandle(h->hT);
		}

	delete h;
	return true;
	}
#endif