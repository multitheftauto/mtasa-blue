#include "StdInc.h"

Handle<FunctionTemplate> CV8Class::Initialize(CV8Isolate* pIsolate)
{
    Isolate*                 pV8Isolate = pIsolate->GetIsolate();
    Local<Context>           pV8Context = pV8Isolate->GetCurrentContext();
    EscapableHandleScope     handleScope{pV8Isolate};

    Handle<FunctionTemplate> classTemplate = FunctionTemplate::New(pV8Isolate);
    classTemplate->SetLength(m_length);

    SetConstructor(classTemplate);

    Local<ObjectTemplate> objectTemplate = classTemplate->InstanceTemplate();
    objectTemplate->SetInternalFieldCount(CV8::EInternalFieldPurpose::Count);
    SetAccessors(objectTemplate);

    classTemplate->SetClassName(CV8Utils::ToV8String(m_name));
    objectTemplate->Set(Symbol::GetToStringTag(pV8Isolate), CV8Utils::ToV8String(m_name));
    pV8Isolate->GetCurrentContext()->Global()->Set(pV8Context, CV8Utils::ToV8String(m_name), classTemplate->GetFunction(pV8Context).ToLocalChecked());
    return handleScope.Escape(classTemplate);
}

void CV8Class::AttachGC(Isolate* isolate, Local<Object> object)
{
    new JavascriptWrapper(isolate, object, m_classId);
}

void* CV8Class::Allocate(Isolate* isolate)
{
    ArrayBuffer::Allocator* allocator = isolate->GetArrayBufferAllocator();

    isolate->AdjustAmountOfExternalAllocatedMemory(m_sizeOf);
    return allocator->AllocateUninitialized(m_sizeOf);
}

void CV8Class::SetAccessors(Local<ObjectTemplate> objectTemplate)
{
    for (auto const& accessors : m_floatAccessors)
    {
        SetAccessor<float>(objectTemplate, accessors.first, accessors.second.first, accessors.second.second);
    }
}

void CV8Class::SetConstructor(Handle<FunctionTemplate> objectTemplate)
{
    objectTemplate->SetCallHandler(
        [](const FunctionCallbackInfo<Value>& info) {
            Isolate*        pV8Isolate = Isolate::GetCurrent();
            HandleScope     scope(pV8Isolate);
            Local<External> data = info.Data().As<External>();
            CV8Class*       thisClass = (CV8Class*)data->Value();

            if (info.Length() != thisClass->GetParametersCount())
            {
                pV8Isolate->ThrowException(CV8Utils::ToV8String("Error"));
                return;
            }

            std::function<void*(CV8FunctionCallbackBase&, void*)> constructionFunction = thisClass->GetConstrutorFunction();
            CV8FunctionCallback                                   funcCallback(info);
            void*                                                 wrappedClass = constructionFunction(funcCallback, thisClass->Allocate(pV8Isolate));

            Local<Object> wrapper = info.Holder();

            wrapper->SetInternalField(CV8::EInternalFieldPurpose::TypeOfClass, CV8Utils::ToV8Number((double)thisClass->GetClassId()));
            wrapper->SetInternalField(CV8::EInternalFieldPurpose::PointerToValue, External::New(pV8Isolate, wrappedClass));

            thisClass->AttachGC(pV8Isolate, wrapper);

            info.GetReturnValue().Set(wrapper);
        },
        External::New(Isolate::GetCurrent(), this));
}

void CV8Class::AddAccessor(std::string name, float (*getter)(void*), void (*setter)(void*, float))
{
    assert(m_floatAccessors.find(name) == m_floatAccessors.end());
    m_length++;
    m_floatAccessors[name] = {getter, setter};
}
