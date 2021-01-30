#include "StdInc.h"
#include <queue>
#include <sstream>

using namespace v8;

static std::queue<std::string> modulesListName;            // Todo, get rid of this list

CV8Isolate::CV8Isolate(const CV8* pCV8, std::string& originResource) : m_pCV8(pCV8)
{
    m_strOriginResource = originResource;
    m_createParams.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
    m_pIsolate = Isolate::New(m_createParams);
    Locker lock(m_pIsolate);

    Isolate::Scope isolateScope(m_pIsolate);
    HandleScope    handleScope(m_pIsolate);

    m_pIsolate->SetMicrotasksPolicy(MicrotasksPolicy::kExplicit);
    m_pIsolate->SetData(0, this);
    m_global.Reset(m_pIsolate, ObjectTemplate::New(m_pIsolate));
    m_context.Reset(m_pIsolate, Context::New(m_pIsolate, nullptr, m_global.Get(m_pIsolate)));

    Context::Scope contextScope(m_context.Get(m_pIsolate));

    Handle<FunctionTemplate> vector2dTemplate = CV8Vector2D::CreateTemplate(m_pIsolate->GetCurrentContext());
    Handle<FunctionTemplate> vector3dTemplate = CV8Vector3D::CreateTemplate(m_context.Get(m_pIsolate), vector2dTemplate);
    Handle<FunctionTemplate> matrixTemplate = CV8Matrix::CreateTemplate(m_context.Get(m_pIsolate));
}

void CV8Isolate::DoPulse()
{
    m_pIsolate->PerformMicrotaskCheckpoint();
}

class CMicrotask
{
public:
    CMicrotask(std::function<void(CV8Isolate*)> microtask, CV8Isolate* isolate) : m_microtask(microtask), m_isolate(isolate) {}
    std::function<void(CV8Isolate*)> m_microtask;
    CV8Isolate*                      m_isolate;
};

void CV8Isolate::EnqueueMicrotask(std::function<void(CV8Isolate*)> microtask)
{
    Locker lock(m_pIsolate);
    m_pIsolate->EnqueueMicrotask(
        [](void* data) {
            CMicrotask*     microtask = (CMicrotask*)data;
            MicrotasksScope scope(microtask->m_isolate->GetIsolate(), MicrotasksScope::Type::kRunMicrotasks);
            microtask->m_microtask(microtask->m_isolate);
            delete microtask;
        },
        new CMicrotask(microtask, this));
}

void CV8Isolate::ReportException(TryCatch* pTryCatch)
{
    String::Utf8Value utf8(m_pIsolate, pTryCatch->Exception());
    Local<Message>    message = pTryCatch->Message();
    int               line = message->GetLineNumber(m_pIsolate->GetCurrentContext()).FromJust();
    int               column = message->GetEndColumn(m_pIsolate->GetCurrentContext()).FromJust();
    printf("%s: '%s' at %i:%i\n", m_strOriginResource.c_str(), *utf8, line, column);
}

MaybeLocal<Module> CV8Isolate::InstantiateModule(Local<Context> context, Local<String> specifier, Local<FixedArray> import_assertions, Local<Module> referrer)
{
    String::Utf8Value  importName(context->GetIsolate(), specifier);
    CV8Module*         pModule = CV8::GetModuleByName(*importName);
    MaybeLocal<Module> module;
    if (!pModule)
        return module;

    modulesListName.push(*importName);

    Local<String> moduleName = String::NewFromUtf8(context->GetIsolate(), *importName).ToLocalChecked();
    auto          exports = pModule->GetExports(context->GetIsolate());
    module = Module::CreateSyntheticModule(context->GetIsolate(), moduleName, exports, [](Local<Context> context, Local<Module> module) {
        CV8Isolate* pThisIsolate = (CV8Isolate*)context->GetIsolate()->GetData(0);
        return pThisIsolate->InitializeModuleExports(context, module);
    });
    Local<Module> checkedModule;
    if (!module.ToLocal(&checkedModule))
    {
        printf("error?");
        return module;
    }

    return checkedModule;
}

void CV8Isolate::AddPromise(std::unique_ptr<CV8Promise> pPromise)
{
    CV8Promise* promise = pPromise.get();
    m_pCV8->GetPlatform()->CallOnWorkerThread(std::make_unique<CV8Task>([promise]() { promise->Run(); }));
    m_vecPromises.push_back(std::move(pPromise));
}

MaybeLocal<Value> CV8Isolate::InitializeModuleExports(Local<Context> context, Local<Module> module)
{
    Locker      lock(m_pIsolate);
    const char* name = modulesListName.front().c_str();
    CV8Module*  pModule = CV8::GetModuleByName(name);
    for (auto const& pair : pModule->GetFunctions())
    {
        Local<Value> value = External::New(context->GetIsolate(), pair.second);

        FunctionCallback callback = [](const FunctionCallbackInfo<Value>& args) {
            Locker          lock(args.GetIsolate());
            HandleScope     handleScope(args.GetIsolate());
            Local<External> ext = args.Data().As<External>();
            CV8Isolate*     pThisIsolate = (CV8Isolate*)args.GetIsolate()->GetData(0);
            void (*func)(CV8FunctionCallbackBase*) = static_cast<void (*)(CV8FunctionCallbackBase*)>(ext->Value());
            CV8FunctionCallback callback(args);
            func(&callback);
        };

        module->SetSyntheticModuleExport(context->GetIsolate(), String::NewFromUtf8(context->GetIsolate(), pair.first).ToLocalChecked(),
                                         Function::New(context, callback, value).ToLocalChecked());
    }
    modulesListName.pop();
    return True(context->GetIsolate());
}

void CV8Isolate::RunCode(std::string& code, std::string& originFileName)
{
    m_iRunCodeCount++;
    Locker         lock(m_pIsolate);
    Isolate::Scope isolateScope(m_pIsolate);
    HandleScope    handleScope(m_pIsolate);
    Context::Scope contextScope(m_context.Get(m_pIsolate));

    Local<String> source = String::NewFromUtf8(m_pIsolate, code.c_str(), NewStringType::kNormal).ToLocalChecked();
    Local<String> fileName = String::NewFromUtf8(m_pIsolate, originFileName.c_str(), NewStringType::kNormal).ToLocalChecked();

    ScriptOrigin origin(fileName, 0, 0, false, -1, Local<Value>(), false, false, true);

    ScriptCompiler::Source compilerSource(source, origin);
    Local<Module>          module;
    TryCatch               compileTryCatch(m_pIsolate);
    if (!ScriptCompiler::CompileModule(m_pIsolate, &compilerSource).ToLocal(&module))
    {
        String::Utf8Value exception(m_pIsolate, compileTryCatch.Exception());
        Local<Message>    message = compileTryCatch.Message();
        int               line = message->GetLineNumber(m_pIsolate->GetCurrentContext()).FromJust();
        int               column = message->GetEndColumn(m_pIsolate->GetCurrentContext()).FromJust();
        std::unique_ptr<STryCatch> sTryCatch = std::make_unique<STryCatch>(originFileName, *exception, line, column);
        m_vecCompilationErrors.emplace_back(std::move(sTryCatch));
        return;
    }
    for (int i = 0; i < module->GetModuleRequestsLength(); i++)
    {
        Local<ModuleRequest> request = module->GetModuleRequests()->Get(m_context.Get(m_pIsolate), i).As<ModuleRequest>();

        Maybe<bool> result = module->InstantiateModule(m_context.Get(m_pIsolate), InstantiateModule);
        if (result.IsNothing())
        {
            String::Utf8Value str(m_pIsolate, request->GetSpecifier());
            printf("Failed to import module %s\n", *str);
        }
    }
    if (Module::Status::kInstantiated == module->GetStatus())
    {
        std::unique_ptr<SModule> smodule = std::make_unique<SModule>();
        smodule->m_module.Reset(m_pIsolate, module);
        m_vecModules.push_back(std::move(smodule));
        /*TryCatch evaluateTryCatch(m_pIsolate);
        Local<Value> returnValue;
        if (!module->Evaluate(m_context.Get(m_pIsolate)).ToLocal(&returnValue))
        {
            ReportException(&evaluateTryCatch);
            return;
        }*/
    }
}

bool CV8Isolate::GetErrorMessage(std::string& error)
{
    if (m_vecCompilationErrors.empty())
    {
        return false;
    }
    std::stringstream stream;

    stream << "[JS Error] Failed to compile " << m_vecCompilationErrors.size() << " of " << m_iRunCodeCount << " scripts in resource \"" << m_strOriginResource << "\"\n";
    for (auto const& tryCatch : m_vecCompilationErrors)
    {
        stream << "\t" << tryCatch->location << ": " << tryCatch->exception << " at " << tryCatch->line << ":" << tryCatch->column << "\n";
    }
    
    stream << '\n';
    error = stream.str();
    return true;
}

void CV8Isolate::Evaluate()
{
    for (auto const& module : m_vecModules)
    {
        TryCatch     evaluateTryCatch(m_pIsolate);
        Local<Value> returnValue;
        if (!module->m_module.Get(m_pIsolate)->Evaluate(m_context.Get(m_pIsolate)).ToLocal(&returnValue))
        {
            ReportException(&evaluateTryCatch);
        }
    }
}

CV8Isolate::~CV8Isolate()
{
    {
        Locker         lock(m_pIsolate);
        Isolate::Scope isolateScope(m_pIsolate);
        HandleScope    handleScope(m_pIsolate);
        Context::Scope contextScope(m_context.Get(m_pIsolate));

        m_global.Reset();
        m_context.Reset();

        for (auto const& module : m_vecModules)
        {
            module->m_module.Reset();
        }
    }

    delete m_createParams.array_buffer_allocator;
    m_pIsolate->Dispose();
}