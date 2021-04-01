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

void CV8Module::AddEnum(std::string name, CV8ExportEnumBase* pEnum)
{
    assert(m_mapFunctions.find(name) == m_mapFunctions.end());
    m_enums[name] = (CV8ExportEnum*)pEnum;
}

void CV8Module::AddObject(std::string name, CV8ExportObjectBase* pObject)
{
    assert(m_objects.find(name) == m_objects.end());
    m_objects[name] = (CV8ExportObject*)pObject;
}

std::unordered_map<std::string, void (*)(CV8FunctionCallbackBase*)> CV8Module::GetFunctions() const
{
    return m_mapFunctions;
}

std::unordered_map<std::string, CV8ExportEnum*> CV8Module::GetEnums() const
{
    return m_enums;
}

std::unordered_map<std::string, CV8ExportObject*> CV8Module::GetObjects() const
{
    return m_objects;
}

std::vector<Local<String>> CV8Module::GetExports(Isolate* pIsolate)
{
    std::vector<Local<String>> exports;
    for (auto const& pair : m_mapFunctions)
        exports.push_back(CV8Utils::ToV8String(pair.first));

    for (auto const& pair : m_enums)
        exports.push_back(CV8Utils::ToV8String(pair.first));

    for (auto const& pair : m_objects)
        exports.push_back(CV8Utils::ToV8String(pair.first));

    return exports;
}
