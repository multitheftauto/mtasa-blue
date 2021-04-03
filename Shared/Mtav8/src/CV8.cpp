#include "StdInc.h"

using namespace v8;

CV8::CV8()
{
}

void CV8::Initialize(int iThreadPool)
{
    V8::InitializeICUDefaultLocation(V8Config::szExecPath);
    V8::InitializeExternalStartupData(V8Config::szExternalStartupData);

    m_pPlatform = platform::NewDefaultPlatform(iThreadPool);
    V8::InitializePlatform(m_pPlatform.get());
    V8::Initialize();

#if DEBUG
    V8::SetFlagsFromString("--expose-gc --trace-opt --trace-deopt", 38);
#endif

}

CV8::~CV8()
{
}

void CV8::Shutdown()
{
    for (auto const& isolate : m_vecIsolates)
    {
        isolate->Shutdown();
    }

    V8::Dispose();
    V8::ShutdownPlatform();
}

void CV8::DoPulse()
{
    for (auto const& isolate : m_vecIsolates)
    {
        isolate->DoPulse();
    }
}

std::vector<CV8IsolateBase*> CV8::GetIsolates()
{
    std::vector<CV8IsolateBase*> isolates = std::vector<CV8IsolateBase*>();
    isolates.reserve(m_vecIsolates.size());
    for (auto const& isolate : m_vecIsolates)
    {
        isolates.push_back(isolate.get());
    }
    return isolates;
}

CV8IsolateBase* CV8::CreateIsolate(std::string originResource)
{
    std::unique_ptr<CV8Isolate> pIsolate = std::make_unique<CV8Isolate>(this, originResource);
    CV8Isolate*                 isolate = pIsolate.get();
    m_vecIsolates.push_back(std::move(pIsolate));
    return isolate;
}

void CV8::RemoveIsolate(CV8IsolateBase* pIsolate)
{
    for (auto it = m_vecIsolates.begin(); it != m_vecIsolates.end(); ++it)
    {
        if ((*it).get() == pIsolate)
        {
            platform::NotifyIsolateShutdown(m_pPlatform.get(), it->get()->GetIsolate());
            m_vecIsolates.erase(it);
            return;
        }
    }
}
