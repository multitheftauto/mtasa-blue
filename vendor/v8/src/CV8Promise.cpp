#include "StdInc.h"
#include <assert.h>

using namespace v8;

CV8Promise::CV8Promise(Isolate* pIsolate) : m_pIsolate(pIsolate)
{
    auto  a = m_pIsolate->GetCurrentContext();
    auto  b = Isolate::GetCurrent();
    auto& promiseResolver = Promise::Resolver::New(m_pIsolate->GetCurrentContext()).ToLocalChecked();
    m_promiseResolver.Reset(m_pIsolate, promiseResolver);

    m_pContext.Reset(m_pIsolate, m_pIsolate->GetCurrentContext());
    m_promise = promiseResolver->GetPromise();
}

CV8Promise::~CV8Promise()
{
    std::lock_guard guard(m_lock);
}

bool CV8Promise::FulFill()
{
    std::lock_guard guard(m_lock);
    if (bHasResult && m_fulfillCallback)
    {
        m_fulfillCallback();
        m_fulfillCallback = nullptr;
        return true;
    }
    return false;
}

bool CV8Promise::Execute()
{
    std::lock_guard guard(m_lock);
    if (m_callback)
    {
        m_callback(this);
        m_callback = nullptr;
        bHasResult = true;
        return true;
    }
    return false;
}

void CV8Promise::Resolve(v8::Local<v8::Value> value)
{
    v8::HandleScope scope(m_pIsolate);
    Local<Promise::Resolver> resolver = m_promiseResolver.Get(m_pIsolate);

    // Local<Promise::Resolver> resolver = m_promiseResolver.Get(m_pIsolate).As<Promise::Resolver>();
    if (resolver->GetPromise()->State() == Promise::PromiseState::kPending)
    {
        v8::Local<v8::Promise::Resolver> local = v8::Local<v8::Promise::Resolver>::New(m_pIsolate, m_promiseResolver);
        local->Resolve(m_pContext.Get(m_pIsolate), value).ToChecked();
        m_promiseResolver.Reset();
        m_pContext.Reset();
    }
}

void CV8Promise::Resolve(std::string arg)
{
    m_fulfillCallback = [this, arg]() {
        // v8::Locker     isolateLocker(m_pIsolate);
        HandleScope   scope(m_pIsolate);
        Local<String>  result = String::NewFromUtf8(m_pIsolate, arg.c_str()).ToLocalChecked();
        Resolve(result);
    };
}