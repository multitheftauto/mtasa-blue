/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CModManagerImpl.h
*  PURPOSE:     Mod manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Oli <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CModManagerImpl;

#ifndef __CMODMANAGERIMPL_H
#define __CMODMANAGERIMPL_H

#include <core/CModManager.h>
#include <core/CServerBase.h>

#include "CDynamicLibrary.h"
#include "CServerImpl.h"

typedef CServerBase* (InitServer)();

class CModManagerImpl : public CModManager
{
public:
                        CModManagerImpl         ( CServerImpl* pServer );
                        ~CModManagerImpl        ( void );

    inline void         SetServerPath           ( const char* szServerPath )    { m_strServerPath = szServerPath; };

    bool                RequestLoad             ( const char* szModName );

    const char*         GetServerPath           ( void )                    { return m_strServerPath; };
    inline const char*  GetModPath              ( void )                    { return m_strModPath; };
    virtual SString     GetAbsolutePath         ( const char* szRelative );

    bool                IsModLoaded             ( void );
    CServerBase*        GetCurrentMod           ( void );

    bool                Load                    ( const char* szModName, int iArgumentCount, char* szArguments [] );
    void                Unload                  ( bool bKeyPressBeforeTerm = false );

    void                HandleInput             ( const char* szCommand );
    void                GetTag                  ( char* szInfoTag, int iInfoTag );

    void                DoPulse                 ( void );

    bool                IsFinished              ( void );

    bool                PendingWorkToDo         ( void );
    bool                GetSleepIntervals       ( int& iSleepBusyMs, int& iSleepIdleMs );

private:
    CServerImpl*        m_pServer;

    CServerBase*        m_pBase;
    CDynamicLibrary     m_Library;
    SString             m_strServerPath;    // Root path
    SString             m_strModPath;       // root/mods/deathmatch
};

#endif
