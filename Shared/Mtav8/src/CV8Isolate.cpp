#include "StdInc.h"
#include <queue>
#include <sstream>
#include "SharedUtil.hpp"

using namespace v8;

static std::queue<std::string> modulesListName;            // Todo, get rid of this list

class PHV : public PersistentHandleVisitor
{
public:
    Isolate* isolate_;

    PHV(Isolate* isolate) : isolate_(isolate) {}
    ~PHV() {}

    void VisitPersistentHandle(Persistent<Value>* value, uint16_t class_id)
    {
        printf("VISIT\n");
        // delete persistent handles on isolate disposal.
        // if (class_id == HC_GARBAGE_COLLECTED_CLASS_ID)
        //{
        //    HandleScope hs(isolate_);
        //    Wrapper*        w =            // extract your wrapped object from
        //                                   // the passed-in value object.
        //        delete w;
        //}
    }
};

CV8Isolate::CV8Isolate(CV8* pCV8, std::string& originResource) : m_pCV8(pCV8)
{
    // m_constraints.ConfigureDefaults(1024 * 1024 * 200, 0);
    m_constraints.ConfigureDefaultsFromHeapSize(1024 * 1024 * 64, 1024 * 1024 * 66);
    m_strOriginResource = originResource;
    m_createParams.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
    m_createParams.constraints = m_constraints;
    m_pIsolate = Isolate::New(m_createParams);
    Locker lock(m_pIsolate);

    Isolate::Scope isolateScope(m_pIsolate);
    HandleScope    handleScope(m_pIsolate);

    m_pIsolate->SetMicrotasksPolicy(MicrotasksPolicy::kExplicit);
    m_pIsolate->SetData(0, this);

    /*using NearHeapLimitCallback = size_t (*)(void* data, size_t current_heap_limit, size_t initial_heap_limit);*/
    m_pIsolate->AddNearHeapLimitCallback(
        [](void* data, size_t current_heap_limit, size_t initial_heap_limit) {
            CV8Isolate* pIsolate = (CV8Isolate*)data;
            int         increaseMB = 1024 * 1024 * 64;
            printf("[V8 Debug] Heap limit reached. New limit: %iMB, Initial limit: %iMB Globals count: %i\n", (current_heap_limit + increaseMB) / 1024 / 1024,
                   initial_heap_limit / 1024 / 1024, JavascriptWrapper::GetGlobalsCount());
            //((Isolate*)data)->RequestInterrupt([](Isolate* isolate, void* data) { isolate->TerminateExecution(); }, nullptr);
            return (size_t)current_heap_limit + increaseMB;
        },
        this);

    m_pIsolate->EnableMemorySavingsMode();
    m_pIsolate->VisitWeakHandles(new PHV(m_pIsolate));
    m_pIsolate->VisitHandlesWithClassIds(new PHV(m_pIsolate));
    m_global.Reset(m_pIsolate, ObjectTemplate::New(m_pIsolate));

    m_context.Reset(m_pIsolate, Context::New(m_pIsolate, nullptr, m_global.Get(m_pIsolate)));
    Context::Scope contextScope(m_context.Get(m_pIsolate));

    Local<Context> context = m_context.Get(m_pIsolate);
    Local<Object>  global = context->Global();

    InitSecurity();

    Handle<FunctionTemplate> vector2dTemplate = CV8Vector2D::CreateTemplate(context);
    Handle<FunctionTemplate> vector3dTemplate = CV8Vector3D::CreateTemplate(context, vector2dTemplate);
    Handle<FunctionTemplate> vector4dTemplate = CV8Vector4D::CreateTemplate(context, vector3dTemplate);
    Handle<FunctionTemplate> matrixTemplate = CV8Matrix::CreateTemplate(context);
}

void CV8Isolate::InitSecurity()
{
    Local<Context> context = m_context.Get(m_pIsolate);
    Local<Object>  global = context->Global();
    global->Set(context, CV8Utils::ToV8String("WebAssembly"), Undefined(m_pIsolate));
}
void CV8Isolate::DoPulse()
{
    m_pIsolate->PerformMicrotaskCheckpoint();
}

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
    HandleScope handle_scope(m_pIsolate);

    if (pTryCatch->HasTerminated())
    {
        printf("%s: Long execution has been termianted.\n", m_strOriginResource.c_str());
        return;
    }
    String::Utf8Value utf8(m_pIsolate, pTryCatch->Exception());
    Local<Message>    message = pTryCatch->Message();
    if (!message.IsEmpty())
    {
        int line = message->GetLineNumber(m_pIsolate->GetCurrentContext()).FromJust();
        int column = message->GetEndColumn(m_pIsolate->GetCurrentContext()).FromJust();
        printf("%s: '%s' at %i:%i\n", m_strOriginResource.c_str(), *utf8, line, column);
    }
    else
    {
        printf("%s: '%s'\n", m_strOriginResource.c_str(), *utf8);
    }
    Local<Value> stackTraceStringV8;
    if (pTryCatch->StackTrace(m_pIsolate->GetCurrentContext()).ToLocal(&stackTraceStringV8))
    {
        if (stackTraceStringV8->IsString() && Local<String>::Cast(stackTraceStringV8)->Length() > 0)
        {
            String::Utf8Value stackTrace(m_pIsolate, stackTraceStringV8);
            std::string       strStackTrace = std::string(*stackTrace);
            strStackTrace.erase(0, strStackTrace.find("\n") + 1);
            printf("%s\n", strStackTrace.c_str());
        }
    }
}

MaybeLocal<Module> CV8Isolate::InstantiateModule(Local<Context> context, Local<String> specifier, Local<FixedArray> import_assertions, Local<Module> referrer)
{
    Isolate*           pIsolate = context->GetIsolate();
    CV8Isolate*        self = (CV8Isolate*)pIsolate->GetData(0);
    String::Utf8Value  importName(pIsolate, specifier);
    CV8Module*         pModule = CV8::GetModuleByName(*importName);
    MaybeLocal<Module> module;
    if (!strcmp(*importName, V8Config::szMtaModulePrefix))
    {
        CV8::RegisterAllModules(this);
        return CV8::GetDummyModule(pIsolate);
    }

    if (!strcmp(*importName, V8Config::szMtaImportAllFunctions))
    {
        CV8::RegisterAllModulesInGlobalNamespace(this);
        return CV8::GetDummyModule(pIsolate);
    }

    if (!pModule)
    {
        Local<Module> scriptModule;
        if (!self->GetScriptModule(*importName).ToLocal(&scriptModule))
        {
            modulesListName.push(*importName);
            return module;
        }
        return scriptModule;
    }
    Local<String> moduleName = CV8Utils::ToV8String(*importName);
    auto          exports = pModule->GetExports(pIsolate);
    module = Module::CreateSyntheticModule(pIsolate, moduleName, exports, [](Local<Context> context, Local<Module> module) {
        CV8Isolate* self = (CV8Isolate*)context->GetIsolate()->GetData(0);
        return self->InitializeModuleExports(context, module);
    });
    Local<Module> checkedModule;
    if (!module.ToLocal(&checkedModule))
    {
        printf("error?");
        return module;
    }

    modulesListName.push(*importName);

    return checkedModule;
}

MaybeLocal<Module> CV8Isolate::GetScriptModule(const char* name)
{
    MaybeLocal<Module> module;
    auto const&        result = m_mapScriptModules.find(name);

    if (result != m_mapScriptModules.end())
    {
        module = result->second.Get(m_pIsolate);
    }
    return module;
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
    for (auto const& [import, callback] : pModule->GetFunctions())
    {
        Local<Function> function = CreateFunction(callback);
        module->SetSyntheticModuleExport(context->GetIsolate(), CV8Utils::ToV8String(import), function);
    }
    modulesListName.pop();
    return True(context->GetIsolate());
}

void CV8Isolate::RunCode(std::string& code, std::string& originFileName)
{
    m_iRunCodeCount++;
    m_strCurrentOriginFileName = originFileName;
    Locker         lock(m_pIsolate);
    Isolate::Scope isolateScope(m_pIsolate);
    HandleScope    handleScope(m_pIsolate);
    Context::Scope contextScope(m_context.Get(m_pIsolate));

    Local<String> source = CV8Utils::ToV8String(code);
    Local<String> fileName = CV8Utils::ToV8String(originFileName);

    ScriptOrigin           origin(fileName, 0, 0, false, -1, Local<Value>(), false, false, true);
    ScriptCompiler::Source compilerSource(source, origin);
    Local<Module>          module;
    TryCatch               compileTryCatch(m_pIsolate);

    if (!ScriptCompiler::CompileModule(m_pIsolate, &compilerSource).ToLocal(&module))
    {
        String::Utf8Value          exception(m_pIsolate, compileTryCatch.Exception());
        Local<Message>             message = compileTryCatch.Message();
        int                        line = message->GetLineNumber(m_pIsolate->GetCurrentContext()).FromJust();
        int                        column = message->GetEndColumn(m_pIsolate->GetCurrentContext()).FromJust();
        std::unique_ptr<STryCatch> sTryCatch = std::make_unique<STryCatch>(originFileName, *exception, line, column);
        m_vecCompilationErrors.emplace_back(std::move(sTryCatch));
        return;
    }

    if (Module::Status::kUninstantiated == module->GetStatus())
    {
        const char* szName = originFileName.c_str();
        m_mapScriptModules[szName] = Global<Module>(m_pIsolate, module);

        // Result always true even module does not exists.
        Maybe<bool> result = module->InstantiateModule(
            m_context.Get(m_pIsolate), [](Local<Context> context, Local<String> specifier, Local<FixedArray> import_assertions, Local<Module> referrer) {
                String::Utf8Value  importName(context->GetIsolate(), specifier);
                CV8Isolate*        self = (CV8Isolate*)context->GetIsolate()->GetData(0);
                MaybeLocal<Module> module = self->InstantiateModule(context, specifier, import_assertions, referrer);
                if (module.IsEmpty())
                {
                    self->ReportMissingModule(*importName);
                    module = CV8::GetDummyModule(context->GetIsolate());            // make error message display all missing modules instead of first only.
                }
                return module;
            });
        std::unique_ptr<SModule> smodule = std::make_unique<SModule>();
        smodule->m_module.Reset(m_pIsolate, module);
        m_vecModules.push_back(std::move(smodule));
    }
}

void CV8Isolate::ReportMissingModule(std::string name)
{
    if (m_mapMissingModules.find(m_strCurrentOriginFileName) == m_mapMissingModules.end())
    {
        m_mapMissingModules[m_strCurrentOriginFileName] = std::vector<std::string>();
    }
    m_mapMissingModules[m_strCurrentOriginFileName].push_back(name);
}

bool CV8Isolate::GetErrorMessage(std::string& error)
{
    if (m_vecCompilationErrors.empty())
    {
        return false;
    }
    std::stringstream stream;

    stream << "[JS Error] Failed to compile " << m_vecCompilationErrors.size() << " of " << m_iRunCodeCount << " scripts in resource \"" << m_strOriginResource
           << "\"\n";
    for (auto const& tryCatch : m_vecCompilationErrors)
    {
        stream << "\t" << tryCatch->location << ": " << tryCatch->exception << " at " << tryCatch->line << ":" << tryCatch->column << "\n";
    }

    stream << '\n';
    error = stream.str();
    return true;
}

bool CV8Isolate::GetMissingModulesErrorMessage(std::string& error)
{
    if (m_mapMissingModules.empty())
    {
        return false;
    }

    int iMissingModules = 0;
    for (auto const& missingModules : m_mapMissingModules)
    {
        iMissingModules += missingModules.second.size();
    }

    std::stringstream stream;
    if (iMissingModules == 1)
    {
        stream << "[JS Error] Failed to resolve " << iMissingModules << " module in resource \"" << m_strOriginResource << "\.\n";
    }
    else
    {
        stream << "[JS Error] Failed to resolve " << iMissingModules << " modules in resource \"" << m_strOriginResource << "\".\n";
    }

    int limit = 4;            // Display only first 4 modules
    for (auto const& missingModules : m_mapMissingModules)
    {
        stream << "\t in " << missingModules.first << ": ";
        for (auto const& importName : missingModules.second)
        {
            stream << "\"" << importName << "\", ";
        }
        stream << "\n";
    }

    error = stream.str();
    return true;
}

void CV8Isolate::SetJsEvalSetting(eJsEval value)
{
    Locker         lock(m_pIsolate);
    Isolate::Scope isolateScope(m_pIsolate);
    HandleScope    handleScope(m_pIsolate);

    m_eJsEval = value;
    switch (m_eJsEval)
    {
        case eJsEval::DISABLED:
            m_context.Get(m_pIsolate)->AllowCodeGenerationFromStrings(false);
            break;
        case eJsEval::ACL_ALLOWED:
            assert(false && "unimplemented eval setting");
        default:
            assert(false && "unimplemented eval setting");
    }
}
void CV8Isolate::TerminateExecution()
{
    // m_pIsolate->RequestInterrupt([](Isolate* isolate, void* data) { isolate->TerminateExecution(); }, nullptr);
}

Local<Object> CV8Isolate::CreateGlobalObject(const char* mapName)
{
    Local<Object> object = Object::New(m_pIsolate);
    m_context.Get(m_pIsolate)->Global()->Set(m_context.Get(m_pIsolate), CV8Utils::ToV8String(mapName), object);
    return object;
}

void CV8Isolate::SetObjectKeyValue(Local<Object> object, const char* key, Local<Value> value)
{
    object->Set(m_context.Get(m_pIsolate), CV8Utils::ToV8String(key), value);
}

void CV8Isolate::SetKeyValue(const char* key, Local<Value> value)
{
    m_context.Get(m_pIsolate)->Global()->Set(m_context.Get(m_pIsolate), CV8Utils::ToV8String(key), value);
}

Local<Function> CV8Isolate::CreateFunction(void (*callback)(CV8FunctionCallbackBase*))
{
    Local<Value> value = External::New(m_pIsolate, callback);

    FunctionCallback functionCallback = [](const FunctionCallbackInfo<Value>& args) {
        Locker          lock(args.GetIsolate());
        HandleScope     handleScope(args.GetIsolate());
        Local<External> ext = args.Data().As<External>();
        void (*func)(CV8FunctionCallbackBase*) = static_cast<void (*)(CV8FunctionCallbackBase*)>(ext->Value());
        CV8FunctionCallback callback(args);
        func(&callback);
    };

    Local<Function> function = Function::New(m_pIsolate->GetCurrentContext(), functionCallback, value).ToLocalChecked();
    return function;
}

void CV8Isolate::Evaluate()
{
    Locker         lock(m_pIsolate);
    Isolate::Scope isolateScope(m_pIsolate);
    HandleScope    handleScope(m_pIsolate);
    Context::Scope contextScope(m_context.Get(m_pIsolate));

    m_context.Get(m_pIsolate)->Enter();

    for (auto const& module : m_vecModules)
    {
        Local<Module> v8Module = module->m_module.Get(m_pIsolate);
        if (Module::Status::kInstantiated != v8Module->GetStatus())
        {
            int a = 5;
            return;
        }
    }

    for (auto const& module : m_vecModules)
    {
        Execution     execution(this);
        TryCatch      evaluateTryCatch(m_pIsolate);
        Local<Module> v8Module = module->m_module.Get(m_pIsolate);
        if (Module::Status::kInstantiated == v8Module->GetStatus())
        {
            Local<Value> val;
            if (!v8Module->Evaluate(m_context.Get(m_pIsolate)).ToLocal(&val))
            {
                ReportException(&evaluateTryCatch);
                continue;
            }
        }
    }

    m_context.Get(m_pIsolate)->Exit();
}

CV8Isolate::~CV8Isolate()
{
    {
        Locker         lock(m_pIsolate);
        Isolate::Scope isolateScope(m_pIsolate);
        HandleScope    handleScope(m_pIsolate);
        Local<Context> thisContext = m_context.Get(m_pIsolate);
        Context::Scope contextScope(thisContext);
        thisContext->Enter();
        /*for (auto const& [index, persistent] : CV8BaseClass::m_mapPersistents)
        {
            auto objectContext = persistent->Get(m_pIsolate)->CreationContext();
            if (objectContext == thisContext)
            {
                persistent->Empty();
            }
        }*/
        // check failed i::FLAG_expose_gc
        m_pIsolate->VisitWeakHandles(new PHV(m_pIsolate));
        m_pIsolate->VisitHandlesWithClassIds(new PHV(m_pIsolate));
        m_pIsolate->LowMemoryNotification();
#if DEBUG
        int before = JavascriptWrapper::GetGlobalsCount();
        m_pIsolate->RequestGarbageCollectionForTesting(Isolate::GarbageCollectionType::kFullGarbageCollection);
        printf("[V8 DEBUG] Garbage colledted: %i objects.\n", before - JavascriptWrapper::GetGlobalsCount());
#endif

        m_global.Reset();
        m_context.Reset();

        for (auto& pair : m_mapScriptModules)
        {
            pair.second.Reset();
        }
        for (auto& module : m_vecModules)
        {
            module->m_module.Reset();
        }
        thisContext->Exit();
    }

    delete m_createParams.array_buffer_allocator;
    m_pIsolate->Dispose();
}
