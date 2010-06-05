/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CSyncDebug.h
*  PURPOSE:     Header for sync debug class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CSYNCDEBUG_H
#define __CSYNCDEBUG_H

class CClientManager;
class CClientPlayer;

class CSyncDebug
{
public:

// Debugmode only interface
#ifdef MTA_DEBUG
                        CSyncDebug              ( CClientManager* pManager );
                        ~CSyncDebug             ( void );

    void                Unreference             ( CClientPlayer& Player );

    void                Attach                  ( CClientPlayer& Player );
    void                Detach                  ( void );

    void                OnPulse                 ( void );

#else
    inline              CSyncDebug              ( CClientManager* pManager ) {};

    inline void         Unreference             ( CClientPlayer& Player ) {};

    inline void         Attach                  ( CClientPlayer& Player ) {};
    inline void         Detach                  ( void ) {};

    inline void         OnPulse                 ( void ) {};

#endif

private:

// Debugmode only interface
#ifdef MTA_DEBUG

    void                OnDraw                  ( void );
    void                OnUpdate                ( void );

    CClientManager*     m_pManager;
    CClientPlayer*      m_pPlayer;

    unsigned long       m_ulLastUpdateTime;

    unsigned int        m_uiPacketsSent;
    unsigned int        m_uiLastPacketsSent;
    unsigned int        m_uiLastPacketsReceived;
    unsigned int        m_uiPacketsReceived;
    unsigned int        m_uiBitsReceived;
    unsigned int        m_uiLastBitsReceived;
    unsigned int        m_uiBitsSent;
    unsigned int        m_uiLastBitsSent;

    unsigned short      m_usFakeLagVariance;
    unsigned short      m_usFakeLagPing;

#endif

};



#endif
