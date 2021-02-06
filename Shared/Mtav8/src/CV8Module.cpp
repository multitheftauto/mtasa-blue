#include "StdInc.h"

using namespace v8;

CV8Module::CV8Module(const char* name) : m_strName(name)
{
}

CV8Module::~CV8Module()
{

}

std::vector<Local<String>> CV8Module::GetExports(Isolate* pIsolate)
{
    std::vector<Local<String>> exports;
    for (auto const& pair : m_mapFunctions)
    {
        exports.push_back(String::NewFromUtf8(pIsolate, pair.first).ToLocalChecked());
    }
    return exports;
}
