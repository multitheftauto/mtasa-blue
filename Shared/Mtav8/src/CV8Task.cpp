#include "StdInc.h"

CV8Task::CV8Task(std::shared_ptr<CV8Isolate::CancelationToken> pCancelationToken, std::function<void()> task)
    : m_pCancelationToken(pCancelationToken), m_task(task)
{
}

void CV8Task::Run()
{
    if (!m_pCancelationToken->IsCanceled())
    {
        m_task();
    }
};
