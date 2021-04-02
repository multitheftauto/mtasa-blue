#include "StdInc.h"
#include <queue>
#include <sstream>
#include "SharedUtil.hpp"

using namespace v8;

CV8Isolate::CV8Isolate(CV8* pCV8, std::string originResource) : m_pCV8(pCV8)
{
    // m_constraints.ConfigureDefaults(1024 * 1024 * 200, 0);
    m_constraints.ConfigureDefaultsFromHeapSize(1024 * 1024 * 64, 1024 * 1024 * 66);
    m_strOriginResource = originResource;
    m_createParams.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
    m_createParams.constraints = m_constraints;
    m_pIsolate = Isolate::New(m_createParams);

    Locker         lock(m_pIsolate);
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

    m_global.Reset(m_pIsolate, ObjectTemplate::New(m_pIsolate));

    m_rootContext.Reset(m_pIsolate, Context::New(m_pIsolate, nullptr, m_global.Get(m_pIsolate)));
    Context::Scope contextScope(m_rootContext.Get(m_pIsolate));

    InitSecurity();
    InitClasses();
}

void CV8Isolate::InitSecurity()
{
    Local<Context> context = m_rootContext.Get(m_pIsolate);
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
    Locker      lock(m_pIsolate);
    HandleScope handleScope(m_pIsolate);

    // Clear all rejected, fulfilled promises
    m_vecPromises.erase(std::remove_if(m_vecPromises.begin(), m_vecPromises.end(), [](std::unique_ptr<CV8Promise>& promise) { return !promise->IsPending(); }),
                        m_vecPromises.end());

    // Continue async await execution
    m_pIsolate->PerformMicrotaskCheckpoint();
}

void CV8Isolate::EnqueueMicrotask(std::function<void(CV8Isolate*)> microtask)
{
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

        v8::String::Utf8Value  fileName(m_pIsolate, message->GetScriptOrigin().ResourceName());
        const char*            fileNameString = CV8Utils::ToString(fileName);
        int                    lineNum = message->GetLineNumber(context).FromJust();
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
    CV8Module*        pModule = self->m_pCV8->GetModuleByName(*importName);
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

MaybeLocal<Module> CV8Isolate::GetScriptModule(std::string name)
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
    CV8Promise*                       promise = pPromise.get();
    std::shared_ptr<CancelationToken> token = std::make_shared<CancelationToken>();
    promise->SetCancelationToken(token);
    m_pCV8->GetPlatform()->CallOnWorkerThread(std::make_unique<CV8Task>(token, [promise]() { promise->Run(); }));
    m_vecPromises.push_back(std::move(pPromise));
}

MaybeLocal<Value> CV8Isolate::InitializeModuleExports(Local<Context> context, Local<Module> module)
{
    CV8Isolate* self = (CV8Isolate*)m_pIsolate->GetData(0);
    std::string moduleName = modulesListName.front();
    CV8Module*  pModule = self->m_pCV8->GetModuleByName(moduleName);
    modulesListName.pop();
    if (pModule)
    {
        for (auto const& [importName, callback] : pModule->GetFunctions())
        {
            Local<Function> function = CreateFunction(callback);
            module->SetSyntheticModuleExport(context->GetIsolate(), CV8Utils::ToV8String(importName), function);
        }
        for (auto const& [importName, pEnum] : pModule->GetEnums())
        {
            Local<Object> objectEnum = pEnum->Initialize(self, importName);
            module->SetSyntheticModuleExport(context->GetIsolate(), CV8Utils::ToV8String(importName), objectEnum);
        }
        for (auto const& [importName, pObject] : pModule->GetObjects())
        {
            Local<Object> objectEnum = pObject->Initialize(self, importName);
            module->SetSyntheticModuleExport(context->GetIsolate(), CV8Utils::ToV8String(importName), objectEnum);
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
            m_rootContext.Get(m_pIsolate), [](Local<Context> context, Local<String> specifier, Local<FixedArray> import_assertions, Local<Module> referrer) {
                String::Utf8Value  importName(context->GetIsolate(), specifier);
                CV8Isolate*        self = (CV8Isolate*)context->GetIsolate()->GetData(0);
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

void CV8Isolate::RunCode(std::string& code, std::string originFileName)
{
    m_iRunCodeCount++;
    m_strCurrentOriginFileName = originFileName;
    Locker         lock(m_pIsolate);
    Isolate::Scope isolateScope(m_pIsolate);
    HandleScope    handleScope(m_pIsolate);
    Context::Scope rootContextScope(m_rootContext.Get(m_pIsolate));

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
            m_mapScriptModules[originFileName] = Global<Module>(m_pIsolate, module);
            m_loadingOrder.push_back(originFileName);
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
        limit--;
        if (limit)
            stream << "\n";
        else
            break;
    }

    error = stream.str();
    return true;
}

void CV8Isolate::SetEvalEnabled(bool value)
{
    Locker         lock(m_pIsolate);
    Isolate::Scope isolateScope(m_pIsolate);
    HandleScope    handleScope(m_pIsolate);

    m_rootContext.Get(m_pIsolate)->AllowCodeGenerationFromStrings(value);
}

void CV8Isolate::TerminateExecution()
{
    m_pIsolate->RequestInterrupt([](Isolate* isolate, void* data) { isolate->TerminateExecution(); }, nullptr);
}

Local<Object> CV8Isolate::CreateGlobalObject(std::string mapName)
{
    Local<Object> object = Object::New(m_pIsolate);
    m_rootContext.Get(m_pIsolate)->Global()->Set(m_rootContext.Get(m_pIsolate), CV8Utils::ToV8String(mapName), object);
    return object;
}

void CV8Isolate::SetObjectKeyValue(Local<Object> object, std::string key, Local<Value> value)
{
    object->Set(m_rootContext.Get(m_pIsolate), CV8Utils::ToV8String(key), value);
}

void CV8Isolate::SetKeyValue(std::string key, Local<Value> value)
{
    m_rootContext.Get(m_pIsolate)->Global()->Set(m_rootContext.Get(m_pIsolate), CV8Utils::ToV8String(key), value);
}

Local<Function> CV8Isolate::CreateFunction(void (*callback)(CV8FunctionCallbackBase*))
{
    Local<Value> value = External::New(m_pIsolate, callback);

    FunctionCallback functionCallback = [](const FunctionCallbackInfo<Value>& args) {
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

    Local<Context> context = m_rootContext.Get(m_pIsolate);
    Context::Scope contextScope(m_rootContext.Get(m_pIsolate));
    m_rootContext.Get(m_pIsolate)->Enter();
    InitializeModules();

    if (HasInitializationError())
    {
        m_rootContext.Get(m_pIsolate)->Exit();
        return;
    }

    // std::reverse(m_loadingOrder.begin(), m_loadingOrder.end());
    for (std::string moduleName : m_loadingOrder)
    {
        auto const&   pair = m_mapScriptModules[moduleName];
        Local<Module> module = pair.Get(m_pIsolate);
        auto          st = module->GetStatus();
        if (Module::Status::kInstantiated == module->GetStatus())
        {
            TryCatch     evaluateTryCatch(m_pIsolate);
            Execution    execution(this);
            Local<Value> val;            // value returned by loaded script.
            if (!module->Evaluate(m_rootContext.Get(m_pIsolate)).ToLocal(&val))
            {
                ReportException(&evaluateTryCatch);
            }
        }
    }

    m_rootContext.Get(m_pIsolate)->Exit();
}

void CV8Isolate::RequestGC()
{
    Locker         lock(m_pIsolate);
    Isolate::Scope isolateScope(m_pIsolate);
    HandleScope    handleScope(m_pIsolate);
    Local<Context> thisContext = m_rootContext.Get(m_pIsolate);
    m_pIsolate->RequestGarbageCollectionForTesting(Isolate::GarbageCollectionType::kFullGarbageCollection);
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

        m_vecPromises.clear();
        // Continue async await execution
        m_pIsolate->PerformMicrotaskCheckpoint();

        m_pIsolate->LowMemoryNotification();

        m_global.Reset();
        m_rootContext.Reset();

        for (auto& pair : m_mapScriptModules)
        {
            pair.second.Reset();
        }
        for (auto& module : m_vecModules)
        {
            module->m_module.Reset();
        }
    }

    delete m_createParams.array_buffer_allocator;
    m_pIsolate->Dispose();
}
