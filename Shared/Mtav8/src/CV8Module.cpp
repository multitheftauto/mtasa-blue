#include "StdInc.h"

using namespace v8;

CV8Module::CV8Module(std::string name) : m_strName(name)
{
}

CV8Module::~CV8Module()
{
}

void CV8Module::AddFunction(std::string name, void (*callback)(CV8FunctionCallbackBase*))
{
    assert(m_mapFunctions.find(name) == m_mapFunctions.end());
    m_mapFunctions[name] = callback;
}

void CV8Module::AddEnum(std::string name, CV8EnumBase* pEnum)
{
    assert(m_mapFunctions.find(name) == m_mapFunctions.end());
    m_enums[name] = (CV8Enum*)pEnum;
}

std::unordered_map<std::string, void (*)(CV8FunctionCallbackBase*)> CV8Module::GetFunctions() const
{
    return m_mapFunctions;
}

std::unordered_map<std::string, CV8Enum*> CV8Module::GetEnums() const
{
    return m_enums;
}

std::vector<Local<String>> CV8Module::GetExports(Isolate* pIsolate)
{
    std::vector<Local<String>> exports;
    for (auto const& pair : m_mapFunctions)
        exports.push_back(CV8Utils::ToV8String(pair.first));

    for (auto const& pair : m_enums)
        exports.push_back(CV8Utils::ToV8String(pair.first));

    return exports;
}
