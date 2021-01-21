#include "StdInc.h"
#include <functional>
using namespace v8;

static std::string g_currentModuleName;

void MtaPrint(const FunctionCallbackInfo<Value>& args)
{
    String::Utf8Value text(args.GetIsolate(), args[0].As<String>());
    printf("%s\n", *text);

    args.GetReturnValue().Set(True(args.GetIsolate()));
}

MaybeLocal<Value> InitializeModuleExports(Local<Context> context, Local<Module> module)
{
    CV8Module* pModule = CV8::GetModuleByName(g_currentModuleName.c_str());
    for (auto const& pair : pModule->GetFunctions())
    {
        static void (*g_currentFunction)(CV8FunctionCallbackBase*) = pair.second;
        module->SetSyntheticModuleExport(String::NewFromUtf8(context->GetIsolate(), pair.first).ToLocalChecked(),
                                         v8::Function::New(context, [](const FunctionCallbackInfo<Value>& args) {
                                             CV8FunctionCallback callback(args);
                                             g_currentFunction(&callback);
                                         }).ToLocalChecked());
    }
    return True(context->GetIsolate());
}

MaybeLocal<Value> TestModule(Local<Context> context, Local<Module> module)
{
    module->SetSyntheticModuleExport(String::NewFromUtf8(context->GetIsolate(), "print").ToLocalChecked(),
                                     v8::Function::New(context, MtaPrint).ToLocalChecked());
    // module->SetSyntheticModuleExport(String::NewFromUtf8(context->GetIsolate(), "PI").ToLocalChecked(), v8::Number::New(context->GetIsolate(), 3.14159));

    return True(context->GetIsolate());
}

MaybeLocal<Module> CV8Isolate::Resolve(Local<Context> context, Local<String> specifier, Local<Module> referrer)
{
    String::Utf8Value importName(context->GetIsolate(), specifier);

    if (!strcmp(*importName, "@mta"))
    {
        printf("Importing: %s\n", *importName);
        Local<String> mtaModuleName = String::NewFromUtf8(context->GetIsolate(), "@mta").ToLocalChecked();
        return Module::CreateSyntheticModule(
            context->GetIsolate(), mtaModuleName,
            {String::NewFromUtf8(context->GetIsolate(), "print").ToLocalChecked(), String::NewFromUtf8(context->GetIsolate(), "PI").ToLocalChecked()},
            TestModule);
    }

    MaybeLocal<Module> empty;
    return empty;
}

CV8Isolate::CV8Isolate(const CV8* pCV8, std::string& originResource) : m_pCV8(pCV8)
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

void CV8Isolate::RunCode(std::string& code, bool bAsModule)
{
    Isolate::Scope isolate_scope(m_pIsolate);
    // Create a stack-allocated handle scope.
    HandleScope handle_scope(m_pIsolate);

    Local<ObjectTemplate> global = ObjectTemplate::New(m_pIsolate);

    Local<Context> context = Context::New(m_pIsolate, nullptr, global);

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
            for (int i = 0; i < module->GetModuleRequestsLength(); i++)
            {
                Local<ModuleRequest>  request = module->GetModuleRequests()->Get(context, i).As<ModuleRequest>();
                v8::String::Utf8Value str(context->GetIsolate(), request->GetSpecifier());

                v8::Maybe<bool> result = module->InstantiateModule(
                    context, [](Local<Context> context, Local<String> specifier, Local<FixedArray> import_assertions, Local<Module> referrer) {
                        String::Utf8Value  importName(context->GetIsolate(), specifier);
                        CV8Module*         pModule = CV8::GetModuleByName(*importName);
                        MaybeLocal<Module> module;
                        if (!pModule)
                            return module;

                        g_currentModuleName = *importName;
                        Local<String> moduleName = String::NewFromUtf8(context->GetIsolate(), *importName).ToLocalChecked();
                        auto          exports = pModule->GetExports(context->GetIsolate());
                        module = Module::CreateSyntheticModule(context->GetIsolate(), moduleName, exports,
                                                               InitializeModuleExports);

                        return module;
                    });
                if (result.IsNothing())
                {
                    printf("Failed to import module %s\n", *str);
                }
            }
            if (Module::Status::kInstantiated == module->GetStatus())
            {
                module->Evaluate(context);
            }
        }
        else
        {
            ScriptOrigin  origin(resourceName);
            Local<Script> script;

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