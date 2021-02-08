#include "StdInc.h"
#include "CVector.h"

using namespace v8;

void CV8Vector3D::GetZ(Local<Name> property, const PropertyCallbackInfo<Value>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(1));
    void*           ptr = wrap->Value();
    float           value = static_cast<CVector*>(ptr)->fZ;
    info.GetReturnValue().Set(value);
}

void CV8Vector3D::SetZ(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(1));
    void*           ptr = wrap->Value();
    static_cast<CVector*>(ptr)->fZ = value->NumberValue(info.GetIsolate()->GetCurrentContext()).ToChecked();
}

float CV8Vector3D::MethodGetLength(CV8FunctionCallback& info, Local<Object> self, CVector* value)
{
    return value->Length();
}

float CV8Vector3D::MethodGetLengthSquared(CV8FunctionCallback& info, Local<Object> self, CVector* value)
{
    return value->LengthSquared();
}

//CVector CV8Vector3D::MethodCrossProduct(CV8FunctionCallback& info, Local<Object> self, CVector* value)
//{
//    CVector other;
//    if (!info.ReadVector(other))
//    {
//        throw std::invalid_argument("Expected vector at argument 1");
//    }
//    CVector thisValue = CVector(value->fX, value->fY, value->fZ);
//    other.CrossProduct(&thisValue);
//    return other;
//}

void CV8Vector3D::MethodCrossProduct(CV8FunctionCallback& info, Local<Object> self, CVector* value)
{
    CVector other;
    if (!info.ReadVector(other))
    {
        throw std::invalid_argument("Expected vector at argument 1");
    }
    value->CrossProduct(&other);
}

void CV8Vector3D::MethodDotProduct(CV8FunctionCallback& info, Local<Object> self, CVector* value)
{
    CVector other;
    if (!info.ReadVector(other))
    {
        throw std::invalid_argument("Expected vector at argument 1");
    }
    value->DotProduct(&other);
}

void CV8Vector3D::MethodNormalize(CV8FunctionCallback& info, Local<Object> self, CVector* value)
{
    value->Normalize();
}

void CV8Vector3D::ConstructorCall(const FunctionCallbackInfo<Value>& info)
{
    auto        isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (!ConstructorCallCheck(info))
        return;

    double              x, y, z;
    CV8FunctionCallback args(info);
    if (!args.ReadNumber(x))
    {
        isolate->ThrowException(CV8Utils::ToV8String("Expected number at argument 1"));
        return;
    }
    if (!args.ReadNumber(y))
    {
        isolate->ThrowException(CV8Utils::ToV8String("Expected number at argument 2"));
        return;
    }
    if (!args.ReadNumber(z))
    {
        isolate->ThrowException(CV8Utils::ToV8String("Expected number at argument 3"));
        return;
    }
    Local<Context>          context = isolate->GetCurrentContext();
    Local<Object>           wrapper = info.Holder();
    ArrayBuffer::Allocator* allocator = isolate->GetArrayBufferAllocator();

    CVector* vector = CreateGarbageCollected<CVector>(wrapper);
    vector->fX = x;
    vector->fY = y;
    vector->fZ = z;

    wrapper->SetInternalField(0, Number::New(isolate, (double)m_eClass));
    wrapper->SetInternalField(1, External::New(isolate, vector));

    info.GetReturnValue().Set(wrapper);
}

bool CV8Vector3D::Convert(Local<Object> object, CVector& vector)
{
    Local<Number> type = Local<Number>::Cast(object->GetInternalField(0));
    if (!type.IsEmpty() && type->Value() == (double)m_eClass)
    {
        Local<External> wrap = Local<External>::Cast(object->GetInternalField(1));
        vector = *(CVector*)wrap->Value();
        return true;
    }
    return false;
}

MaybeLocal<Object> CV8Vector3D::New(CVector vector)
{
    Isolate* isolate = Isolate::GetCurrent();
    assert(isolate);
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
    vector3dTemplate->SetCallHandler(ConstructorCall);
    vector3dTemplate->SetLength(sizeof(CVector) / sizeof(float));
    vector3dTemplate->SetClassName(CV8Utils::ToV8String(m_szName));
    Local<ObjectTemplate> objectTemplate = vector3dTemplate->InstanceTemplate();
    objectTemplate->SetInternalFieldCount(2);
    objectTemplate->SetAccessor(CV8Utils::ToV8String("z"), GetZ, SetZ);
    AddMethod<float, CVector>(objectTemplate, "getLenght", MethodGetLength);
    AddMethod<float, CVector>(objectTemplate, "getLenghtSquared", MethodGetLengthSquared);
    AddMethod<void, CVector>(objectTemplate, "crossProduct", MethodCrossProduct);
    AddMethod<void, CVector>(objectTemplate, "dotProduct", MethodDotProduct);
    AddMethod<void, CVector>(objectTemplate, "normalize", MethodNormalize);
    objectTemplate->Set(Symbol::GetToStringTag(isolate), CV8Utils::ToV8String(m_szName));
    context->Global()->Set(context, CV8Utils::ToV8String(m_szName), vector3dTemplate->GetFunction(context).ToLocalChecked());
    return handleScope.Escape(vector3dTemplate);
}
