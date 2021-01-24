#include "StdInc.h"


// Common checks
bool CV8BaseClass::ConstructorCallCheck(const FunctionCallbackInfo<Value>& info)
{
    if (!info.IsConstructCall())
    {
        info.GetIsolate()->ThrowException(String::NewFromUtf8(info.GetIsolate(), "Cannot call constructor as function").ToLocalChecked());
        return false;
    }
    return true;
}