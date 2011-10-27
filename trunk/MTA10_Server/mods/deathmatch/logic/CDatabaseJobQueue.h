/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDatabaseJobQueue.h
*  PURPOSE:     Threaded job queue
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


///////////////////////////////////////////////////////////////
//
// CDatabaseJobQueue
//
//      JobQueue -> DatabaseType -> DatabaseConnection
//
///////////////////////////////////////////////////////////////
class CDatabaseJobQueue
{
public:
    virtual                     ~CDatabaseJobQueue          ( void ) {}

    virtual void                DoPulse                     ( void ) = 0;
    virtual CDbJobData*         AddCommand                  ( EJobCommandType jobType, SConnectionHandle connectionHandle, const SString& strData ) = 0;
    virtual bool                PollCommand                 ( CDbJobData* pJobData ) = 0;
    virtual bool                FreeCommand                 ( CDbJobData* pJobData ) = 0;
    virtual CDbJobData*         FindCommandFromId           ( SDbJobId id ) = 0;
    virtual void                IgnoreConnectionResults     ( SConnectionHandle connectionHandle ) = 0;
};

CDatabaseJobQueue* NewDatabaseJobQueue ( void );
