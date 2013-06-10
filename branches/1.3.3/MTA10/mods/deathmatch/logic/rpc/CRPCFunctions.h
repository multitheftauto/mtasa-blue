/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CRPCFunctions.h
*  PURPOSE:     Header for RPC functions class
*  DEVELOPERS:  Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRPCFunctions_H
#define __CRPCFunctions_H

class CRPCFunctions;

#define DECLARE_RPC(a) static void a ( class NetBitStreamInterface& bitStream );
#define DECLARE_ELEMENT_RPC(a) static void a ( CClientEntity* pSourceEntity, NetBitStreamInterface& bitStream );

class CRPCFunctions
{
private:

    typedef void (*pfnRPCHandler) ( class NetBitStreamInterface& bitStream );
    typedef void (*pfnElementRPCHandler) ( CClientEntity* pSourceEntity, class NetBitStreamInterface& bitStream );
    struct SRPCHandler
    {
        SRPCHandler () { Callback = NULL; }
        pfnRPCHandler Callback;
        SString strName;
    };
    struct SElementRPCHandler
    {
        SElementRPCHandler () { Callback = NULL; }
        pfnElementRPCHandler Callback;
        SString strName;
    };

public:
                                CRPCFunctions                           ( class CClientGame* pClientGame );
    virtual                     ~CRPCFunctions                          ( void );

    void                        AddHandlers                             ( void );
    static void                 AddHandler                              ( unsigned char ucID, pfnRPCHandler Callback, const char * szName = "unknown" );
    static void                 AddHandler                              ( unsigned char ucID, pfnElementRPCHandler Callback, const char * szName = "unknown" );
    void                        ProcessPacket                           ( unsigned char ucPacketID, class NetBitStreamInterface& bitStream );

    bool                        m_bShowRPCs;

protected:
    static class CClientManager*                m_pManager;
    static class CClientCamera*                 m_pCamera;
    static class CClientMarkerManager*          m_pMarkerManager;
    static class CClientObjectManager*          m_pObjectManager;
    static class CClientPickupManager*          m_pPickupManager;
    static class CClientPlayerManager*          m_pPlayerManager;
    static class CClientRadarAreaManager*       m_pRadarAreaManager;
    static class CClientRadarMarkerManager*     m_pRadarMarkerManager;
    static class CClientDisplayManager*         m_pDisplayManager;
    static class CClientVehicleManager*         m_pVehicleManager;
    static class CClientPathManager*            m_pPathManager;
    static class CClientTeamManager*            m_pTeamManager;
    static class CClientPedManager*             m_pPedManager;
    static class CClientWaterManager*           m_pWaterManager;

    static class CBlendedWeather*               m_pBlendedWeather;
    static class CClientGame*                   m_pClientGame;

protected:
    // Include the RPC functions enum
    #include "net/rpc_enums.h"

    static SFixedArray < SRPCHandler, NUM_RPC_FUNCS >           m_RPCHandlers;
    static SFixedArray < SElementRPCHandler, NUM_RPC_FUNCS >    m_ElementRPCHandlers;
};

#endif
