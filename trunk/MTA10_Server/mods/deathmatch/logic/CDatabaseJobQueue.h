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

enum EJobType
{
    JOBTYPE_CONNECT,
    JOBTYPE_DISCONNECT,
    JOBTYPE_QUERY,
};

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

    virtual SJobHandle          AddCommand                  ( EJobType jobType, SConnectionHandle connectionHandle, const SString& strData ) = 0;
    virtual bool                PollCommand                 ( SQueryResult& queryResult, SJobHandle jobHandle ) = 0;
    virtual bool                IgnoreCommandResult         ( SJobHandle jobHandle ) = 0;
    virtual void                IgnoreConnectionResults     ( SConnectionHandle connectionHandle ) = 0;

    static bool                 IsValidJobHandleRange       ( SJobHandle jobHandle );
};

CDatabaseJobQueue* NewDatabaseJobQueue ( void );
