/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CModManager.h
*  PURPOSE:     Header file for game mod manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CMODMANAGER_H
#define __CMODMANAGER_H

#include <core/CModManagerInterface.h>
#include <core/CClientBase.h>
#include "CSingleton.h"
#include <windows.h>

#ifdef MTA_DEBUG
    #define CMODMANAGER_CLIENTDLL "client_d.dll"
#else
    #define CMODMANAGER_CLIENTDLL "client.dll"
#endif

class CModManager : public CModManagerInterface, public CSingleton < CModManager >
{
public:
                        CModManager             ( void );
                        ~CModManager            ( void );

    void                RequestLoad             ( const char* szModName, const char* szArguments );
    void                RequestLoadDefault      ( const char* szArguments );
    void                RequestUnload           ( void );
    void                ClearRequest            ( void );

    bool                IsLoaded                ( void );

    CClientBase*        Load                    ( const char* szName, const char* szArguments );
    void                Unload                  ( void );

    void                DoPulsePreFrame         ( void );
    void                DoPulsePostFrame        ( void );

    CClientBase*        GetCurrentMod           ( void );

    void                RefreshMods             ( void );

    static long WINAPI  HandleExceptionGlobal   ( _EXCEPTION_POINTERS* pException );
    static void         DumpCoreLog             ( CExceptionInformation* pExceptionInformation );
    static void         DumpMiniDump            ( _EXCEPTION_POINTERS* pException );
    static void         RunErrorTool            ( CExceptionInformation* pExceptionInformation );

private:
    void                InitializeModList       ( const char* szModFolderPath );
    void                Clear                   ( void );

    void                VerifyAndAddEntry       ( const char* szModFolderPath, const char* szName );

    std::map < std::string, std::string > m_ModDLLFiles;
    HMODULE             m_hClientDLL;
    CClientBase*        m_pClientBase;

    SString             m_strDefaultModName;

    bool                m_bUnloadRequested;
    SString             m_strRequestedMod;
    SString             m_strRequestedModArguments;
};

#endif
