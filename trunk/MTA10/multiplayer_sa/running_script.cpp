/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/running_script.cpp
*  PURPOSE:     running script modification
*  DEVELOPERS:  eAi <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#define HOOKPOS_CRunningScript_Process          0x469F00

bool bHasProcessedScript;
void HOOK_CRunningScript_Process();

void running_script_init ( void )
{
    bHasProcessedScript = false;
    HookInstall(HOOKPOS_CRunningScript_Process, (DWORD)HOOK_CRunningScript_Process, 6);
}

float fX, fY, fZ;
void CRunningScript_Process ( void )
{
	if ( !bHasProcessedScript )
	{
        CCamera * pCamera = pGameInterface->GetCamera();
        pCamera->SetFadeColor ( 0, 0, 0 );
        pCamera->Fade ( 0.0f, FADE_OUT );

		DWORD dwFunc = 0x409D10; // RequestSpecialModel

        char szModelName [64];
		strcpy ( szModelName, "player" );
		_asm
		{
			push	26
			lea		eax, szModelName
			push	eax
			push	0
			call	dwFunc
			add		esp, 12
		}

		dwFunc = 0x40EA10; // load all requested models
		_asm
		{
			push	1
			call	dwFunc
			add		esp, 4
		}

		dwFunc = 0x60D790; // setup player ped
		_asm
		{
			push	0
			call	dwFunc
			add		esp, 4
		}
		
		/*dwFunc = 0x05E47E0; // set created by
		_asm
		{
			mov		edi, 0xB7CD98
			mov		ecx, [edi]
			push	2
			call	dwFunc
		}

		dwFunc = 0x609520; // deactivate player ped
		_asm
		{
			push	0
			call	dwFunc
			add		esp, 4
		}
*/
		//_asm int 3
		dwFunc = 0x420B80; // set position
		fX = 2488.562f;
		fY = -1666.864f;
		fZ = 12.8757f;
		_asm
		{
			mov		edi, 0xB7CD98
			push	fZ
			push	fY
			push	fX
			mov		ecx, [edi]
			call	dwFunc
		}
		/*_asm int 3
		dwFunc = 0x609540; // reactivate player ped
		_asm
		{
			push	0
			call	dwFunc
			add		esp, 4
		}

		dwFunc = 0x61A5A0; // CTask::operator new
		_asm
		{
			push	28
			call	dwFunc
			add		esp, 4
		}

		dwFunc = 0x685750; // CTaskSimplePlayerOnFoot::CTaskSimplePlayerOnFoot
		_asm
		{
			mov		ecx, eax
			call	dwFunc
		}

		dwFunc = 0x681AF0; // set task
		_asm
		{
			mov		edi, 0xB7CD98
			mov		edi, [edi]
			mov		ecx, [edi+0x47C]
			add		ecx, 4
			push	0
			push	4	
			push	eax
			call	dwFunc
		}*/

		
		bHasProcessedScript = true;
	}
}

void _declspec(naked) HOOK_CRunningScript_Process()
{
    _asm
    {
        pushad
    }

    CRunningScript_Process ();

    _asm
    {
        popad
	    retn
    }
}