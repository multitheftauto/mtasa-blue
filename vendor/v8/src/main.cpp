#include "StdInc.h"
#include "SharedUtil.Defines.h"

using namespace v8;

const char* ToCString(const String::Utf8Value& value)
{
    return *value ? *value : "<string conversion failed>";
}

void Foo(const FunctionCallbackInfo<Value>& args)
{
    Local<String> returnStr = String::NewFromUtf8(args.GetIsolate(), "FOOO", NewStringType::kNormal).ToLocalChecked();

    args.GetReturnValue().Set(returnStr);
}

MTAEXPORT CV8Base* Run()
{
    CV8* v8 = new CV8();
    std::string code("typeof NaN = number");
    std::string origin("patrikCode.js");
    v8->CreateIsolate(code, origin);
    return v8;
}