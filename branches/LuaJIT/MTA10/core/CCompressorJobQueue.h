/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        MTA10/core/CCompressorJobQueue.h
*  PURPOSE:     Threaded job queue
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


namespace EJobResult
{
    enum EJobResultType
    {
        NONE,
        SUCCESS,
        FAIL,
    };
};

namespace EJobStage
{
    enum EJobStageType
    {
        NONE,
        COMMAND_QUEUE,
        PROCCESSING,
        RESULT,
        FINISHED,
    };
};

using EJobResult::EJobResultType;
using EJobStage::EJobStageType;


//
// All data realating to a compress job
//
class CCompressJobData
{
public:
    ZERO_ON_NEW
    bool        SetCallback     ( PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback, uint uiTimeSpentInQueue );
    bool        HasCallback     ( void );
    void        ProcessCallback ( void );

    CCompressJobData                 ( void )    { DEBUG_CREATE_COUNT( "CCompressJobData" ); }
    ~CCompressJobData ( void )
    {
        DEBUG_DESTROY_COUNT( "CCompressJobData" );
    }

    EJobStageType   stage;

    struct
    {
        uint                uiSizeX;
        uint                uiSizeY;
        uint                uiQuality;
        CBuffer             buffer;
    } command;

    struct
    {
        EJobResultType      status;
        CBuffer             buffer;
    } result;

    struct
    {
        PFN_SCREENSHOT_CALLBACK     pfnScreenShotCallback;
        uint                        uiTimeSpentInQueue;
        bool                        bSet;
        bool                        bDone;
    } callback;
};


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueue
//
//
//
///////////////////////////////////////////////////////////////
class CCompressorJobQueue
{
public:
    virtual                     ~CCompressorJobQueue        ( void ) {}

    virtual void                DoPulse                     ( void ) = 0;
    virtual CCompressJobData*   AddCommand                  ( uint uiSizeX, uint uiSizeY, uint uiQuality, uint uiTimeSpentInQueue, PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback, const CBuffer& buffer ) = 0;
    virtual bool                PollCommand                 ( CCompressJobData* pJobData, uint uiTimeout ) = 0;
    virtual bool                FreeCommand                 ( CCompressJobData* pJobData ) = 0;
};

CCompressorJobQueue* NewCompressorJobQueue ( void );
