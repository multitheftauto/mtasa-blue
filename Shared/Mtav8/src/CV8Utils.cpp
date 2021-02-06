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
