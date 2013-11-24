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
#include <net/CNetServer.h>
#include "CPlayerManager.h"

class CPlayer;
class CGame;

#define DECLARE_RPC(a) static void a ( NetBitStreamInterface& bitStream );

class CRPCFunctions
{
protected:
//    enum eRPCFunctions;

private:
    typedef void (*pfnRPCHandler) ( NetBitStreamInterface& bitStream );
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
    void                        ProcessPacket                           ( const NetServerPlayerID& Socket, NetBitStreamInterface& bitStream );

protected:
    DECLARE_RPC ( PlayerInGameNotice );
    DECLARE_RPC ( InitialDataStream );
    DECLARE_RPC ( PlayerTarget );
    DECLARE_RPC ( PlayerWeapon );
    DECLARE_RPC ( KeyBind );
    DECLARE_RPC ( CursorEvent );
    DECLARE_RPC ( RequestStealthKill );

protected:
    static CPlayer *                m_pSourcePlayer;

    std::vector < SRPCHandler * >   m_RPCHandlers;

public:
    enum eRPCFunctions
    {
        PLAYER_INGAME_NOTICE,
        INITIAL_DATA_STREAM,
        PLAYER_TARGET,
        PLAYER_WEAPON,
        KEY_BIND,
        CURSOR_EVENT,
        REQUEST_STEALTH_KILL,
    };
};

#endif
