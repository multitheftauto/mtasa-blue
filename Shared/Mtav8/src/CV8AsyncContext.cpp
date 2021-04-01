#include "StdInc.h"

CV8AsyncContext::CV8AsyncContext(CV8Promise* pPromise) : m_pPromise(pPromise), m_pCancelationToken(m_pPromise->GetCancelationToken())
{
}

void CV8AsyncContext::Resolve(std::string value)
{
    assert(!hasResult);
    if (!m_pCancelationToken->IsCanceled()  && m_pPromise->IsPending())
    {
        m_pPromise->Resolve(value);
    }
    hasResult = true;
}
