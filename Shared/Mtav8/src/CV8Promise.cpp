#include "StdInc.h"
#include <assert.h>

using namespace v8;

CV8Promise::CV8Promise(CV8Isolate* pIsolate, std::function<void(CV8AsyncContextBase*)> pFunctionAsyncCallback)
    : m_pIsolate(pIsolate), m_pFunctionAsyncCallback(pFunctionAsyncCallback)
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
    CV8AsyncContext context(this);
    m_pFunctionAsyncCallback(&context);
    int a = 5;
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
    Locker lock(m_pIsolate->GetIsolate());
    m_pIsolate->EnqueueMicrotask([arg, this](CV8Isolate* pIsolate) {
        Isolate*        isolate = pIsolate->GetIsolate();
        Isolate::Scope  scope(isolate);
        Resolve(CV8Utils::ToV8String(arg));
    });
}
