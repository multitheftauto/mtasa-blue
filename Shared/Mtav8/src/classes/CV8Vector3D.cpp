#include "StdInc.h"
#include "CVector.h"

using namespace v8;

float CV8Vector3D::GetZ(CVector* internalValue)
{
    return internalValue->fZ;
}

void CV8Vector3D::SetZ(CVector* internalValue, float value)
{
    internalValue->fZ = value;
}

float CV8Vector3D::MethodGetLength(CV8FunctionCallback& info, Local<Object> self, CVector* value)
{
    return value->Length();
}

float CV8Vector3D::MethodGetLengthSquared(CV8FunctionCallback& info, Local<Object> self, CVector* value)
{
    return value->LengthSquared();
}

void CV8Vector3D::MethodCrossProduct(CV8FunctionCallback& info, Local<Object> self, CVector* value)
{
    CVector other;
    if (!info.Read(other))
        return;
    value->CrossProduct(&other);
}

void CV8Vector3D::MethodDotProduct(CV8FunctionCallback& info, Local<Object> self, CVector* value)
{
    CVector other;
    if (!info.Read(other))
        return;
    value->DotProduct(&other);
}

void CV8Vector3D::MethodNormalize(CV8FunctionCallback& info, Local<Object> self, CVector* value)
{
    value->Normalize();
}

bool CV8Vector3D::ConstructorCall(CV8FunctionCallback& info, Local<Object> object, CVector* value)
{
    double x, y, z;
    if (!info.Read(x, y, z))
        return false;
    value->fX = x;
    value->fY = y;
    value->fZ = z;
    object->SetInternalField(EInternalFieldPurpose::TypeOfClass, CV8Utils::ToV8Number((double)m_eClass));
    return true;
}

bool CV8Vector3D::Convert(Local<Object> object, CVector& vector)
{
    Local<Number> type = Local<Number>::Cast(object->GetInternalField(EInternalFieldPurpose::TypeOfClass));
    if (!type.IsEmpty() && type->Value() == (double)m_eClass)
    {
        Local<External> wrap = Local<External>::Cast(object->GetInternalField(EInternalFieldPurpose::PointerToValue));
        vector = *(CVector*)wrap->Value();
        return true;
    }
    return false;
}

MaybeLocal<Object> CV8Vector3D::New(CVector vector)
{
    Isolate* isolate = Isolate::GetCurrent();
    assert(isolate);
    Locker               lock(isolate);
    EscapableHandleScope handleScope(isolate);
    MaybeLocal<Object>   object = CV8Utils::NewObject(m_szName, vector.fX, vector.fY, vector.fZ);
    return handleScope.EscapeMaybe(object);
}

Handle<FunctionTemplate> CV8Vector3D::CreateTemplate(Local<Context> context, Handle<FunctionTemplate> parent)
{
    Isolate*             isolate = context->GetIsolate();
    EscapableHandleScope handleScope(isolate);

    Handle<FunctionTemplate> vector3dTemplate = FunctionTemplate::New(isolate);
    vector3dTemplate->Inherit(parent);

    SetConstructor(vector3dTemplate, ConstructorCall);
    vector3dTemplate->SetLength(sizeof(CVector) / sizeof(float));
    vector3dTemplate->SetClassName(CV8Utils::ToV8String(m_szName));
    Local<ObjectTemplate> objectTemplate = vector3dTemplate->InstanceTemplate();
    objectTemplate->SetInternalFieldCount(EInternalFieldPurpose::Count);

    SetAccessor(objectTemplate, "z", GetZ, SetZ);
    AddMethod(objectTemplate, "getLenght", MethodGetLength);
    AddMethod(objectTemplate, "getLenghtSquared", MethodGetLengthSquared);
    AddMethod(objectTemplate, "crossProduct", MethodCrossProduct);
    AddMethod(objectTemplate, "dotProduct", MethodDotProduct);
    AddMethod(objectTemplate, "normalize", MethodNormalize);

    objectTemplate->Set(Symbol::GetToStringTag(isolate), CV8Utils::ToV8String(m_szName));
    context->Global()->Set(context, CV8Utils::ToV8String(m_szName), vector3dTemplate->GetFunction(context).ToLocalChecked());
    return handleScope.Escape(vector3dTemplate);
}
