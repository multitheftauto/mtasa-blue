#include "StdInc.h"

// Common checks
bool CV8BaseClass::ConstructorCallCheck(const FunctionCallbackInfo<Value>& info)
{
    if (!info.IsConstructCall())
    {
        info.GetIsolate()->ThrowException(CV8Utils::ToV8String("Cannot call constructor as function"));
        return false;
    }
    return true;
}

void CV8BaseClass::AttachGC(Isolate* isolate, Local<Object> object)
{
    new JavascriptWrapper(isolate, object);
}

std::map<int, JavascriptWrapper*> JavascriptWrapper::m_pGlobals;
int                               JavascriptWrapper::i;

int JavascriptWrapper::GetGlobalsCount()
{
    return m_pGlobals.size();
}
