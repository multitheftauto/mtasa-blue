#include "StdInc.h"
#include "CVector4D.h"

using namespace v8;

float CV8Vector4D::GetW(CVector4D* internalValue)
{
    return internalValue->fW;
}

void CV8Vector4D::SetW(CVector4D* internalValue, float value)
{
    internalValue->fW = value;
}

float CV8Vector4D::MethodGetLength(CV8FunctionCallback& info, Local<Object> self, CVector4D* value)
{
    return value->Length();
}

float CV8Vector4D::MethodGetLengthSquared(CV8FunctionCallback& info, Local<Object> self, CVector4D* value)
{
    return value->LengthSquared();
}

void CV8Vector4D::MethodNormalize(CV8FunctionCallback& info, Local<Object> self, CVector4D* value)
{
    value->Normalize();
}

void CV8Vector4D::MethodDotProduct(CV8FunctionCallback& info, Local<Object> self, CVector4D* value)
{
    CVector4D other;
    if (!info.Read(other))
        return;
    value->DotProduct(other);
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
    Locker               lock(isolate);
    EscapableHandleScope handleScope(isolate);
    MaybeLocal<Object>   object = CV8Utils::NewObject(m_szName, vector.fX, vector.fY, vector.fZ, vector.fW);
    return handleScope.EscapeMaybe(object);
}

Handle<FunctionTemplate> CV8Vector4D::CreateTemplate(Local<Context> context, Handle<FunctionTemplate> parent)
{
    Isolate*             isolate = context->GetIsolate();
    EscapableHandleScope handleScope(isolate);

    Handle<FunctionTemplate> vector4dTemplate = FunctionTemplate::New(isolate);
    SetConstructor(vector4dTemplate, ConstructorCall);

    vector4dTemplate->SetLength(sizeof(CVector4D) / sizeof(float));
    vector4dTemplate->SetClassName(CV8Utils::ToV8String(m_szName));
    Local<ObjectTemplate> objectTemplate = vector4dTemplate->InstanceTemplate();
    objectTemplate->SetInternalFieldCount(EInternalFieldPurpose::Count);

    SetAccessor(objectTemplate, "w", GetW, SetW);
    vector4dTemplate->Inherit(parent);
    AddMethod(objectTemplate, "getLength", MethodGetLength);
    AddMethod(objectTemplate, "getLengthSquared", MethodGetLengthSquared);
    AddMethod(objectTemplate, "normalize", MethodNormalize);
    AddMethod(objectTemplate, "dotProduct", MethodDotProduct);

    objectTemplate->Set(Symbol::GetToStringTag(isolate), CV8Utils::ToV8String(m_szName));
    context->Global()->Set(context, CV8Utils::ToV8String(m_szName), vector4dTemplate->GetFunction(context).ToLocalChecked());
    return handleScope.Escape(vector4dTemplate);
}
