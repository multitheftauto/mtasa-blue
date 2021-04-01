#include "StdInc.h"

using namespace v8;

std::unordered_map<std::string, std::unique_ptr<CV8Module>> CV8::m_mapModules;

CV8::CV8()
{
}

void CV8::Initialize(int iThreadPool)
{
    V8::InitializeICUDefaultLocation(V8Config::szExecPath);
    V8::InitializeExternalStartupData(V8Config::szExternalStartupData);

    m_pPlatform = platform::NewDefaultPlatform(iThreadPool);
    V8::InitializePlatform(m_pPlatform.get());
    V8::Initialize();

    // Todo, expose it in mtaserver.conf
    // V8::SetFlagsFromString("--expose-gc --trace-opt --trace-deopt", 38);
#if DEBUG
    V8::SetFlagsFromString("--expose-gc --trace-opt --trace-deopt", 38);
#endif

    m_longExecutionGuardThread = std::thread([this]() {
        while (true)
        {
            {
                std::lock_guard guard(m_lock);
                if (m_bDisposing)
                    return;
            }

            {
                std::lock_guard lock(m_executionGuard);
                if (m_pCurrentExecutionIsolate)
                {
                    if (m_pIsolateExecutionTicks <= 0)
                    {
                        m_pCurrentExecutionIsolate->TerminateExecution();
                        m_pCurrentExecutionIsolate = nullptr;
                    }
                    else
                    {
                        m_pIsolateExecutionTicks--;            // use decrement counter over getTickCount comparison to prevent terminating scripts while
                                                               // debugger is in use.
                    }
                }
            }

            std::chrono::milliseconds timespan(V8Config::iGuardThreadSleep);

            std::this_thread::sleep_for(timespan);
        }
    });
}

CV8::~CV8()
{
}

void CV8::Shutdown()
{
    for (auto const& isolate : m_vecIsolates)
    {
        isolate->Shutdown();
    }
    {
        std::lock_guard lk(m_lock);
        m_bDisposing = true;
    }
    m_longExecutionGuardThread.join();
    V8::Dispose();
    V8::ShutdownPlatform();
}

void CV8::EnterExecution(CV8Isolate* pIsolate)
{
    std::lock_guard lk(m_executionGuard);
    m_pIsolateExecutionTicks = m_iTime / V8Config::iGuardThreadSleep;
    m_pCurrentExecutionIsolate = pIsolate;
}

void CV8::ExitExecution(CV8Isolate* pIsolate)
{
    std::lock_guard lk(m_executionGuard);
    m_pCurrentExecutionIsolate = nullptr;
}

void CV8::DoPulse()
{
    for (auto const& isolate : m_vecIsolates)
    {
        isolate->DoPulse();
    }
}

std::vector<CV8IsolateBase*> CV8::GetIsolates()
{
    std::vector<CV8IsolateBase*> isolates = std::vector<CV8IsolateBase*>();
    isolates.reserve(m_vecIsolates.size());
    for (auto const& isolate : m_vecIsolates)
    {
        isolates.push_back(isolate.get());
    }
    return isolates;
}

std::vector<CV8Class*> CV8::GetClasses()
{
    std::vector<CV8Class*> isolates = std::vector<CV8Class*>();
    isolates.reserve(m_vecClasses.size());
    for (auto const& isolate : m_vecClasses)
    {
        isolates.push_back(isolate.get());
    }
    return isolates;
}

CV8IsolateBase* CV8::CreateIsolate(std::string originResource)
{
    std::unique_ptr<CV8Isolate> pIsolate = std::make_unique<CV8Isolate>(this, originResource);
    CV8Isolate*                 isolate = pIsolate.get();
    m_vecIsolates.push_back(std::move(pIsolate));
    return isolate;
}

void CV8::RemoveIsolate(CV8IsolateBase* pIsolate)
{
    for (auto it = m_vecIsolates.begin(); it != m_vecIsolates.end(); ++it)
    {
        if ((*it).get() == pIsolate)
        {
            platform::NotifyIsolateShutdown(m_pPlatform.get(), it->get()->GetIsolate());
            m_vecIsolates.erase(it);
            return;
        }
    }
}

Local<Module> CV8::GetDummyModule(Isolate* pIsolate)
{
    Local<String> source = CV8Utils::ToV8String("");
    Local<String> fileName = CV8Utils::ToV8String("");

    ScriptOrigin           origin(fileName, 0, 0, false, -1, Local<Value>(), false, false, true);
    ScriptCompiler::Source compilerSource(source, origin);
    Local<Module>          module;
    ScriptCompiler::CompileModule(pIsolate, &compilerSource).ToLocal(&module);
    return module;
}

void CV8::RegisterAllModules(CV8Isolate* pIsolate)
{
    Isolate* isolate = pIsolate->GetIsolate();

    for (auto const& [name, module] : m_mapModules)
    {
        std::string   moduleName = name.substr(strlen(V8Config::szMtaModulePrefix) + 1);
        Local<Object> object = pIsolate->CreateGlobalObject(moduleName.c_str());
        for (auto const& [import, callback] : module->GetFunctions())
        {
            Local<Function> function = pIsolate->CreateFunction(callback);
            pIsolate->SetObjectKeyValue(object, import, function);
        }
    }
}

void CV8::RegisterAllModulesInGlobalNamespace(CV8Isolate* pIsolate)
{
    Isolate* isolate = pIsolate->GetIsolate();

    for (auto const& [name, module] : m_mapModules)
    {
        for (auto const& [import, callback] : module->GetFunctions())
        {
            Local<Function> function = pIsolate->CreateFunction(callback);
            pIsolate->SetKeyValue(import, function);
        }
    }
}

CV8Module* CV8::GetModuleByName(std::string name)
{
    if (CV8::m_mapModules.find(name) != CV8::m_mapModules.end())
        return CV8::m_mapModules[name].get();
    return nullptr;
}

CV8EnumBase* CV8::CreateEnum()
{
    std::unique_ptr<CV8Enum> pPtrEnum = std::make_unique<CV8Enum>();
    CV8Enum*                 pEnum = pPtrEnum.get();
    m_vecEnums.push_back(std::move(pPtrEnum));
    return pEnum;
}

CV8ModuleBase* CV8::CreateModule(std::string name)
{
    std::string moduleName(V8Config::szMtaModulePrefix);
    moduleName.append("/");
    moduleName.append(name);

    assert(CV8::m_mapModules.find(moduleName) == CV8::m_mapModules.end());

    std::unique_ptr<CV8Module> module = std::make_unique<CV8Module>(moduleName);
    CV8Module*                 pModule = module.get();
    CV8::m_mapModules.insert({moduleName, std::move(module)});
    return pModule;
}

CV8ClassBase* CV8::CreateClass(std::string name, size_t classId)
{
    std::unique_ptr<CV8Class> v8Class = std::make_unique<CV8Class>(name, classId);
    CV8Class*                 pClass = v8Class.get();
    m_vecClasses.push_back(std::move(v8Class));
    return pClass;
}

CV8ExportObjectBase* CV8::CreateExportObject()
{
    std::unique_ptr<CV8ExportObject> v8Object = std::make_unique<CV8ExportObject>();
    CV8ExportObject*                 pObject = v8Object.get();
    m_vecObjects.push_back(std::move(v8Object));
    return pObject;
}
