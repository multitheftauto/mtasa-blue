#include "StdInc.h"

#include "v8-inspector.h"
using namespace v8;

std::unordered_map<std::string, std::unique_ptr<CV8Module>> CV8::m_mapModules;

const char* ToCString(const String::Utf8Value& value)
{
    return *value ? *value : "<string conversion failed>";
}

CV8::CV8()
{
    V8::InitializeICUDefaultLocation(V8Config::szExecPath);
    V8::InitializeExternalStartupData(V8Config::szExternalStartupData);

    m_pPlatform = platform::NewDefaultPlatform(4);
    V8::InitializePlatform(m_pPlatform.get());
    V8::Initialize();
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
                    double time = static_cast<double>(GetTickCount64_()) - m_pIsolateExecutionStart;
                    //printf("Time %lf\n", time);
                    if (time > 2000)
                    {
                        m_pCurrentExecutionIsolate->TerminateExecution();
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
    {
        std::lock_guard guard(m_lock);
        m_bDisposing = true;
    }
    m_longExecutionGuardThread.join();
    V8::Dispose();
    V8::ShutdownPlatform();
}

void CV8::EnterExecution(CV8Isolate* pIsolate)
{
    std::lock_guard lock(m_executionGuard);
    m_pIsolateExecutionStart = static_cast<double>(GetTickCount64_());
    m_pCurrentExecutionIsolate = pIsolate;
}

void CV8::ExitExecution(CV8Isolate* pIsolate)
{
    std::lock_guard lock(m_executionGuard);
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

CV8IsolateBase* CV8::CreateIsolate(std::string& originResource)
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
    Local<String> source = String::NewFromUtf8(pIsolate, "", NewStringType::kNormal).ToLocalChecked();
    Local<String> fileName = String::NewFromUtf8(pIsolate, "", NewStringType::kNormal).ToLocalChecked();

    ScriptOrigin           origin(fileName, 0, 0, false, -1, Local<Value>(), false, false, true);
    ScriptCompiler::Source compilerSource(source, origin);
    Local<Module>          module;
    ScriptCompiler::CompileModule(pIsolate, &compilerSource).ToLocal(&module);
    return module;
}

void CV8::RegisterAllModules(Isolate* pIsolate)
{
    for (auto const& [name, module] : m_mapModules)
    {

    }
}

CV8Module* CV8::GetModuleByName(const char* name)
{
    if (CV8::m_mapModules.find(name) != CV8::m_mapModules.end())
        return CV8::m_mapModules[name].get();
    return nullptr;
}

CV8ModuleBase* CV8::CreateModule(const char* name)
{
    std::string buf(V8Config::szMtaModulePrefix);
    buf.append("/");
    buf.append(name);

    std::unique_ptr<CV8Module> module = std::make_unique<CV8Module>(buf.c_str());
    CV8Module*                 pModule = module.get();
    CV8::m_mapModules.insert({buf, std::move(module)});
    return pModule;
}
