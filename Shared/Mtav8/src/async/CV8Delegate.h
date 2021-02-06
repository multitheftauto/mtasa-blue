#pragma once

class CV8Delegate
{
public:
};
class CV8AsyncFunction
{
public:
    CV8AsyncFunction() {}

    void Setup(CV8IsolateBase* pIsolate) { m_pIsolate = pIsolate; }

    // Executes code on worker thread
    virtual void Run() = 0;

    virtual void Cancel() = 0;

    // Returns true if this thread should return from the worker task
    bool ShouldYield() const { return false; }

protected:
    CV8IsolateBase* m_pIsolate = nullptr;
};