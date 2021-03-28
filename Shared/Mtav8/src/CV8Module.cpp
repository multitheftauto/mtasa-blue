#include "StdInc.h"

using namespace v8;

CV8Module::CV8Module(const char* name) : m_strName(name)
{
}

CV8Module::~CV8Module()
{
}

void CV8Module::AddFunction(const char* name, void (*callback)(CV8FunctionCallbackBase*))
{
    m_mapFunctions[name] = callback;
}

std::unordered_map<const char*, void (*)(CV8FunctionCallbackBase*)> CV8Module::GetFunctions() const
{
    return m_mapFunctions;
}

std::vector<Local<String>> CV8Module::GetExports(Isolate* pIsolate)
{
    std::vector<Local<String>> exports;
    for (auto const& pair : m_mapFunctions)
    {
        exports.push_back(CV8Utils::ToV8String(pair.first));
    }
    return exports;
}
