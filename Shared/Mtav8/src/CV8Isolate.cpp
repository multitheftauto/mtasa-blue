#include "StdInc.h"
#include <queue>
#include <sstream>
#include "SharedUtil.hpp"

using namespace v8;

void Print(const FunctionCallbackInfo<Value>& args)
{
    bool first = true;
    for (int i = 0; i < args.Length(); i++)
    {
        HandleScope handle_scope(args.GetIsolate());
        if (first)
        {
            first = false;
        }
        else
        {
            printf(" ");
        }
        String::Utf8Value str(args.GetIsolate(), args[i]);
        const char*       cstr = CV8Utils::ToString(str);
        printf("%s", cstr);
    }
    printf("\n");
    fflush(stdout);
}

CV8Isolate::CV8Isolate(CV8* pCV8, std::string originResource) : m_pCV8(pCV8)
{
    m_strOriginResource = originResource;

    m_createParams.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
    m_pIsolate = Isolate::New(m_createParams);

    Isolate::Scope isolateScope(m_pIsolate);
    HandleScope    handleScope(m_pIsolate);

    m_pIsolate->SetMicrotasksPolicy(MicrotasksPolicy::kExplicit);
    m_pIsolate->SetData(0, this);

    Local<ObjectTemplate> global = ObjectTemplate::New(m_pIsolate);
    m_global.Reset(m_pIsolate, global);
    global->Set(m_pIsolate, "version", CV8Utils::ToV8String(V8::GetVersion()));
    global->Set(m_pIsolate, "print", FunctionTemplate::New(m_pIsolate, Print));

    Local<Context> context = Context::New(m_pIsolate, nullptr, global);

    m_rootContext.Reset(m_pIsolate, context);
    Context::Scope contextScope(context);

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
    HandleScope handleScope(m_pIsolate);

    // Continue async await execution
    m_pIsolate->PerformMicrotaskCheckpoint();
}

void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch)
{
    v8::String::Utf8Value  exception(isolate, try_catch->Exception());
    const char*            exception_string = CV8Utils::ToString(exception);
    v8::Local<v8::Message> message = try_catch->Message();
    if (message.IsEmpty())
    {
        // V8 didn't provide any extra information about this error; just
        // print the exception.
        fprintf(stderr, "%s\n", exception_string);
    }
    else
    {
        // Print (filename):(line number): (message).
        v8::String::Utf8Value  filename(isolate, message->GetScriptOrigin().ResourceName());
        v8::Local<v8::Context> context(isolate->GetCurrentContext());
        const char*            filename_string = CV8Utils::ToString(filename);
        int                    linenum = message->GetLineNumber(context).FromJust();
        fprintf(stderr, "%s:%i: %s\n", filename_string, linenum, exception_string);
        // Print line of source code.
        v8::String::Utf8Value sourceline(isolate, message->GetSourceLine(context).ToLocalChecked());
        const char*           sourceline_string = CV8Utils::ToString(sourceline);
        fprintf(stderr, "%s\n", sourceline_string);
        // Print wavy underline (GetUnderline is deprecated).
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
        v8::Local<v8::Value> stack_trace_string;
        if (try_catch->StackTrace(context).ToLocal(&stack_trace_string) && stack_trace_string->IsString() && stack_trace_string.As<v8::String>()->Length() > 0)
        {
            v8::String::Utf8Value stack_trace(isolate, stack_trace_string);
            const char*           stack_trace_string = CV8Utils::ToString(stack_trace);
            fprintf(stderr, "%s\n", stack_trace_string);
        }
    }
}

bool CV8Isolate::RunCode(std::string& code, std::string originFileName)
{
    m_strCurrentOriginFileName = originFileName;

    Isolate::Scope isolateScope(m_pIsolate);
    HandleScope    handleScope(m_pIsolate);

    Local<String> source = CV8Utils::ToV8String(code);
    Local<String> fileName = CV8Utils::ToV8String(originFileName);

    TryCatch       tryCatchCompile(m_pIsolate);
    ScriptOrigin   origin(fileName);
    Local<Context> context = m_rootContext.Get(m_pIsolate);

    Context::Scope context_scope(context);

    ScriptCompiler::Source compilerSource(source, origin);
    TryCatch               compileTryCatch(m_pIsolate);
    Local<Script>          script;
    if (!Script::Compile(context, source, &origin).ToLocal(&script))
    {
        ReportException(m_pIsolate, &tryCatchCompile);
        return false;
    }
    TryCatch     tryCatchRun(m_pIsolate);
    Local<Value> result;
    if (!script->Run(context).ToLocal(&result))
    {
        ReportException(m_pIsolate, &tryCatchRun);
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
