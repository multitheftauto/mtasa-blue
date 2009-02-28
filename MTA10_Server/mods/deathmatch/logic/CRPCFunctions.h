/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CRPCFunctions.h
*  PURPOSE:     Remote procedure call functionality class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRPCFunctions_H
#define __CRPCFunctions_H

#include "CCommon.h"
#include <vector>
using namespace std;
#include <net/CNetServer.h>
#include "CPlayerManager.h"

class CPlayer;
class CGame;

#define DECLARE_RPC(a) static void a ( NetServerBitStreamInterface& bitStream );

class CRPCFunctions
{
protected:
    enum eRPCFunctions;

private:
    typedef void (*pfnRPCHandler) ( NetServerBitStreamInterface& bitStream );
    struct SRPCHandler
    {
        unsigned char ID;
        pfnRPCHandler Callback;
    };
public:

                                CRPCFunctions                           ( void );
    virtual                     ~CRPCFunctions                          ( void );

    void                        AddHandlers                             ( void );
    static void                 AddHandler                              ( unsigned char ucID, pfnRPCHandler Callback );
    void                        ProcessPacket                           ( NetServerPlayerID& Socket, NetServerBitStreamInterface& bitStream );

protected:
    DECLARE_RPC ( PlayerInGameNotice );
    DECLARE_RPC ( PlayerTarget );
    DECLARE_RPC ( PlayerWeapon );
    DECLARE_RPC ( KeyBind );
    DECLARE_RPC ( CursorEvent );
    DECLARE_RPC ( RequestStealthKill );

protected:
    static CPlayer *            m_pSourcePlayer;

    vector < SRPCHandler * >    m_RPCHandlers;

    enum eRPCFunctions
    {
        PLAYER_INGAME_NOTICE,
        PLAYER_TARGET,
        PLAYER_WEAPON,
        KEY_BIND,
        CURSOR_EVENT,
        REQUEST_STEALTH_KILL,
    };
};

#endif