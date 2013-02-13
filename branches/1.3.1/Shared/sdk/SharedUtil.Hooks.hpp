/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Hooks.hpp
*  PURPOSE:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

namespace SharedUtil
{

    #define MAX_JUMPCODE_SIZE 50

    VOID HookInstallMethod( DWORD dwInstallAddress,
                            DWORD dwHookFunction )
    {
        MemPut < DWORD > ( dwInstallAddress, dwHookFunction );
    }

    VOID HookInstallCall ( DWORD dwInstallAddress,
                            DWORD dwHookFunction )
    {
        DWORD dwOffset = dwHookFunction - (dwInstallAddress + 5);
        MemPut < BYTE > ( dwInstallAddress, 0xE8 );
        MemPut < DWORD > ( dwInstallAddress+1, dwOffset );
    }

    ////////////////////////////////////////////////////////////////////

    BOOL HookInstall( DWORD dwInstallAddress,
                      DWORD dwHookHandler,
                      int iJmpCodeSize )
    {
        BYTE JumpBytes[MAX_JUMPCODE_SIZE];
        MemSetFast ( JumpBytes, 0x90, MAX_JUMPCODE_SIZE );
        if ( CreateJump ( dwInstallAddress, dwHookHandler, JumpBytes ) )
        {
            MemCpy ( (PVOID)dwInstallAddress, JumpBytes, iJmpCodeSize );
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    ////////////////////////////////////////////////////////////////////

    BYTE * CreateJump ( DWORD dwFrom, DWORD dwTo, BYTE * ByteArray )
    {
        ByteArray[0] = 0xE9;
        MemPutFast < DWORD > ( &ByteArray[1], dwTo - (dwFrom + 5) );
        return ByteArray;
    }



    eGameVersion FindGameVersion ( void )
    {
        unsigned char ucA = *reinterpret_cast < unsigned char* > ( 0x748ADD );
        unsigned char ucB = *reinterpret_cast < unsigned char* > ( 0x748ADE );
        if ( ucA == 0xFF && ucB == 0x53 )
        {
            return VERSION_US_10;
        }
        else if ( ucA == 0x0F && ucB == 0x84 )
        {
            return VERSION_EU_10;
        }
        else if ( ucA == 0xFE && ucB == 0x10 )
        {
            return VERSION_11;
        }
        else
        {
            return VERSION_UNKNOWN;
        }
    }

    eGameVersion GetGameVersion ( void )
    {
        static eGameVersion gameVersion = FindGameVersion();
        return gameVersion;
    }

    void MemSet ( void* dwDest, int cValue, uint uiAmount )
    {
        memset ( dwDest, cValue, uiAmount );
    }

    void MemCpy ( void* dwDest, const void* dwSrc, uint uiAmount )
    {
        memcpy ( dwDest, dwSrc, uiAmount );
    }
};
