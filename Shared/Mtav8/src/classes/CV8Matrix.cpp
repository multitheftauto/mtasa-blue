#include "StdInc.h"
#include "CMatrix.h"

using namespace v8;

void CV8Matrix::GetPosition(Local<Name> property, const PropertyCallbackInfo<Value>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(1));
    void*           ptr = wrap->Value();
    CVector         value = static_cast<CMatrix*>(ptr)->GetPosition();
    info.GetReturnValue().Set(CV8Vector3D::New(value));
}

void CV8Matrix::SetPosition(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(1));
    void*           ptr = wrap->Value();
    CVector         vector;
    if (value->IsObject() && CV8Vector3D::Convert(Local<Object>::Cast(value), vector))
    {
        static_cast<CMatrix*>(ptr)->SetPosition(vector);
        return;
    }
    info.GetIsolate()->ThrowException(String::NewFromUtf8(info.GetIsolate(), "Value is not Vector3 type").ToLocalChecked());
}

void CV8Matrix::GetRotation(Local<Name> property, const PropertyCallbackInfo<Value>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(1));
    void*           ptr = wrap->Value();
    CVector         value = static_cast<CMatrix*>(ptr)->GetRotation();
    info.GetReturnValue().Set(CV8Vector3D::New(value * (180.0f / PI)));
}

void CV8Matrix::SetRotation(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(1));
    void*           ptr = wrap->Value();
    CVector         vector;
    if (value->IsObject() && CV8Vector3D::Convert(Local<Object>::Cast(value), vector))
    {
        static_cast<CMatrix*>(ptr)->SetRotation(vector * (180.0f / PI));
        return;
    }
    info.GetIsolate()->ThrowException(String::NewFromUtf8(info.GetIsolate(), "Value is not Vector3 type").ToLocalChecked());
}

void CV8Matrix::GetScale(Local<Name> property, const PropertyCallbackInfo<Value>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(1));
    void*           ptr = wrap->Value();
    CVector         value = static_cast<CMatrix*>(ptr)->GetScale();
    info.GetReturnValue().Set(CV8Vector3D::New(value));
}

void CV8Matrix::SetScale(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info)
{
    Local<Object>   self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(1));
    void*           ptr = wrap->Value();
    CVector         vector;
    if (value->IsObject() && CV8Vector3D::Convert(Local<Object>::Cast(value), vector))
    {
        static_cast<CMatrix*>(ptr)->SetScale(vector);
        return;
    }
    info.GetIsolate()->ThrowException(String::NewFromUtf8(info.GetIsolate(), "Value is not Vector3 type").ToLocalChecked());
}

void CV8Matrix::ConstructorCall(const FunctionCallbackInfo<Value>& info)
{
    Isolate*    isolate = info.GetIsolate();
    HandleScope handleScope(isolate);
    if (!ConstructorCallCheck(info))
        return;

    CVector             position, rotation;
    CVector             scale(1, 1, 1);
    CV8FunctionCallback args(info);

    args.ReadVector(position);
    args.ReadVector(rotation);
    args.ReadVector(scale);

    rotation = rotation / (180.0f / PI);

    Local<Context>     context = isolate->GetCurrentContext();
    Local<Object>      wrapper = info.Holder();
    Persistent<Object> matrixObject = Persistent<Object>(isolate, wrapper);

    CMatrix* matrix = Allocate<CMatrix>(isolate);
    matrix = new(matrix) CMatrix(position, rotation, scale);
    wrapper->SetInternalField(0, Number::New(isolate, (double)m_eClass));
    wrapper->SetInternalField(1, External::New(isolate, matrix));
    AttachGC(wrapper, sizeof(CMatrix));

    info.GetReturnValue().Set(matrixObject.Get(isolate));
}

Handle<FunctionTemplate> CV8Matrix::CreateTemplate(Local<Context> context)
{
    Isolate*             isolate = context->GetIsolate();
    EscapableHandleScope handleScope{isolate};

    Handle<FunctionTemplate> vector2dTemplate = FunctionTemplate::New(isolate);
    vector2dTemplate->SetCallHandler(ConstructorCall);
    vector2dTemplate->SetLength(16);
    vector2dTemplate->SetClassName(String::NewFromUtf8(isolate, m_szName).ToLocalChecked());
    Local<ObjectTemplate> objectTemplate = vector2dTemplate->InstanceTemplate();
    objectTemplate->SetInternalFieldCount(2);

    objectTemplate->SetAccessor(String::NewFromUtf8(isolate, "position").ToLocalChecked(), GetPosition, SetPosition);
    objectTemplate->SetAccessor(String::NewFromUtf8(isolate, "rotation").ToLocalChecked(), GetRotation, SetRotation);
    objectTemplate->SetAccessor(String::NewFromUtf8(isolate, "scale").ToLocalChecked(), GetScale, SetScale);
    objectTemplate->Set(Symbol::GetToStringTag(isolate), String::NewFromUtf8(isolate, m_szName).ToLocalChecked());
    context->Global()->Set(context, String::NewFromUtf8(isolate, m_szName).ToLocalChecked(), vector2dTemplate->GetFunction(context).ToLocalChecked());
    return handleScope.Escape(vector2dTemplate);
}
