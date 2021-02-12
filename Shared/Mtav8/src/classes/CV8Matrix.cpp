#include "StdInc.h"
#include "CMatrix.h"

using namespace v8;

CVector CV8Matrix::GetPosition(CMatrix* internalValue)
{
    return internalValue->GetPosition();
}

void CV8Matrix::SetPosition(CMatrix* internalValue, CVector value)
{
    internalValue->SetPosition(value);
}

CVector CV8Matrix::GetRotation(CMatrix* internalValue)
{
    return internalValue->GetRotation();
}

void CV8Matrix::SetRotation(CMatrix* internalValue, CVector value)
{
    internalValue->SetRotation(value);
}

CVector CV8Matrix::GetScale(CMatrix* internalValue)
{
    return internalValue->GetScale();
}

void CV8Matrix::SetScale(CMatrix* internalValue, CVector value)
{
    internalValue->SetScale(value);
}

bool CV8Matrix::ConstructorCall(CV8FunctionCallback& info, Local<Object> object, CMatrix* value)
{
    CVector position, rotation;
    CVector scale(1, 1, 1);
    if (!info.Read(position, rotation, scale))
        return false;

    rotation = rotation / (180.0f / PI);

    value->SetPosition(position);
    value->SetRotation(rotation);
    value->SetScale(scale);
    object->SetInternalField(EInternalFieldPurpose::TypeOfClass, CV8Utils::ToV8Number((double)m_eClass));
    return true;
}

MaybeLocal<Object> CV8Matrix::New(CVector position, CVector rotation, CVector scale)
{
    Isolate* isolate = Isolate::GetCurrent();
    assert(isolate);
    EscapableHandleScope handleScope(isolate);
    MaybeLocal<Object>   vecPosition = CV8Utils::NewObject(CV8Vector3D::m_szName, position.fX, position.fY, position.fZ);
    MaybeLocal<Object>   vecRotation = CV8Utils::NewObject(CV8Vector3D::m_szName, rotation.fX, rotation.fY, rotation.fZ);
    MaybeLocal<Object>   vecScale = CV8Utils::NewObject(CV8Vector3D::m_szName, scale.fX, scale.fY, scale.fZ);
    MaybeLocal<Object>   object = CV8Utils::NewObject(m_szName, vecPosition, vecRotation, vecScale);
    return handleScope.EscapeMaybe(object);
}

Handle<FunctionTemplate> CV8Matrix::CreateTemplate(Local<Context> context)
{
    Isolate*             isolate = context->GetIsolate();
    EscapableHandleScope handleScope{isolate};

    Handle<FunctionTemplate> matrixTemplate = FunctionTemplate::New(isolate);
    SetConstructor(matrixTemplate, ConstructorCall);
    matrixTemplate->SetLength(sizeof(CMatrix) / sizeof(CVector));
    matrixTemplate->SetClassName(CV8Utils::ToV8String(m_szName));
    Local<ObjectTemplate> objectTemplate = matrixTemplate->InstanceTemplate();
    objectTemplate->SetInternalFieldCount(EInternalFieldPurpose::Count);

    SetAccessor(objectTemplate, "position", GetPosition, SetPosition);
    SetAccessor(objectTemplate, "rotation", GetRotation, SetRotation);
    SetAccessor(objectTemplate, "scale", GetScale, SetScale);

    objectTemplate->Set(Symbol::GetToStringTag(isolate), CV8Utils::ToV8String(m_szName));
    context->Global()->Set(context, CV8Utils::ToV8String(m_szName), matrixTemplate->GetFunction(context).ToLocalChecked());
    return handleScope.Escape(matrixTemplate);
}
