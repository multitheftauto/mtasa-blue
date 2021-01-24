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

void CV8Vector2D::ConstructorCall(const FunctionCallbackInfo<Value>& info)
{
    if (!info.IsConstructCall())
    {
        int a = 5;
        // Isolate::ThrowException(String::NewFromUtf8(info.GetIsolate(), "Cannot call constructor as function").ToLocalChecked());
    }

    v8::HandleScope handle_scope(info.GetIsolate());
    Local<Context>  context = info.GetIsolate()->GetCurrentContext();

    double x = info[0]->ToNumber(context).ToLocalChecked()->Value();
    double y = info[1]->ToNumber(context).ToLocalChecked()->Value();

    v8::Local<v8::Object> wrapper = info.Holder();

    CVector2D* point = new CVector2D(x, y);
    wrapper->SetInternalField(0, v8::External::New(info.GetIsolate(), point));
    info.GetReturnValue().Set(wrapper);
}

void CV8Vector2D::CreateTemplate(Local<Context> context)
{
    Isolate*       isolate = context->GetIsolate();
    Isolate::Scope isolate_scope(isolate);
    HandleScope    handle_scope(isolate);

    Handle<FunctionTemplate> point_template = FunctionTemplate::New(isolate);
    point_template->SetCallHandler(ConstructorCall);
    point_template->SetLength(1);
    point_template->SetClassName(String::NewFromUtf8(isolate, "Vector2").ToLocalChecked());
    Local<ObjectTemplate> objectTemplate = point_template->InstanceTemplate();
    objectTemplate->SetCallAsFunctionHandler(ConstructorCall);
    objectTemplate->SetInternalFieldCount(1);

    objectTemplate->SetAccessor(String::NewFromUtf8(isolate, "x").ToLocalChecked(), GetX, SetX);
    objectTemplate->SetAccessor(String::NewFromUtf8(isolate, "y").ToLocalChecked(), GetY, SetY);
    objectTemplate->Set(Symbol::GetToStringTag(isolate), String::NewFromUtf8(isolate, "Vector2").ToLocalChecked());
    context->Global()->Set(context, String::NewFromUtf8(isolate, "Vector2").ToLocalChecked(), point_template->GetFunction(context).ToLocalChecked());
}
