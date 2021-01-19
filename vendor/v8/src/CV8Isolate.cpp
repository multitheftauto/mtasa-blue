#include "StdInc.h"

using namespace v8;

CV8Isolate::CV8Isolate(std::string& originResource)
{
    m_createParams.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
    m_pIsolate = Isolate::New(m_createParams);
    m_strOriginResource = originResource;
}

void CV8Isolate::ReportException(v8::TryCatch* pTryCatch)
{
    String::Utf8Value      utf8(m_pIsolate, pTryCatch->Exception());
    v8::Local<v8::Message> message = pTryCatch->Message();
    int                    line = message->GetLineNumber(m_pIsolate->GetCurrentContext()).FromJust();
    int                    column = message->GetEndColumn(m_pIsolate->GetCurrentContext()).FromJust();
    printf("%s: '%s' at %i:%i\n", m_strOriginResource.c_str(), *utf8, line, column);
}

void Foo(const FunctionCallbackInfo<Value>& args)
{
    Local<String> returnStr = String::NewFromUtf8(args.GetIsolate(), "FOOO", NewStringType::kNormal).ToLocalChecked();

    args.GetReturnValue().Set(returnStr);
}

MaybeLocal<Value> TestModule(Local<Context> context, Local<Module> module)
{
    module->SetSyntheticModuleExport(String::NewFromUtf8(context->GetIsolate(), "foo").ToLocalChecked(), v8::Function::New(context, Foo).ToLocalChecked());

    return True(context->GetIsolate());
}

void CV8Isolate::RunCode(std::string& code, bool bAsModule)
{
    Isolate::Scope isolate_scope(m_pIsolate);
    // Create a stack-allocated handle scope.
    HandleScope handle_scope(m_pIsolate);

    // Create a new context.
    // Local<Context> context = Context::New(isolate);
    // Enter the context for compiling and running the hello world script.

    Local<String> mtaModuleName = String::NewFromUtf8(m_pIsolate, "mta").ToLocalChecked();
    // Local<Module> module = Module::CreateSyntheticModule(m_pIsolate, mtaModuleName, {String::NewFromUtf8(m_pIsolate, "foo").ToLocalChecked()}, AddMtaModule);

    Local<ObjectTemplate> global = ObjectTemplate::New(m_pIsolate);

    Local<Context> context = Context::New(m_pIsolate, nullptr, global);
    // std::vector<Local<String>> names = {String::NewFromUtf8(m_pIsolate, "foo").ToLocalChecked()};
    // Local<Module>              module = Module::CreateSyntheticModule(m_pIsolate, mtaModuleName, names, TestModule);

    Context::Scope context_scope(context);

    // Create a string containing the JavaScript source code.
    Local<String> source = String::NewFromUtf8(m_pIsolate, code.c_str(), NewStringType::kNormal).ToLocalChecked();
    Local<String> resourceName = String::NewFromUtf8(m_pIsolate, m_strOriginResource.c_str(), NewStringType::kNormal).ToLocalChecked();
    // Compile the source code.

    {
        v8::TryCatch compileTryCatch(m_pIsolate);
        if (bAsModule)
        {
            ScriptOrigin origin(resourceName, 0, 0, false, -1, Local<Value>(), false, false, true);

            ScriptCompiler::Source compilerSource(source, origin);
            Local<Module>          module;
            if (!ScriptCompiler::CompileModule(m_pIsolate, &compilerSource).ToLocal(&module))
            {
                ReportException(&compileTryCatch);
                return;
            }
            Local<FixedArray> requests = module->GetModuleRequests();
            for (int i = 0; i < requests->Length(); i++)
            {
                Local<ModuleRequest> request = requests->Get(context, i).As<ModuleRequest>();
                String::Utf8Value    utf8(m_pIsolate, request->GetSpecifier());
                printf("Attempt to import: %s\n", *utf8);
            }
        }
        else
        {
            ScriptOrigin          origin(resourceName);
            v8::Local<v8::Script> script;

            if (!Script::Compile(context, source, &origin).ToLocal(&script))
            {
                ReportException(&compileTryCatch);
                return;
            }
        }
    }

    // if (result.IsEmpty())
    // Local<Value> result = script->Run(context).ToLocalChecked();
    //// Convert the result to an UTF8 string and print it.
    // String::Utf8Value utf8(m_pIsolate, result);
    // printf("%s\n", *utf8);
}

CV8Isolate::~CV8Isolate()
{
    m_pIsolate->Dispose();

    delete m_createParams.array_buffer_allocator;
}