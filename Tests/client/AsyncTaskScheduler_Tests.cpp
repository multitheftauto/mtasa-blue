/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/AsyncTaskScheduler_Tests.cpp
 *  PURPOSE:     SharedUtil async task scheduler tests
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include <SharedUtil.AsyncTaskScheduler.h>

#include <atomic>
#include <future>

TEST(AsyncTaskScheduler, DestructorFinishesQueuedTasks)
{
    std::promise<void> firstTaskStarted;
    std::promise<void> releaseFirstTask;
    std::atomic<bool>  secondTaskExecuted = false;

    auto scheduler = std::make_unique<SharedUtil::CAsyncTaskScheduler>(1);
    scheduler->PushTask(
        [&]
        {
            firstTaskStarted.set_value();
            releaseFirstTask.get_future().wait();
            return 1;
        },
        [](int) {});
    scheduler->PushTask(
        [&]
        {
            secondTaskExecuted = true;
            return 2;
        },
        [](int) {});

    firstTaskStarted.get_future().wait();

    std::promise<void> destructionStarted;
    std::thread        destructorThread(
        [&]
        {
            destructionStarted.set_value();
            scheduler.reset();
        });

    destructionStarted.get_future().wait();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    releaseFirstTask.set_value();
    destructorThread.join();

    EXPECT_TRUE(secondTaskExecuted);
}
