#include "StdInc.h"

Local<String> CV8Utils::ToV8String(const char* szString)
{
    Isolate* isolate = Isolate::GetCurrent();
    assert(isolate);
    return String::NewFromUtf8(isolate, szString).ToLocalChecked();
}

Local<String> CV8Utils::ToV8String(std::string str)
{
    Isolate* isolate = Isolate::GetCurrent();
    assert(isolate);
    return String::NewFromUtf8(isolate, str.c_str()).ToLocalChecked();
}

Local<Number> CV8Utils::ToV8Number(double number)
{
    Isolate* isolate = Isolate::GetCurrent();
    assert(isolate);
    return Number::New(isolate, number);
}

MaybeLocal<Value> CV8Utils::GetGlobal(const char* name)
{
    Isolate* isolate = Isolate::GetCurrent();
    assert(isolate);
    MaybeLocal<Value> maybeValue = isolate->GetCurrentContext()->Global()->Get(isolate->GetCurrentContext(), CV8Utils::ToV8String(name));
    return maybeValue;
}

MaybeLocal<Value> CV8Utils::GetClassByName(const char* className)
{
    Isolate* isolate = Isolate::GetCurrent();
    assert(isolate);
    EscapableHandleScope handleScope(isolate);
    MaybeLocal<Value>    maybeValue = GetGlobal(className);
    Local<Value>         value;
    if (maybeValue.ToLocal(&value))
    {
        if (value->IsFunction())
        {
            Handle<Function> v8Class = Handle<Function>::Cast(value);
            if (v8Class->IsConstructor())
            {
                return handleScope.Escape(value);
            }
        }
    }
    
    return handleScope.EscapeMaybe(MaybeLocal<Value>());
}
