/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CSyncDebug.h
 *  PURPOSE:     Header for sync debug class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CClientManager;
class CClientPlayer;

class CSyncDebug
{
public:
// Debugmode only interface
#ifdef MTA_DEBUG
    CSyncDebug(CClientManager* pManager);
    ~CSyncDebug();

    void Unreference(CClientPlayer& Player);

    void Attach(CClientPlayer& Player);
    void Detach();

    void OnPulse();

#else
    CSyncDebug(CClientManager* pManager){};

    void Unreference(CClientPlayer& Player){};

    void Attach(CClientPlayer& Player){};
    void Detach(){};

    void OnPulse(){};

#endif

private:
// Debugmode only interface
#ifdef MTA_DEBUG

    void OnDraw();
    void OnUpdate();

    CClientManager* m_pManager;
    CClientPlayer*  m_pPlayer;

    unsigned long m_ulLastUpdateTime;

    unsigned int m_uiPacketsSent;
    unsigned int m_uiLastPacketsSent;
    unsigned int m_uiLastPacketsReceived;
    unsigned int m_uiPacketsReceived;
    unsigned int m_uiBitsReceived;
    unsigned int m_uiLastBitsReceived;
    unsigned int m_uiBitsSent;
    unsigned int m_uiLastBitsSent;

    unsigned short m_usFakeLagVariance;
    unsigned short m_usFakeLagPing;

#endif
};
