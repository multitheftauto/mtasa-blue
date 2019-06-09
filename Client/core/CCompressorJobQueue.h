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
    bool SetCallback(PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback, uint uiTimeSpentInQueue);
    bool HasCallback();
    void ProcessCallback();

    CCompressJobData() { DEBUG_CREATE_COUNT("CCompressJobData"); }
    ~CCompressJobData() { DEBUG_DESTROY_COUNT("CCompressJobData"); }

    EJobStageType stage = EJobStageType::NONE;

    struct
    {
        uint    uiSizeX = 0;
        uint    uiSizeY = 0;
        uint    uiQuality = 0;
        CBuffer buffer;
    } command;

    struct
    {
        EJobResultType status = EJobResultType::NONE;
        CBuffer        buffer;
    } result;

    struct
    {
        PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback = nullptr;
        uint                    uiTimeSpentInQueue = 0;
        bool                    bSet = false;
        bool                    bDone = false;
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
    virtual ~CCompressorJobQueue() {}

    virtual void              DoPulse() = 0;
    virtual CCompressJobData* AddCommand(uint uiSizeX, uint uiSizeY, uint uiQuality, uint uiTimeSpentInQueue, PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback,
                                         const CBuffer& buffer) = 0;
    virtual bool              PollCommand(CCompressJobData* pJobData, uint uiTimeout) = 0;
    virtual bool              FreeCommand(CCompressJobData* pJobData) = 0;
};

CCompressorJobQueue* NewCompressorJobQueue();
