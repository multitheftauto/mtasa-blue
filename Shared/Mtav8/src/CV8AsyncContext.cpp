#include "StdInc.h"

CV8AsyncContext::CV8AsyncContext(CV8Promise* pPromise) : m_pPromise(pPromise)
{

}

void CV8AsyncContext::Resolve(std::string value)
{
    m_pPromise->Resolve(value);
}
