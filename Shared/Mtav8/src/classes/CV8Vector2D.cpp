#include "StdInc.h"
#include "CVector2D.h"

using namespace v8;

float CV8Vector2D::GetX(CVector2D* internalValue)
{
    return internalValue->fX;
}

void CV8Vector2D::SetX(CVector2D* internalValue, float value)
{
    internalValue->fX = value;
}

float CV8Vector2D::GetY(CVector2D* internalValue)
{
    return internalValue->fY;
}

void CV8Vector2D::SetY(CVector2D* internalValue, float value)
{
    internalValue->fY = value;
}

float CV8Vector2D::MethodGetLength(CV8FunctionCallback& info, Local<Object> self, CVector2D* value)
{
    return value->Length();
}

float CV8Vector2D::MethodGetLengthSquared(CV8FunctionCallback& info, Local<Object> self, CVector2D* value)
{
    return value->LengthSquared();
}

bool CV8Vector2D::MethodNormalize(CV8FunctionCallback& info, Local<Object> self, CVector2D* value)
{
    value->Normalize();
    return true;
}

float CV8Vector2D::MethodDotProduct(CV8FunctionCallback& info, Local<Object> self, CVector2D* value)
{
    CVector2D other;
    if (!info.ReadVector(other))
    {
        throw std::invalid_argument("Expected vector at argument 1");
    }
    return value->DotProduct(other);
}

bool CV8Vector2D::ConstructorCall(CV8FunctionCallback& info, Local<Object> object, CVector2D* value)
{
    double x, y;
    if (!info.Read(x, y))
        return false;
    value->fX = x;
    value->fY = y;
    object->SetInternalField(EInternalFieldPurpose::TypeOfClass, CV8Utils::ToV8Number((double)m_eClass));
    return true;
}

bool CV8Vector2D::Convert(Local<Object> object, CVector2D& vector)
{
    Local<Number> type = Local<Number>::Cast(object->GetInternalField(EInternalFieldPurpose::TypeOfClass));
    if (!type.IsEmpty() && type->Value() == (double)m_eClass)
    {
        Local<External> wrap = Local<External>::Cast(object->GetInternalField(EInternalFieldPurpose::PointerToValue));
        vector = *(CVector2D*)wrap->Value();
        return true;
    }
    return false;
}

MaybeLocal<Object> CV8Vector2D::New(CVector2D vector)
{
    Isolate* isolate = Isolate::GetCurrent();
    assert(isolate);
    EscapableHandleScope handleScope(isolate);
    MaybeLocal<Object>   object = CV8Utils::NewObject(m_szName, vector.fX, vector.fY);
    return handleScope.EscapeMaybe(object);
}

Handle<FunctionTemplate> CV8Vector2D::CreateTemplate(Local<Context> context)
{
    Isolate*             isolate = context->GetIsolate();
    EscapableHandleScope handleScope{isolate};

    Handle<FunctionTemplate> vector2dTemplate = FunctionTemplate::New(isolate);
    SetConstructor(vector2dTemplate, ConstructorCall);
    vector2dTemplate->SetLength(sizeof(CVector2D) / sizeof(float));
    vector2dTemplate->SetClassName(CV8Utils::ToV8String(m_szName));
    Local<ObjectTemplate> objectTemplate = vector2dTemplate->InstanceTemplate();
    objectTemplate->SetInternalFieldCount(EInternalFieldPurpose::Count);

    SetAccessor(objectTemplate, "x", GetX, SetX);
    SetAccessor(objectTemplate, "y", GetY, SetY);

    AddMethod(objectTemplate, "getLenght", MethodGetLength);
    AddMethod(objectTemplate, "getLengthSquared", MethodGetLengthSquared);
    AddMethod(objectTemplate, "normalize", MethodNormalize);
    AddMethod(objectTemplate, "dotProduct", MethodDotProduct);

    objectTemplate->Set(Symbol::GetToStringTag(isolate), CV8Utils::ToV8String(m_szName));
    context->Global()->Set(context, CV8Utils::ToV8String(m_szName), vector2dTemplate->GetFunction(context).ToLocalChecked());
    return handleScope.Escape(vector2dTemplate);
}
