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
    bool SetCallback(PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback, std::uint32_t uiTimeSpentInQueue);
    bool HasCallback();
    void ProcessCallback();

    CCompressJobData() { DEBUG_CREATE_COUNT("CCompressJobData"); }
    ~CCompressJobData() { DEBUG_DESTROY_COUNT("CCompressJobData"); }

    EJobStageType stage;

    struct
    {
        std::uint32_t    uiSizeX;
        std::uint32_t    uiSizeY;
        std::uint32_t    uiQuality;
        CBuffer buffer;
    } command;

    struct
    {
        EJobResultType status;
        CBuffer        buffer;
    } result;

    struct
    {
        PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback;
        std::uint32_t           uiTimeSpentInQueue;
        bool                    bSet;
        bool                    bDone;
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
    virtual CCompressJobData* AddCommand(std::uint32_t uiSizeX, std::uint32_t uiSizeY,
        std::uint32_t uiQuality, std::uint32_t uiTimeSpentInQueue,
        PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback,const CBuffer& buffer) = 0;
    virtual bool              PollCommand(CCompressJobData* pJobData, std::int32_t uiTimeout) = 0;
    virtual bool              FreeCommand(CCompressJobData* pJobData) = 0;
};

CCompressorJobQueue* NewCompressorJobQueue();
