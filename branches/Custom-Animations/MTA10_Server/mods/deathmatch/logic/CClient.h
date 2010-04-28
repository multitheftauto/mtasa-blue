/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CClient.h
*  PURPOSE:     Connected client class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENT_H
#define __CCLIENT_H

class CAccount;
class CElement;

class CClient
{
public:
    enum
    {
        CLIENT_CONSOLE,
        CLIENT_PLAYER,
        CLIENT_REMOTECLIENT,
        CLIENT_SCRIPT,
    };

    enum eQuitReasons
    {
        QUIT_QUIT,
        QUIT_KICK,
        QUIT_BAN,
        QUIT_CONNECTION_DESYNC,
        QUIT_TIMEOUT,
    };

public:
                            CClient                 ( void );
                            ~CClient                ( void );

    virtual int             GetClientType           ( void ) = 0;
    virtual const char*     GetNick                 ( void ) = 0;

    bool                    IsRegistered            ( void );

    virtual void            SendEcho                ( const char* szEcho ) = 0;
    virtual void            SendConsole             ( const char* szEcho ) = 0;

    inline CAccount*        GetAccount              ( void )                    { return m_pAccount; }
    inline void             SetAccount              ( CAccount* pAccount )      { m_pAccount = pAccount; }

    CElement*               GetElement              ( void );

protected:
    CAccount*               m_pAccount;
};

#endif
