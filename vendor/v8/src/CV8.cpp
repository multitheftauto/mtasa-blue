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
    V8::InitializeICUDefaultLocation("");
    V8::InitializeExternalStartupData("");
    // V8::InitializeICUDefaultLocation(argv[0]);
    // V8::InitializeExternalStartupData(argv[0]);

    m_pPlatform = platform::NewDefaultPlatform(4);
    V8::InitializePlatform(m_pPlatform.get());
    V8::Initialize();
}

CV8::~CV8()
{
    V8::Dispose();
    V8::ShutdownPlatform();
}

void CV8::DoTaskPulse()
{
    for (auto const& isolate : m_vecIsolates)
    {
        isolate->DoTaskPulse();
    }
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

CV8IsolateBase* CV8::CreateIsolate(std::string& strCode, std::string& originResource)
{
    std::unique_ptr<CV8Isolate> pIsolate = std::make_unique<CV8Isolate>(this, originResource);
    pIsolate->RunCode(strCode, true);
    CV8Isolate* isolate = pIsolate.get();
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

CV8Module* CV8::GetModuleByName(const char* name)
{
    if (CV8::m_mapModules.find(name) != CV8::m_mapModules.end())
        return CV8::m_mapModules[name].get();
    return nullptr;
}

CV8ModuleBase* CV8::CreateModule(const char* name)
{
    std::string buf("@mta/");
    buf.append(name);

    std::unique_ptr<CV8Module> module = std::make_unique<CV8Module>(buf.c_str());
    CV8Module*                 pModule = module.get();
    CV8::m_mapModules.insert({buf, std::move(module)});
    return pModule;
}