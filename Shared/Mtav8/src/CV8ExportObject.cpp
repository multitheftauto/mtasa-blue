#include "StdInc.h"

CV8ExportObject::CV8ExportObject()
{
}

void CV8ExportObject::AddFunction(std::string name, void (*callback)(CV8FunctionCallbackBase*))
{
    assert(m_mapFunctions.find(name) == m_mapFunctions.end());
    m_mapFunctions[name] = callback;
}

std::unordered_map<std::string, void (*)(CV8FunctionCallbackBase*)> CV8ExportObject::GetFunctions() const
{
    return m_mapFunctions;
}

Local<Function> CV8ExportObject::CreateFunction(Isolate* pIsolate, void (*callback)(CV8FunctionCallbackBase*))
{
    Local<Value> value = External::New(pIsolate, callback);

    FunctionCallback functionCallback = [](const FunctionCallbackInfo<Value>& args) {
        HandleScope     handleScope(args.GetIsolate());
        Local<External> ext = args.Data().As<External>();
        void (*func)(CV8FunctionCallbackBase*) = static_cast<void (*)(CV8FunctionCallbackBase*)>(ext->Value());
        CV8FunctionCallback callback(args);
        func(&callback);
    };

    Local<Function> function = Function::New(pIsolate->GetCurrentContext(), functionCallback, value).ToLocalChecked();
    return function;
}

Local<Object> CV8ExportObject::Initialize(CV8Isolate* pV8Isolate, std::string name)
{
    Isolate*       pIsolate = pV8Isolate->GetIsolate();
    Local<Context> context = pIsolate->GetCurrentContext();

    EscapableHandleScope  handleScope{pIsolate};
    Context::Scope        context_scope(context);
    Local<ObjectTemplate> exportObject = ObjectTemplate::New(pIsolate);
    exportObject->Set(Symbol::GetToStringTag(pV8Isolate->GetIsolate()), CV8Utils::ToV8String(name));

    Local<Object> object = exportObject->NewInstance(context).ToLocalChecked();
    for (auto const& pair : m_mapFunctions)
    {
        object->Set(context, CV8Utils::ToV8String(pair.first), CreateFunction(pIsolate, pair.second));
    }

    return handleScope.Escape(object);
}
