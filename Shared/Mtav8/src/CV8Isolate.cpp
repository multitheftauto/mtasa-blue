#include "StdInc.h"
#include <queue>
#include <sstream>
#include "SharedUtil.hpp"

using namespace v8;

CV8Isolate::CV8Isolate(CV8* pCV8, std::string originResource) : m_pCV8(pCV8)
{
    m_strOriginResource = originResource;
    m_createParams.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
    m_createParams.constraints = m_constraints;
    m_pIsolate = Isolate::New(m_createParams);

    Locker         lock(m_pIsolate);
    Isolate::Scope isolateScope(m_pIsolate);
    HandleScope    handleScope(m_pIsolate);

    m_pIsolate->SetMicrotasksPolicy(MicrotasksPolicy::kExplicit);
    m_pIsolate->SetData(0, this);

    m_global.Reset(m_pIsolate, ObjectTemplate::New(m_pIsolate));

    m_rootContext.Reset(m_pIsolate, Context::New(m_pIsolate, nullptr, m_global.Get(m_pIsolate)));
    Context::Scope contextScope(m_rootContext.Get(m_pIsolate));

    Local<Context> context = m_rootContext.Get(m_pIsolate);
    Local<Object>  global = context->Global();
    global->Set(context, CV8Utils::ToV8String("Version"), CV8Utils::ToV8String(V8::GetVersion()));

    InitSecurity();
}

void CV8Isolate::InitSecurity()
{
    Local<Context>        context = m_rootContext.Get(m_pIsolate);
    Local<ObjectTemplate> global = m_global.Get(m_pIsolate);
    global->Set(m_pIsolate, "WebAssembly", Undefined(m_pIsolate));
}


void CV8Isolate::DoPulse()
{
    Locker      lock(m_pIsolate);
    HandleScope handleScope(m_pIsolate);

    // Continue async await execution
    m_pIsolate->PerformMicrotaskCheckpoint();
}

void CV8Isolate::ReportException(TryCatch* pTryCatch)
{
    HandleScope handleScope(m_pIsolate);

    if (pTryCatch->HasTerminated())
    {
        printf("%s: Long execution has been termianted.\n", m_strOriginResource.c_str());
    }

    String::Utf8Value exception(m_pIsolate, pTryCatch->Exception());
    const char*       exceptionString = CV8Utils::ToString(exception);
    Local<Message>    message = pTryCatch->Message();
    if (message.IsEmpty())
    {
        // V8 didn't provide any extra information about this error; just
        // print the exception.
        fprintf(stderr, "%s\n", exceptionString);
    }
    else
    {
        Local<Context> context(m_pIsolate->GetCurrentContext());
        Local<Value>   stackTraceString;
        if (pTryCatch->StackTrace(context).ToLocal(&stackTraceString) && stackTraceString->IsString() && stackTraceString.As<String>()->Length() > 0)
        {
            String::Utf8Value stackTrace(m_pIsolate, stackTraceString);
            const char*       stackTraceString = CV8Utils::ToString(stackTrace);
            fprintf(stderr, "%s\n", stackTraceString);
        }

        v8::String::Utf8Value fileName(m_pIsolate, message->GetScriptOrigin().ResourceName());
        const char*           fileNameString = CV8Utils::ToString(fileName);
        int                   lineNum = message->GetLineNumber(context).FromJust();
        fprintf(stderr, "%s:%i: %s\n", fileNameString, lineNum, exceptionString);

        // Print line of source code.
        String::Utf8Value sourceLine(m_pIsolate, message->GetSourceLine(context).ToLocalChecked());
        const char*       sourceLineString = CV8Utils::ToString(sourceLine);
        fprintf(stderr, "\n%s\n", sourceLineString);
        // Print wavy underline.
        int start = message->GetStartColumn(context).FromJust();
        for (int i = 0; i < start; i++)
        {
            fprintf(stderr, " ");
        }
        int end = message->GetEndColumn(context).FromJust();
        for (int i = start; i < end; i++)
        {
            fprintf(stderr, "^");
        }
        fprintf(stderr, "\n");
    }
}

bool CV8Isolate::RunCode(std::string& code, std::string originFileName)
{
    m_iRunCodeCount++;
    m_strCurrentOriginFileName = originFileName;

    Local<String>          source = CV8Utils::ToV8String(code);
    Local<String>          fileName = CV8Utils::ToV8String(originFileName);

    v8::HandleScope        handle_scope(m_pIsolate);
    v8::TryCatch           tryCatch(m_pIsolate);
    v8::ScriptOrigin       origin(fileName);
    v8::Local<v8::Context> context(m_pIsolate->GetCurrentContext());
    v8::Local<v8::Script>  script;


    ScriptOrigin           origin(fileName, 0, 0, false, -1, Local<Value>(), false, false, true);
    ScriptCompiler::Source compilerSource(source, origin);
    Local<Module>          module;
    TryCatch               compileTryCatch(m_pIsolate);
    if (!Script::Compile(context, source, &origin).ToLocal(&script))
    {
        ReportException(&tryCatch);
        return false;
    }
    return true;
}

void CV8Isolate::Shutdown()
{
}
CV8Isolate::~CV8Isolate()
{
    {
        Locker         lock(m_pIsolate);
        Isolate::Scope isolateScope(m_pIsolate);
        HandleScope    handleScope(m_pIsolate);
        Local<Context> thisContext = m_rootContext.Get(m_pIsolate);

        for (auto& [classId, classInstance] : m_classInstances)
        {
            classInstance.Empty();
        }

        m_global.Reset();
        m_rootContext.Reset();
    }

    delete m_createParams.array_buffer_allocator;
    m_pIsolate->Dispose();
}
