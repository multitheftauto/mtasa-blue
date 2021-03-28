#include "StdInc.h"

Handle<FunctionTemplate> CV8Class::Initialize(CV8Isolate* pIsolate)
{
    Isolate*                 pV8Isolate = pIsolate->GetIsolate();
    Local<Context>           pV8Context = pV8Isolate->GetCurrentContext();
    EscapableHandleScope     handleScope{pV8Isolate};
    Handle<FunctionTemplate> classTemplate = FunctionTemplate::New(pV8Isolate);
    SetConstructor(classTemplate);

    // vector2dTemplate->SetLength(sizeof(CVector2D) / sizeof(float));
    Local<ObjectTemplate> objectTemplate = classTemplate->InstanceTemplate();
    objectTemplate->SetInternalFieldCount(CV8::EInternalFieldPurpose::Count);

    classTemplate->SetClassName(CV8Utils::ToV8String(m_name));
    objectTemplate->Set(Symbol::GetToStringTag(pV8Isolate), CV8Utils::ToV8String(m_name));
    pV8Isolate->GetCurrentContext()->Global()->Set(pV8Context, CV8Utils::ToV8String(m_name), classTemplate->GetFunction(pV8Context).ToLocalChecked());
    return handleScope.Escape(classTemplate);
}

void CV8Class::AttachGC(Isolate* isolate, Local<Object> object, uint16_t classId)
{
    new JavascriptWrapper(isolate, object, classId);
}

void* CV8Class::Allocate(Isolate* isolate)
{
    ArrayBuffer::Allocator* allocator = isolate->GetArrayBufferAllocator();

    isolate->AdjustAmountOfExternalAllocatedMemory(m_sizeOf);
    return allocator->AllocateUninitialized(m_sizeOf);
}
