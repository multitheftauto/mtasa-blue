/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CTrainerMonitoringHistory.cpp
*  PURPOSE:     Trainer monitoring log
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CTrainerMonitoringHistory::CTrainerMonitoringHistory ( void )
{

}


CTrainerMonitoringHistory::~CTrainerMonitoringHistory ( void )
{

}


void CTrainerMonitoringHistory::Add ( unsigned long ulOffset, unsigned int uiSize, unsigned int uiType )
{
    // Add it to the history
    STrainerMonitoringHistoryItem Temp;
    Temp.ulOffset = ulOffset;
    Temp.uiSize = uiSize;
    Temp.uiType = uiType;
    m_History.push_back ( Temp );
}


bool CTrainerMonitoringHistory::Exists ( unsigned long ulOffset, unsigned int uiSize, unsigned int uiType )
{
    // Iterate through the items
    vector < STrainerMonitoringHistoryItem > ::iterator iter = m_History.begin ();
    for ( ; iter != m_History.end (); iter++ )
    {
        if ( ulOffset == iter->ulOffset && uiSize == iter->uiSize && uiType == iter->uiType )
        {
            return true;
        }
    }

    // Doesn't exist
    return false;
}


void CTrainerMonitoringHistory::Clear ( void )
{
    m_History.clear ();
}
