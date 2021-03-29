#include "StdInc.h"
#include <queue>
#include <sstream>
#include "SharedUtil.hpp"

using namespace v8;

void CV8GC::VisitPersistentHandle(Persistent<Value>* persistent, uint16_t usClassId)
{
    HandleScope          handleScope(m_pIsolate);
    CV8BaseClass::EClass classId = (CV8BaseClass::EClass)usClassId;
    Local<Value>         value = persistent->Get(m_pIsolate);

    assert(value->IsObject());            // may be removed in future

    Local<Object>   object = value->ToObject(m_pIsolate->GetCurrentContext()).ToLocalChecked();
    Local<External> external = Local<External>::Cast(object->GetInternalField(CV8BaseClass::EInternalFieldPurpose::PointerToValue));
    void*           pointerToValue = external->Value();

    switch (classId) // Cast to 
    {
        case CV8BaseClass::EClass::Invalid:
            assert(false && "Invalid class id");
            break;
        case CV8BaseClass::EClass::Vector2:
        case CV8BaseClass::EClass::Vector3:
        case CV8BaseClass::EClass::Vector4:
        case CV8BaseClass::EClass::Matrix:
            delete pointerToValue;
            break;
        default:
            assert(false && "Not implemented class");
    }
}

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
            printf("[V8 Debug] Heap limit reached. New limit: %iMB, Initial limit: %iMB\n", (current_heap_limit + increaseMB) / 1024 / 1024,
                   initial_heap_limit / 1024 / 1024);
            //((Isolate*)data)->RequestInterrupt([](Isolate* isolate, void* data) { isolate->TerminateExecution(); }, nullptr);
            return (size_t)current_heap_limit + increaseMB;
        },
        this);
    m_pIsolate->EnableMemorySavingsMode();
    m_global.Reset(m_pIsolate, ObjectTemplate::New(m_pIsolate));

    m_context.Reset(m_pIsolate, Context::New(m_pIsolate, nullptr, m_global.Get(m_pIsolate)));
    Context::Scope contextScope(m_context.Get(m_pIsolate));

    Local<Context> context = m_context.Get(m_pIsolate);
    Local<Object>  global = context->Global();
    // m_pIsolate->SetCounterFunction([](const char* name) {
    //    printf("%s\n", name);
    //    return new int(0);
    //});

    m_pGC = std::make_unique<CV8GC>(m_pIsolate);

    InitSecurity();
    InitClasses();

    //Handle<FunctionTemplate> vector2dTemplate = CV8Vector2D::CreateTemplate(context);
    //Handle<FunctionTemplate> vector3dTemplate = CV8Vector3D::CreateTemplate(context, vector2dTemplate);
    //Handle<FunctionTemplate> vector4dTemplate = CV8Vector4D::CreateTemplate(context, vector3dTemplate);
    //Handle<FunctionTemplate> matrixTemplate = CV8Matrix::CreateTemplate(context);
}

void CV8Isolate::InitSecurity()
{
    Local<Context> context = m_context.Get(m_pIsolate);
    Local<Object>  global = context->Global();
    global->Set(context, CV8Utils::ToV8String("WebAssembly"), Undefined(m_pIsolate));
}

void CV8Isolate::InitClasses()
{
    for (auto const& pClass : m_pCV8->GetClasses())
    {
        pClass->Initialize(this);
    }
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

std::string CV8Isolate::GetModuleName(Local<Module> module)
{
    std::string strReferrer;
    for (std::string moduleName : m_loadingOrder)
    {
        auto const& pair = m_mapScriptModules[moduleName];
        if (pair == module)
        {
            return moduleName;
        }
    }
    // Maybe module belong to other resource
    assert(false);
}

MaybeLocal<Module> CV8Isolate::InstantiateModule(Local<Context> context, Local<String> specifier, Local<FixedArray> import_assertions, Local<Module> referrer)
{
    std::string strReferrer = GetModuleName(referrer);

    Isolate*          pIsolate = context->GetIsolate();
    CV8Isolate*       self = (CV8Isolate*)pIsolate->GetData(0);
    String::Utf8Value importName(pIsolate, specifier);
    CV8Module*        pModule = CV8::GetModuleByName(*importName);
    std::string       strImportName = *importName;

    if (!strcmp(strReferrer.c_str(), *importName))
    {
        RaiseInitializationError("%s can not import itself\n", strReferrer.c_str());
        return {};
    }

    auto B = std::find(m_loadingOrder.begin(), m_loadingOrder.end(), strImportName);
    if (B != m_loadingOrder.end())
    {
        auto A = std::find(m_loadingOrder.begin(), m_loadingOrder.end(), strReferrer);
        if (std::distance(m_loadingOrder.begin(), B) > std::distance(m_loadingOrder.begin(), A))
        {
            RaiseInitializationError("%s can not import %s\n", strReferrer.c_str(), strImportName.c_str());
            return {};
        }
    }

    if (!strcmp(*importName, V8Config::szMtaModulePrefix))
    {
        // import * as X from "@mta/X" for each module
        CV8::RegisterAllModules(this);
        return CV8::GetDummyModule(pIsolate);
    }
    else if (!strcmp(*importName, V8Config::szMtaImportAllFunctions))
    {
        // import { A, B, C } from "@mta/..." for each module
        CV8::RegisterAllModulesInGlobalNamespace(this);
        return CV8::GetDummyModule(pIsolate);
    }
    else if (!pModule)
    {
        // Modules written in .js
        Local<Module> scriptModule;
        if (!self->GetScriptModule(*importName).ToLocal(&scriptModule))
        {
            modulesListName.push(*importName);
            return {};
        }
        return scriptModule;
    }
    else
    {
        // @mta/moduleName/...
        Local<String> moduleName = CV8Utils::ToV8String(*importName);
        auto          exports = pModule->GetExports(pIsolate);

        Local<Module> module = Module::CreateSyntheticModule(pIsolate, moduleName, exports, [](Local<Context> context, Local<Module> module) {
            CV8Isolate* self = (CV8Isolate*)context->GetIsolate()->GetData(0);
            return self->InitializeModuleExports(context, module);
        });

        modulesListName.push(*importName);
        return module;
    }
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
    modulesListName.pop();
    if (pModule)
    {
        for (auto const& [import, callback] : pModule->GetFunctions())
        {
            Local<Function> function = CreateFunction(callback);
            module->SetSyntheticModuleExport(context->GetIsolate(), CV8Utils::ToV8String(import), function);
        }
        return True(context->GetIsolate());
    }
    return False(context->GetIsolate());
}

void CV8Isolate::InitializeModules()
{
    for (std::string moduleName : m_loadingOrder)
    {
        if (HasInitializationError())
            break;

        auto const& pair = m_mapScriptModules[moduleName];
        m_loadedModules.insert(moduleName);
        Local<Module> module = pair.Get(m_pIsolate);
        // Result always true even module does not exists.
        Maybe<bool> result = module->InstantiateModule(
            m_context.Get(m_pIsolate), [](Local<Context> context, Local<String> specifier, Local<FixedArray> import_assertions, Local<Module> referrer) {
                String::Utf8Value importName(context->GetIsolate(), specifier);
                CV8Isolate*       self = (CV8Isolate*)context->GetIsolate()->GetData(0);
                MaybeLocal<Module> module;
                if (self->HasInitializationError())
                {
                    module = CV8::GetDummyModule(context->GetIsolate());            // make error message display all missing modules instead of first only.
                    return module;
                }

                module = self->InstantiateModule(context, specifier, import_assertions, referrer);
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

    switch (module->GetStatus())
    {
        case Module::Status::kUninstantiated:
        {
            const char* szName = originFileName.c_str();
            m_mapScriptModules[szName] = Global<Module>(m_pIsolate, module);
            m_loadingOrder.push_back(szName);
        }
        break;
        case Module::Status::kInstantiating:
            break;
        case Module::Status::kInstantiated:
            break;
        case Module::Status::kEvaluating:
            break;
        case Module::Status::kEvaluated:
            break;
        case Module::Status::kErrored:
            break;
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
        stream << "[JS Error] Failed to resolve " << iMissingModules << " module in resource \"" << m_strOriginResource << "\".\n";
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

    Local<Context> context = m_context.Get(m_pIsolate);
    Context::Scope contextScope(m_context.Get(m_pIsolate));
    m_context.Get(m_pIsolate)->Enter();
    InitializeModules();

    if (HasInitializationError())
    {
        m_context.Get(m_pIsolate)->Exit();
        return;
    }

    std::reverse(m_loadingOrder.begin(), m_loadingOrder.end());
    for (std::string moduleName : m_loadingOrder)
    {
        auto const&   pair = m_mapScriptModules[moduleName];
        Execution     execution(this);
        TryCatch      evaluateTryCatch(m_pIsolate);
        Local<Module> module = pair.Get(m_pIsolate);
        if (Module::Status::kInstantiated == module->GetStatus())
        {
            Local<Value> val;
            if (!module->Evaluate(m_context.Get(m_pIsolate)).ToLocal(&val))
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
        thisContext->Enter();

        m_pIsolate->VisitWeakHandles(m_pGC.get());

        m_pIsolate->LowMemoryNotification();
#if DEBUG
        m_pIsolate->RequestGarbageCollectionForTesting(Isolate::GarbageCollectionType::kFullGarbageCollection);
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
