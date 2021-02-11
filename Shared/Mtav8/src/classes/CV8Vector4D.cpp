#include "StdInc.h"
#include "CVector4D.h"

using namespace v8;

void CV8Vector4D::GetW(Local<Name> property, const PropertyCallbackInfo<Value>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(EInternalFieldPurpose::PointerToValue));
    void*           ptr = wrap->Value();
    float           value = static_cast<CVector4D*>(ptr)->fW;
    info.GetReturnValue().Set(value);
}

void CV8Vector4D::SetW(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(EInternalFieldPurpose::PointerToValue));
    void*           ptr = wrap->Value();
    static_cast<CVector4D*>(ptr)->fW = value->NumberValue(info.GetIsolate()->GetCurrentContext()).ToChecked();
}

void CV8Vector4D::MethodGetLength(const FunctionCallbackInfo<Value>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(EInternalFieldPurpose::PointerToValue));
    void*           ptr = wrap->Value();
    CVector4D*      value = static_cast<CVector4D*>(ptr);
    info.GetReturnValue().Set(value->Length());
}

bool CV8Vector4D::ConstructorCall(CV8FunctionCallback& info, Local<Object> object, CVector4D* value)
{
    double x, y, z, w;
    if (!info.Read(x, y, z, w))
        return false;
    value->fX = x;
    value->fY = y;
    value->fZ = z;
    value->fW = w;
    object->SetInternalField(EInternalFieldPurpose::TypeOfClass, CV8Utils::ToV8Number((double)m_eClass));
    return true;
}

bool CV8Vector4D::Convert(Local<Object> object, CVector4D& vector)
{
    Local<Number> type = Local<Number>::Cast(object->GetInternalField(EInternalFieldPurpose::TypeOfClass));
    if (!type.IsEmpty() && type->Value() == (double)m_eClass)
    {
        Local<External> wrap = Local<External>::Cast(object->GetInternalField(EInternalFieldPurpose::PointerToValue));
        vector = *(CVector4D*)wrap->Value();
        return true;
    }
    return false;
}

MaybeLocal<Object> CV8Vector4D::New(CVector4D vector)
{
    Isolate* isolate = Isolate::GetCurrent();
    assert(isolate);
    EscapableHandleScope handleScope(isolate);
    MaybeLocal<Object>   object = CV8Utils::NewObject(m_szName, vector.fX, vector.fY, vector.fZ, vector.fW);
    return handleScope.EscapeMaybe(object);
}

Handle<FunctionTemplate> CV8Vector4D::CreateTemplate(Local<Context> context, Handle<FunctionTemplate> parent)
{
    Isolate*             isolate = context->GetIsolate();
    EscapableHandleScope handleScope(isolate);

    Handle<FunctionTemplate> vector4dTemplate = FunctionTemplate::New(isolate);
    vector4dTemplate->Inherit(parent);
    SetConstructor(vector4dTemplate, ConstructorCall);

    vector4dTemplate->SetLength(sizeof(CVector4D) / sizeof(float));
    vector4dTemplate->SetClassName(CV8Utils::ToV8String(m_szName));
    Local<ObjectTemplate> objectTemplate = vector4dTemplate->InstanceTemplate();
    objectTemplate->SetInternalFieldCount(EInternalFieldPurpose::Count);
    objectTemplate->SetAccessor(CV8Utils::ToV8String("w"), GetW, SetW);
    objectTemplate->Set(CV8Utils::ToV8String("getLength"), FunctionTemplate::New(isolate, MethodGetLength));
    objectTemplate->Set(Symbol::GetToStringTag(isolate), CV8Utils::ToV8String(m_szName));
    context->Global()->Set(context, CV8Utils::ToV8String(m_szName), vector4dTemplate->GetFunction(context).ToLocalChecked());
    return handleScope.Escape(vector4dTemplate);
}
