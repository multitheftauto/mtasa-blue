/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CMultiplayerSA_Files.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"


//////////////////////////////////////////////////////////////////////////////////////////
//
// fopen
//
// Record problematic fopen failures
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_Rtl_fopen_Post ( FILE* fh, DWORD calledFrom, const char* szFilename, const char* szMode )
{
    // Check for file open error
    if ( !fh )
    {
        SString strFilename = szFilename;

        // These files can fail without problem
        static const char* ignoreList[] = {
                                    "GTASAsf",
                                    "sa-utrax.dat",
                                    "gta3.ini",
                                    "player.bmp",
                                    "gta_sa.set"
                                };

        for ( uint i = 0 ; i < NUMELMS ( ignoreList ) ; i++ )
        {
            if ( strFilename.ContainsI ( ignoreList[i] ) )
                return;
        }

        AddReportLog ( 5321, SString ( "Rtl_fopen failed: called from:%08x  mode:%s  name:%s", calledFrom, szMode, *strFilename ) );
        SetApplicationSetting ( "diagnostics", "gta-fopen-fail", strFilename );
    }
}

// Hook info
#define HOOKPOS_Rtl_fopen_US                         0x8232D8
#define HOOKSIZE_Rtl_fopen_US                        6
#define HOOKPOS_Rtl_fopen_EU                         0x823318
#define HOOKSIZE_Rtl_fopen_EU                        6
DWORD RETURN_Rtl_fopen_US =                          0x8232DE;
DWORD RETURN_Rtl_fopen_EU =                          0x82331E;
DWORD RETURN_Rtl_fopen_BOTH =                        0;
void _declspec(naked) HOOK_Rtl_fopen()
{
    _asm
    {
        push    [esp+4*3]
        push    [esp+4*3]
        push    [esp+4*3]
        call inner
        add     esp, 4*3

        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        push    eax
        call    OnMY_Rtl_fopen_Post
        add     esp, 4*2+4+4
        popad
        retn

inner:
        push    40h
        push    [esp+0x0c] 
        jmp     RETURN_Rtl_fopen_BOTH
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_Files
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Files ( void )
{
    EZHookInstall ( Rtl_fopen );
}
