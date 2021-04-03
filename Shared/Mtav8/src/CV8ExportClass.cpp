#include "StdInc.h"

CV8ExportClass::CV8ExportClass(std::string name, uint16_t classId) : m_name(name), m_classId(classId)
{

}

Local<FunctionTemplate> CV8ExportClass::Initialize(CV8Isolate* pV8Isolate)
{
    Isolate*             pIsolate = pV8Isolate->GetIsolate();
    Local<Context>       pV8Context = pIsolate->GetCurrentContext();
    EscapableHandleScope handleScope{pIsolate};

    Local<FunctionTemplate> classTemplate = FunctionTemplate::New(pIsolate);
    classTemplate->SetLength(m_length);

    SetConstructor(classTemplate);

    Local<ObjectTemplate> objectTemplate = classTemplate->InstanceTemplate();
    objectTemplate->SetInternalFieldCount(CV8::EInternalFieldPurpose::Count);
    SetAccessors(objectTemplate);
    objectTemplate->Set(Symbol::GetToStringTag(pIsolate), CV8Utils::ToV8String(m_name));

    classTemplate->Set(Symbol::GetToStringTag(pIsolate), CV8Utils::ToV8String(m_name));
    classTemplate->SetClassName(CV8Utils::ToV8String(m_name));
    return handleScope.Escape(classTemplate);
}

void CV8ExportClass::AttachGC(Isolate* isolate, Local<Object> object)
{
    new JavascriptWrapper(isolate, object, m_classId);
}

void* CV8ExportClass::Allocate(Isolate* isolate)
{
    ArrayBuffer::Allocator* allocator = isolate->GetArrayBufferAllocator();

    isolate->AdjustAmountOfExternalAllocatedMemory(m_sizeOf);
    return allocator->AllocateUninitialized(m_sizeOf);
}

void CV8ExportClass::SetAccessors(Local<ObjectTemplate> objectTemplate)
{
    for (auto const& accessors : m_floatAccessors)
    {
        SetAccessor<float>(objectTemplate, accessors.first, accessors.second.first, accessors.second.second);
    }
}

void CV8ExportClass::SetConstructor(Handle<FunctionTemplate> objectTemplate)
{
    objectTemplate->SetCallHandler(
        [](const FunctionCallbackInfo<Value>& info) {
            Isolate*        pV8Isolate = Isolate::GetCurrent();
            HandleScope     scope(pV8Isolate);
            Local<External> data = info.Data().As<External>();
            CV8ExportClass* that = (CV8ExportClass*)data->Value();

            //if (info.Length() != that->GetParametersCount())
            //{
            //    pV8Isolate->ThrowException(CV8Utils::ToV8String("Error"));
            //    return;
            //}

            std::function<void(CV8FunctionCallbackBase*)> constructionFunction = that->GetConstrutorFunction();
            CV8FunctionCallback                                   funcCallback(info);

            constructionFunction(&funcCallback);

            Local<Object> wrapper = info.Holder();

            wrapper->SetInternalField(CV8::EInternalFieldPurpose::TypeOfClass, CV8Utils::ToV8Number((double)that->GetClassId()));
            wrapper->SetInternalField(CV8::EInternalFieldPurpose::PointerToValue, External::New(pV8Isolate, funcCallback.GetReturnValuePtr()));

            that->AttachGC(pV8Isolate, wrapper);

            info.GetReturnValue().Set(wrapper);
        },
        External::New(Isolate::GetCurrent(), this));
}

void CV8ExportClass::AddAccessor(std::string name, float (*getter)(void*), void (*setter)(void*, float))
{
    assert(m_floatAccessors.find(name) == m_floatAccessors.end());
    m_length++;
    m_floatAccessors[name] = {getter, setter};
}
