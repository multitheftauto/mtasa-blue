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

    inline void         SetServerPath           ( const char* szServerPath )    { strncpy ( m_szServerPath, szServerPath, MAX_PATH - 1 ); };

    bool                RequestLoad             ( const char* szModName );

    inline const char*  GetModPath              ( void )                    { return m_szModPath; };
    const char*         GetAbsolutePath         ( const char* szRelative, char* szBuffer, unsigned int uiBufferSize );

    bool                IsModLoaded             ( void );
    CServerBase*        GetCurrentMod           ( void );

    bool                Load                    ( const char* szModName, int iArgumentCount, char* szArguments [] );
    void                Unload                  ( void );

	void				HandleInput				( const char* szCommand );
	void				GetTag					( char* szInfoTag, int iInfoTag );

    void                DoPulse                 ( void );

    bool                IsFinished              ( void );

private:
    CServerImpl*        m_pServer;

    CServerBase*        m_pBase;
    CDynamicLibrary     m_Library;
    char                m_szServerPath [MAX_PATH];
    char                m_szModPath [MAX_PATH];
};

#endif
