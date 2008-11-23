/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CTrainerMonitoringHistory.h
*  PURPOSE:     Header for trainer monitoring history class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CTRAINERMONITORINGHISTORY_H
#define __CTRAINERMONITORINGHISTORY_H

#include <vector>

using namespace std;

struct STrainerMonitoringHistoryItem
{
    unsigned long ulOffset;
    unsigned int  uiSize;
    unsigned int  uiType; // 0 = MTA_OFFSET_BASE, 1 = MTA_OFFSET_PLAYER, 2 = MTA_OFFSET_VEHICLE
};

class CTrainerMonitoringHistory
{
public:
                    CTrainerMonitoringHistory           ( void );
                    ~CTrainerMonitoringHistory          ( void );

    void            Add                                 ( unsigned long ulOffset, unsigned int uiSize, unsigned int uiType );
    bool            Exists                              ( unsigned long ulOffset, unsigned int uiSize, unsigned int uiType );
    void            Clear                               ( void );

private:
    std::vector < STrainerMonitoringHistoryItem>  m_History;
};

#endif
