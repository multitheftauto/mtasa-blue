#include "StdInc.h"
#include "CVector2D.h"

using namespace v8;

void CV8Vector2D::GetX(Local<Name> property, const PropertyCallbackInfo<Value>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
    void*           ptr = wrap->Value();
    float           value = static_cast<CVector2D*>(ptr)->fX;
    info.GetReturnValue().Set(value);
}

void CV8Vector2D::SetX(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
    void*           ptr = wrap->Value();
    static_cast<CVector2D*>(ptr)->fX = value->NumberValue(info.GetIsolate()->GetCurrentContext()).ToChecked();
}

void CV8Vector2D::GetY(Local<Name> property, const PropertyCallbackInfo<Value>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
    void*           ptr = wrap->Value();
    float           value = static_cast<CVector2D*>(ptr)->fY;
    info.GetReturnValue().Set(value);
}

void CV8Vector2D::SetY(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
    void*           ptr = wrap->Value();
    static_cast<CVector2D*>(ptr)->fY = value->NumberValue(info.GetIsolate()->GetCurrentContext()).ToChecked();
}

void CV8Vector2D::MethodGetLength(const FunctionCallbackInfo<Value>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
    void*           ptr = wrap->Value();
    CVector2D*      value = static_cast<CVector2D*>(ptr);
    info.GetReturnValue().Set(value->Length());
}

void CV8Vector2D::ConstructorCall(const FunctionCallbackInfo<Value>& info)
{
    HandleScope handleScope(info.GetIsolate());
    if (!ConstructorCallCheck(info))
        return;

    double              x, y;
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
    Local<Context> context = info.GetIsolate()->GetCurrentContext();
    Local<Object>  wrapper = info.Holder();

    CVector2D* point = new CVector2D(x, y);
    wrapper->SetInternalField(0, External::New(info.GetIsolate(), point));
    info.GetReturnValue().Set(wrapper);
}

void CV8Vector2D::CreateTemplate(Local<Context> context)
{
    Isolate*       isolate = context->GetIsolate();
    Isolate::Scope isolate_scope(isolate);
    HandleScope    handle_scope(isolate);

    Handle<FunctionTemplate> vector2dTemplate = FunctionTemplate::New(isolate);
    vector2dTemplate->SetCallHandler(ConstructorCall);
    vector2dTemplate->SetLength(2);
    vector2dTemplate->SetClassName(String::NewFromUtf8(isolate, m_szName).ToLocalChecked());
    Local<ObjectTemplate> objectTemplate = vector2dTemplate->InstanceTemplate();
    objectTemplate->SetInternalFieldCount(1);

    objectTemplate->SetAccessor(String::NewFromUtf8(isolate, "x").ToLocalChecked(), GetX, SetX);
    objectTemplate->SetAccessor(String::NewFromUtf8(isolate, "y").ToLocalChecked(), GetY, SetY);
    objectTemplate->Set(String::NewFromUtf8(isolate, "getLength").ToLocalChecked(), FunctionTemplate::New(isolate, MethodGetLength));
    objectTemplate->Set(Symbol::GetToStringTag(isolate), String::NewFromUtf8(isolate, m_szName).ToLocalChecked());
    context->Global()->Set(context, String::NewFromUtf8(isolate, m_szName).ToLocalChecked(), vector2dTemplate->GetFunction(context).ToLocalChecked());
}
