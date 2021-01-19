#include "StdInc.h"

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

CV8::CV8()
{
    V8::InitializeICUDefaultLocation("");
    V8::InitializeExternalStartupData("");
    // V8::InitializeICUDefaultLocation(argv[0]);
    // V8::InitializeExternalStartupData(argv[0]);

    m_pPlatform = platform::NewSingleThreadedDefaultPlatform();
    V8::InitializePlatform(m_pPlatform.get());
    V8::Initialize();
}

CV8::~CV8()
{
    V8::Dispose();
    V8::ShutdownPlatform();
}

CV8IsolateBase* CV8::CreateIsolate(std::string& strCode, std::string& originResource)
{
    CV8Isolate* pIsolate = new CV8Isolate(originResource);
    pIsolate->RunCode(strCode);
    return pIsolate;
}
