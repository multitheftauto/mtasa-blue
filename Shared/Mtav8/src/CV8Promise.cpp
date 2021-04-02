#include "StdInc.h"
#include <assert.h>

using namespace v8;

CV8Promise::CV8Promise(CV8Isolate* pIsolate, std::function<void(CV8AsyncContextBase*)> pFunctionAsyncCallback)
    : m_pIsolate(pIsolate), m_pFunctionAsyncCallback(pFunctionAsyncCallback)
{
    Isolate*                 isolate = pIsolate->GetIsolate();
    Local<Promise::Resolver> promiseResolver = Promise::Resolver::New(isolate->GetCurrentContext()).ToLocalChecked();
    m_promiseResolver.Reset(isolate, promiseResolver);
    m_pContext.Reset(isolate, isolate->GetCurrentContext());
}

CV8Promise::~CV8Promise()
{
    if (m_pCancelationToken)
        m_pCancelationToken->Cancel();

    // If something interrupted execution, eg resource restart
    if (!bHasResult)
    {
        Reject();
    }
}

void CV8Promise::Run()
{
    bool pending = false;
    {
        Locker         lock(m_pIsolate->GetIsolate());

        pending = IsPending();
    }
    if (pending)
    {
        if (!m_pCancelationToken->IsCanceled())
        {
            CV8AsyncContext context(this);
            m_pFunctionAsyncCallback(&context);
            assert(context.HasResult());            // Resolve, Reject must be called for each path.
        }
    }
};

bool CV8Promise::IsPending()
{
    Locker         lock(m_pIsolate->GetIsolate());
    Isolate::Scope scope(m_pIsolate->GetIsolate());
    HandleScope    handleScope(m_pIsolate->GetIsolate());

    Local<Promise::Resolver> resolver = m_promiseResolver.Get(m_pIsolate->GetIsolate());
    if (!resolver.IsEmpty())
    {
        return resolver->GetPromise()->State() == Promise::PromiseState::kPending;
    }
    return false;
}

bool CV8Promise::IsFulfilled()
{
    if (m_pCancelationToken->IsCanceled())
        return false;

    Local<Promise::Resolver> resolver = m_promiseResolver.Get(m_pIsolate->GetIsolate());
    if (!resolver.IsEmpty())
    {
        return resolver->GetPromise()->State() == Promise::PromiseState::kFulfilled;
    }
    return false;
}

bool CV8Promise::IsRejected()
{
    if (m_pCancelationToken->IsCanceled())
        return false;

    Local<Promise::Resolver> resolver = m_promiseResolver.Get(m_pIsolate->GetIsolate());
    if (!resolver.IsEmpty())
    {
        return resolver->GetPromise()->State() == Promise::PromiseState::kRejected;
    }
    return true;
}

void CV8Promise::Resolve(Local<Value> value)
{
    if (IsPending())
    {
        Local<Promise::Resolver> resolver = Local<Promise::Resolver>::New(m_pIsolate->GetIsolate(), m_promiseResolver);
        resolver->Resolve(m_pContext.Get(m_pIsolate->GetIsolate()), value).ToChecked();
        m_promiseResolver.Reset();
        m_pContext.Reset();
        bHasResult = true;
    }
}

void CV8Promise::Reject()
{
    Locker lock(m_pIsolate->GetIsolate());
    m_pIsolate->EnqueueMicrotask([this](CV8Isolate* pIsolate) {
        Isolate*       isolate = pIsolate->GetIsolate();
        Isolate::Scope scope(isolate);
        Reject(Undefined(isolate));
    });
}

void CV8Promise::Reject(Local<Value> value)
{

}

void CV8Promise::RejectUndefined()
{
    if (IsPending())
    {
        Local<Promise::Resolver> resolver = Local<Promise::Resolver>::New(m_pIsolate->GetIsolate(), m_promiseResolver);
        resolver->Reject(m_pContext.Get(m_pIsolate->GetIsolate()), Undefined(m_pIsolate->GetIsolate())).ToChecked();
        m_promiseResolver.Reset();
        m_pContext.Reset();
        bHasResult = true;
    }
}

void CV8Promise::Resolve(std::string arg)
{
    Locker lock(m_pIsolate->GetIsolate());
    m_pIsolate->EnqueueMicrotask([arg, this](CV8Isolate* pIsolate) {
        Isolate*       isolate = pIsolate->GetIsolate();
        Isolate::Scope scope(isolate);
        Resolve(CV8Utils::ToV8String(arg));
    });
}

void CV8Promise::Resolve()
{
    Locker lock(m_pIsolate->GetIsolate());
    m_pIsolate->EnqueueMicrotask([this](CV8Isolate* pIsolate) {
        Isolate*       isolate = pIsolate->GetIsolate();
        Isolate::Scope scope(isolate);
        Resolve(Undefined(isolate));
    });
}
