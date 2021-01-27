#include "StdInc.h"
#include "CVector2D.h"

using namespace v8;

void CV8Vector2D::GetX(Local<Name> property, const PropertyCallbackInfo<Value>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(1));
    void*           ptr = wrap->Value();
    float           value = static_cast<CVector2D*>(ptr)->fX;
    info.GetReturnValue().Set(value);
}

void CV8Vector2D::SetX(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(1));
    void*           ptr = wrap->Value();
    static_cast<CVector2D*>(ptr)->fX = value->NumberValue(info.GetIsolate()->GetCurrentContext()).ToChecked();
}

void CV8Vector2D::GetY(Local<Name> property, const PropertyCallbackInfo<Value>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(1));
    void*           ptr = wrap->Value();
    float           value = static_cast<CVector2D*>(ptr)->fY;
    info.GetReturnValue().Set(value);
}

void CV8Vector2D::SetY(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(1));
    void*           ptr = wrap->Value();
    static_cast<CVector2D*>(ptr)->fY = value->NumberValue(info.GetIsolate()->GetCurrentContext()).ToChecked();
}

void CV8Vector2D::MethodGetLength(const FunctionCallbackInfo<Value>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(1));
    void*           ptr = wrap->Value();
    CVector2D*      value = static_cast<CVector2D*>(ptr);
    info.GetReturnValue().Set(value->Length());
}

void CV8Vector2D::ConstructorCall(const FunctionCallbackInfo<Value>& info)
{
    Isolate*    isolate = info.GetIsolate();
    HandleScope handleScope(isolate);
    if (!ConstructorCallCheck(info))
        return;

    double              x, y;
    CV8FunctionCallback args(info);
    if (!args.ReadNumber(x))
    {
        isolate->ThrowException(String::NewFromUtf8(isolate, "Expected number at argument 1").ToLocalChecked());
        return;
    }
    if (!args.ReadNumber(y))
    {
        isolate->ThrowException(String::NewFromUtf8(isolate, "Expected number at argument 2").ToLocalChecked());
        return;
    }
    Local<Context> context = isolate->GetCurrentContext();
    Local<Object>  wrapper = info.Holder();

    CVector2D* vector = Allocate<CVector2D>(isolate);
    wrapper->SetInternalField(0, Number::New(isolate, (double)m_eClass));
    wrapper->SetInternalField(1, External::New(isolate, vector));
    AttachGC(wrapper, sizeof(CVector2D));
    info.GetReturnValue().Set(wrapper);
}

Handle<FunctionTemplate> CV8Vector2D::CreateTemplate(Local<Context> context)
{
    Isolate*                 isolate = context->GetIsolate();
    v8::EscapableHandleScope handleScope{isolate};

    Handle<FunctionTemplate> vector2dTemplate = FunctionTemplate::New(isolate);
    vector2dTemplate->SetCallHandler(ConstructorCall);
    vector2dTemplate->SetLength(2);
    vector2dTemplate->SetClassName(String::NewFromUtf8(isolate, m_szName).ToLocalChecked());
    Local<ObjectTemplate> objectTemplate = vector2dTemplate->InstanceTemplate();
    objectTemplate->SetInternalFieldCount(2);

    objectTemplate->SetAccessor(String::NewFromUtf8(isolate, "x").ToLocalChecked(), GetX, SetX);
    objectTemplate->SetAccessor(String::NewFromUtf8(isolate, "y").ToLocalChecked(), GetY, SetY);
    objectTemplate->Set(String::NewFromUtf8(isolate, "getLength").ToLocalChecked(), FunctionTemplate::New(isolate, MethodGetLength));
    objectTemplate->Set(Symbol::GetToStringTag(isolate), String::NewFromUtf8(isolate, m_szName).ToLocalChecked());
    context->Global()->Set(context, String::NewFromUtf8(isolate, m_szName).ToLocalChecked(), vector2dTemplate->GetFunction(context).ToLocalChecked());
    return handleScope.Escape(vector2dTemplate);
}
