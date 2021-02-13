#include "StdInc.h"
#include <assert.h>

using namespace v8;

CV8Promise::CV8Promise(CV8Isolate* pIsolate, std::unique_ptr<CV8AsyncFunction> pAsyncFunction)
    : m_pIsolate(pIsolate), m_pAsyncFunction(std::move(pAsyncFunction))
{
    Isolate* isolate = pIsolate->GetIsolate();
    Local<Promise::Resolver>    promiseResolver = Promise::Resolver::New(isolate->GetCurrentContext()).ToLocalChecked();
    m_promiseResolver.Reset(isolate, promiseResolver);
    m_pContext.Reset(isolate, isolate->GetCurrentContext());
}

CV8Promise::~CV8Promise()
{
}

void CV8Promise::Run()
{
    std::unique_ptr<CV8Delegate> delegate = std::make_unique<CV8Delegate>(this);
    m_pAsyncFunction->Run((CV8DelegateBase*)delegate.get());
};

void CV8Promise::Resolve(Local<Value> value)
{
    Local<Promise::Resolver> resolver = m_promiseResolver.Get(m_pIsolate->GetIsolate());

    // Local<Promise::Resolver> resolver = m_promiseResolver.Get(m_pIsolate).As<Promise::Resolver>();
    if (resolver->GetPromise()->State() == Promise::PromiseState::kPending)
    {
        Local<Promise::Resolver> local = Local<Promise::Resolver>::New(m_pIsolate->GetIsolate(), m_promiseResolver);
        local->Resolve(m_pContext.Get(m_pIsolate->GetIsolate()), value).ToChecked();
        m_promiseResolver.Reset();
        m_pContext.Reset();
    }
}

void CV8Promise::Resolve(std::string arg)
{
    m_pIsolate->EnqueueMicrotask([arg, this](CV8Isolate* pIsolate) {
        Isolate*    isolate = pIsolate->GetIsolate();
        Locker      lock(isolate);
        HandleScope scope(isolate);
        Resolve(CV8Utils::ToV8String(arg));
    });
}
