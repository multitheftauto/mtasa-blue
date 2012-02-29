/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CRealtimeStats.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CRealtimeStatsInterface
{
public:
    virtual             ~CRealtimeStatsInterface  ( void ) {}

    virtual void        FrameEnd                    ( void ) = 0;
    virtual void        DoPulse                     ( void ) = 0;
    virtual void        OnCommand                   ( const std::string& strParameters ) = 0;
    virtual void        RemovedStatSource           ( void ) = 0;
};

CRealtimeStatsInterface* GetRealtimeStats ( void );

