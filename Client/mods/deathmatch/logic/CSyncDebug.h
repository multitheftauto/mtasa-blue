/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CSyncDebug.h
 *  PURPOSE:     Header for sync debug class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
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

    std::uint32_t m_uiPacketsSent;
    std::uint32_t m_uiLastPacketsSent;
    std::uint32_t m_uiLastPacketsReceived;
    std::uint32_t m_uiPacketsReceived;
    std::uint32_t m_uiBitsReceived;
    std::uint32_t m_uiLastBitsReceived;
    std::uint32_t m_uiBitsSent;
    std::uint32_t m_uiLastBitsSent;

    std::uint16_t m_usFakeLagVariance;
    std::uint16_t m_usFakeLagPing;

#endif
};
