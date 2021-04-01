#include "StdInc.h"

CV8ExportEnum::CV8ExportEnum()
{
}

void CV8ExportEnum::SetValue(std::string key, std::string value)
{
    assert(m_stringKeys.find(key) == m_stringKeys.end() && "Duplicated enum key");
    m_stringKeys.insert({key, value});
}

Local<Object> CV8ExportEnum::Initialize(CV8Isolate* pV8Isolate, std::string name)
{
    Isolate*       pIsolate = pV8Isolate->GetIsolate();
    Local<Context> context = pIsolate->GetCurrentContext();

    EscapableHandleScope  handleScope{pIsolate};
    Context::Scope        context_scope(context);
    Local<ObjectTemplate> object = ObjectTemplate::New(pIsolate);
    object->Set(Symbol::GetToStringTag(pV8Isolate->GetIsolate()), CV8Utils::ToV8String(name));

    for (auto const& pair : m_stringKeys)
    {
        object->Set(CV8Utils::ToV8String(pair.first), CV8Utils::ToV8String(pair.second), PropertyAttribute::ReadOnly);
    }
    return handleScope.Escape(object->NewInstance(context).ToLocalChecked());
}
