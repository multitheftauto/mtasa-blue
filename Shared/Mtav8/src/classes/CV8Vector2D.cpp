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

void CV8Vector2D::MethodDotProduct(const FunctionCallbackInfo<Value>& info)
{
    HandleScope         handleScope(info.GetIsolate());
    CV8FunctionCallback args(info);
    CVector2D           other;
    if (!args.ReadVector(other))
    {
        info.GetIsolate()->ThrowException(CV8Utils::ToV8String("Expected vector at argument 1"));
        return;
    }
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(1));
    void*           ptr = wrap->Value();
    CVector2D*      value = static_cast<CVector2D*>(ptr);
    info.GetReturnValue().Set(value->DotProduct(other));
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
        isolate->ThrowException(CV8Utils::ToV8String("Expected number at argument 1"));
        return;
    }
    if (!args.ReadNumber(y))
    {
        isolate->ThrowException(CV8Utils::ToV8String("Expected number at argument 2"));
        return;
    }
    Local<Context> context = isolate->GetCurrentContext();
    Local<Object>  wrapper = info.Holder();

    CVector2D* vector = CreateGarbageCollected<CVector2D>(wrapper);
    vector->fX = x;
    vector->fY = y;

    wrapper->SetInternalField(0, Number::New(isolate, (double)m_eClass));
    wrapper->SetInternalField(1, External::New(isolate, vector));
    info.GetReturnValue().Set(wrapper);
}


bool CV8Vector2D::Convert(Local<Object> object, CVector2D& vector)
{
    Local<Number> type = Local<Number>::Cast(object->GetInternalField(0));
    if (!type.IsEmpty() && type->Value() == (double)m_eClass)
    {
        Local<External> wrap = Local<External>::Cast(object->GetInternalField(1));
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
    Isolate*                 isolate = context->GetIsolate();
    v8::EscapableHandleScope handleScope{isolate};

    Handle<FunctionTemplate> vector2dTemplate = FunctionTemplate::New(isolate);
    vector2dTemplate->SetCallHandler(ConstructorCall);
    vector2dTemplate->SetLength(sizeof(CVector2D) / sizeof(float));
    vector2dTemplate->SetClassName(CV8Utils::ToV8String(m_szName));
    Local<ObjectTemplate> objectTemplate = vector2dTemplate->InstanceTemplate();
    objectTemplate->SetInternalFieldCount(2);

    objectTemplate->SetAccessor(CV8Utils::ToV8String("x"), GetX, SetX);
    objectTemplate->SetAccessor(CV8Utils::ToV8String("y"), GetY, SetY);
    objectTemplate->Set(CV8Utils::ToV8String("getLength"), FunctionTemplate::New(isolate, MethodGetLength));
    objectTemplate->Set(CV8Utils::ToV8String("dotProduct"), FunctionTemplate::New(isolate, MethodDotProduct));
    objectTemplate->Set(Symbol::GetToStringTag(isolate), CV8Utils::ToV8String(m_szName));
    context->Global()->Set(context, CV8Utils::ToV8String(m_szName), vector2dTemplate->GetFunction(context).ToLocalChecked());
    return handleScope.Escape(vector2dTemplate);
}
