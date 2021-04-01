#pragma once

#include "CV8PromiseBase.h"
#include "CV8FunctionCallbackBase.h"
#include "CV8IsolateBase.h"
#include "CV8ModuleBase.h"
#include "CV8ExportClassBase.h"
#include "CV8ExportObjectBase.h"
#include "CV8ExportEnumBase.h"
#include "CV8AsyncContextBase.h"

class CV8Base
{
public:
    virtual CV8IsolateBase* CreateIsolate(std::string originResource) = 0;
    virtual void            RemoveIsolate(CV8IsolateBase* pIsolate) = 0;

    virtual CV8ModuleBase*       CreateModule(std::string name) = 0;
    virtual CV8ExportEnumBase*         CreateEnum() = 0;
    virtual CV8ExportObjectBase* CreateExportObject() = 0;

    virtual CV8ExportClassBase* CreateClass(std::string name, size_t classId) = 0;

    virtual void DoPulse() = 0;
    virtual void Initialize(int iThreadPool) = 0;
    virtual void Shutdown() = 0;

    virtual std::vector<CV8IsolateBase*> GetIsolates() = 0;

    virtual void SetExecutionTimeLimit(int iTime) = 0;
};
