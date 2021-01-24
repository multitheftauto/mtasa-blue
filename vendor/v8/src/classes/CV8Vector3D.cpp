#include "StdInc.h"
#include "CVector.h"

using namespace v8;

void CV8Vector3D::GetZ(Local<Name> property, const PropertyCallbackInfo<Value>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
    void*           ptr = wrap->Value();
    float           value = static_cast<CVector*>(ptr)->fZ;
    info.GetReturnValue().Set(value);
}

void CV8Vector3D::SetZ(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
    void*           ptr = wrap->Value();
    static_cast<CVector*>(ptr)->fZ = value->NumberValue(info.GetIsolate()->GetCurrentContext()).ToChecked();
}

void CV8Vector3D::MethodGetLength(const FunctionCallbackInfo<Value>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
    void*           ptr = wrap->Value();
    CVector*        value = static_cast<CVector*>(ptr);
    info.GetReturnValue().Set(value->Length());
}

void CV8Vector3D::ConstructorCall(const FunctionCallbackInfo<Value>& info)
{
    HandleScope handleScope(info.GetIsolate());
    if (!ConstructorCallCheck(info))
        return;

    double              x, y, z;
    CV8FunctionCallback args(info);
    if (!args.ReadNumber(x))
    {
        info.GetIsolate()->ThrowException(String::NewFromUtf8(info.GetIsolate(), "Expected number at argument 1").ToLocalChecked());
        return;
    }
    if (!args.ReadNumber(y))
    {
        info.GetIsolate()->ThrowException(String::NewFromUtf8(info.GetIsolate(), "Expected number at argument 2").ToLocalChecked());
        return;
    }
    if (!args.ReadNumber(z))
    {
        info.GetIsolate()->ThrowException(String::NewFromUtf8(info.GetIsolate(), "Expected number at argument 3").ToLocalChecked());
        return;
    }
    Local<Context> context = info.GetIsolate()->GetCurrentContext();
    Local<Object>  wrapper = info.Holder();

    CVector* point = new CVector(x, y, z);
    wrapper->SetInternalField(0, External::New(info.GetIsolate(), point));
    info.GetReturnValue().Set(wrapper);
}

Handle<FunctionTemplate> CV8Vector3D::CreateTemplate(Local<Context> context, Handle<FunctionTemplate> parent)
{
    Isolate*       isolate = context->GetIsolate();
    v8::EscapableHandleScope handleScope{isolate};

    Handle<FunctionTemplate> vector3dTemplate = FunctionTemplate::New(isolate);
    vector3dTemplate->Inherit(parent);
    vector3dTemplate->SetCallHandler(ConstructorCall);
    vector3dTemplate->SetLength(3);
    vector3dTemplate->SetClassName(String::NewFromUtf8(isolate, m_szName).ToLocalChecked());
    Local<ObjectTemplate> objectTemplate = vector3dTemplate->InstanceTemplate();
    objectTemplate->SetInternalFieldCount(1);

    objectTemplate->SetAccessor(String::NewFromUtf8(isolate, "z").ToLocalChecked(), GetZ, SetZ);
    objectTemplate->Set(String::NewFromUtf8(isolate, "getLength").ToLocalChecked(), FunctionTemplate::New(isolate, MethodGetLength));
    objectTemplate->Set(Symbol::GetToStringTag(isolate), String::NewFromUtf8(isolate, m_szName).ToLocalChecked());
    context->Global()->Set(context, String::NewFromUtf8(isolate, m_szName).ToLocalChecked(), vector3dTemplate->GetFunction(context).ToLocalChecked());
    return handleScope.Escape(vector3dTemplate);
}
