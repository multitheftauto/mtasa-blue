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
void OnMY_Rtl_fopen_Post(FILE* fh, DWORD calledFrom, const char* szFilename, const char* szMode)
{
    // Note filename in case of crash
    if (fh)
        SetApplicationSetting("diagnostics", "gta-fopen-last", szFilename);

    // Check for file open error
    if (!fh)
    {
        SString strFilename = szFilename;

        // These files can fail without problem
        static const char* ignoreList[] = {
            "GTASAsf", "sa-utrax.dat", "gta3.ini", "player.bmp", "gta_sa.set", "resources", "Gallery",
        };

        for (uint i = 0; i < NUMELMS(ignoreList); i++)
        {
            if (strFilename.ContainsI(ignoreList[i]))
                return;
        }

        AddReportLog(5321, SString("Rtl_fopen failed: called from:%08x  mode:%s  name:%s", calledFrom, szMode, *strFilename));
        SetApplicationSetting("diagnostics", "gta-fopen-fail", strFilename);
    }
}

// Hook info
#define HOOKPOS_Rtl_fopen                            0x8232D8
#define HOOKSIZE_Rtl_fopen                           6
DWORD RETURN_Rtl_fopen = 0x8232DE;
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
        jmp     RETURN_Rtl_fopen
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// fclose
//
// Record problematic fclose failures
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_Rtl_fclose(DWORD calledFrom, FILE* fh)
{
    // Un-note filename
    SetApplicationSetting("diagnostics", "gta-fopen-last", "");
}

// Hook info
#define HOOKPOS_Rtl_fclose                            0x82318B
#define HOOKSIZE_Rtl_fclose                           6
DWORD RETURN_Rtl_fclose = 0x823192;
void _declspec(naked) HOOK_Rtl_fclose()
{
    _asm
    {
        pushad
        push    [esp+32+4*1]
        push    [esp+32+4*1]
        call    OnMY_Rtl_fclose
        add     esp, 4*1 + 4
        popad

        push    0Ch
        push    0x887EC8
        jmp     RETURN_Rtl_fclose
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_Files
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Files()
{
    EZHookInstall(Rtl_fopen);
    EZHookInstall(Rtl_fclose);
}
