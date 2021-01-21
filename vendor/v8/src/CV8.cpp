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

    m_pPlatform = platform::NewSingleThreadedDefaultPlatform();
    V8::InitializePlatform(m_pPlatform.get());
    V8::Initialize();
}

CV8::~CV8()
{
    V8::Dispose();
    V8::ShutdownPlatform();
}

CV8IsolateBase* CV8::CreateIsolate(std::string& strCode, std::string& originResource)
{
    CV8Isolate* pIsolate = new CV8Isolate(this, originResource);
    pIsolate->RunCode(strCode, true);
    return pIsolate;
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