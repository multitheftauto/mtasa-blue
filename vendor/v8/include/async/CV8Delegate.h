#include "CV8DelegateBase.h"

class CV8Delegate : public CV8DelegateBase
{
public:
    CV8Delegate(CV8Promise* pPromise) : m_pPromise(pPromise) {}
    void Resolve(std::string value) { m_pPromise->Resolve(value); }

private:
    CV8Promise* m_pPromise;
};
