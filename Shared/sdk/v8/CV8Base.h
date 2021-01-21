#pragma once

#include "CV8FunctionCallbackBase.h"
#include "CV8IsolateBase.h"
#include "CV8ModuleBase.h"

class CV8Base
{
public:
    virtual CV8IsolateBase* CreateIsolate(std::string& strCode, std::string& originResource) = 0;
    virtual CV8ModuleBase*  CreateModule(const char* name) = 0;
};
