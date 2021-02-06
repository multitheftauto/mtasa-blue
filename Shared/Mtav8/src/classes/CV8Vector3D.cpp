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

void CV8Vector3D::MethodGetLength(const FunctionCallbackInfo<Value>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(1));
    void*           ptr = wrap->Value();
    CVector*        value = static_cast<CVector*>(ptr);
    info.GetReturnValue().Set(value->Length());
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

    CVector* vector = CreateGarbageCollected<CVector>(isolate, wrapper);
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

Local<Object> CV8Vector3D::New(CVector vector)
{
    Isolate*             isolate = Isolate::GetCurrent();
    EscapableHandleScope handleScope(isolate);
    MaybeLocal<Value>    maybeValue =
        isolate->GetCurrentContext()->Global()->Get(isolate->GetCurrentContext(), CV8Utils::ToV8String(m_szName));
    Local<Value> value;
    if (!maybeValue.ToLocal(&value))
    {
        isolate->ThrowException(CV8Utils::ToV8String("Vector3 class not found."));
        return Object::New(isolate);
    }
    if (value->IsFunction())
    {
        Handle<Function> vector3Class = Handle<Function>::Cast(value);
        Handle<Value>    args[3];
        args[0] = Number::New(isolate, vector.fX);
        args[1] = Number::New(isolate, vector.fY);
        args[2] = Number::New(isolate, vector.fZ);
        if (vector3Class->IsConstructor())
        {
            Local<Object> object = Local<Object>::Cast(vector3Class->CallAsConstructor(isolate->GetCurrentContext(), 3, args).ToLocalChecked());
            return handleScope.Escape(object);
        }
    }
    isolate->ThrowException(CV8Utils::ToV8String("Constructor for class Vector3 not found."));
    return Object::New(isolate);
}

Handle<FunctionTemplate> CV8Vector3D::CreateTemplate(Local<Context> context, Handle<FunctionTemplate> parent)
{
    Isolate*             isolate = context->GetIsolate();
    EscapableHandleScope handleScope(isolate);

    Handle<FunctionTemplate> vector3dTemplate = FunctionTemplate::New(isolate);
    vector3dTemplate->Inherit(parent);
    vector3dTemplate->SetCallHandler(ConstructorCall);
    vector3dTemplate->SetLength(3);
    vector3dTemplate->SetClassName(CV8Utils::ToV8String(m_szName));
    Local<ObjectTemplate> objectTemplate = vector3dTemplate->InstanceTemplate();
    objectTemplate->SetInternalFieldCount(2);
    objectTemplate->SetAccessor(CV8Utils::ToV8String("z"), GetZ, SetZ);
    objectTemplate->Set(CV8Utils::ToV8String("getLength"), FunctionTemplate::New(isolate, MethodGetLength));
    objectTemplate->Set(Symbol::GetToStringTag(isolate), CV8Utils::ToV8String(m_szName));
    context->Global()->Set(context, CV8Utils::ToV8String(m_szName), vector3dTemplate->GetFunction(context).ToLocalChecked());
    return handleScope.Escape(vector3dTemplate);
}
