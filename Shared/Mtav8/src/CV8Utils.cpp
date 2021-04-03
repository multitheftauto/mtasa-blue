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

const char* CV8Utils::ToString(const String::Utf8Value& value)
{
    return *value ? *value : "<string conversion failed>";
}
