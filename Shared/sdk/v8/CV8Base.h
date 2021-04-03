#pragma once

#include "CV8IsolateBase.h"

class CV8Base
{
public:
    virtual CV8IsolateBase* CreateIsolate(std::string originResource) = 0;
    virtual void            RemoveIsolate(CV8IsolateBase* pIsolate) = 0;

    virtual void DoPulse() = 0;
    virtual void Initialize() = 0;
    virtual void Shutdown() = 0;

    virtual std::vector<CV8IsolateBase*> GetIsolates() = 0;
};
