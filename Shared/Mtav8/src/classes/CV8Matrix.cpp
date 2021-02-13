#include "StdInc.h"
#include "CMatrix.h"

using namespace v8;


inline float WrapAround(float fValue, float fHigh)
{
    return fValue - (fHigh * floor(static_cast<float>(fValue / fHigh)));
}

inline float ConvertDegreesToRadians(float fRotation)
{
    return WrapAround(static_cast<float>(fRotation * PI / 180.0f + 2 * PI), static_cast<float>(2 * PI));
}

inline float ConvertRadiansToDegrees(float fRotation)
{
    return WrapAround(static_cast<float>(fRotation * 180.0f / PI + 360.0f), 360.0f);
}

inline void ConvertRadiansToDegrees(CVector& vecRotation)
{
    vecRotation.fX = ConvertRadiansToDegrees(vecRotation.fX);
    vecRotation.fY = ConvertRadiansToDegrees(vecRotation.fY);
    vecRotation.fZ = ConvertRadiansToDegrees(vecRotation.fZ);
}

inline void ConvertDegreesToRadians(CVector& vecRotation)
{
    vecRotation.fX = ConvertDegreesToRadians(vecRotation.fX);
    vecRotation.fY = ConvertDegreesToRadians(vecRotation.fY);
    vecRotation.fZ = ConvertDegreesToRadians(vecRotation.fZ);
}

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
    CVector v = internalValue->GetRotation();
    v.fX *= 180 / PI;
    v.fY *= 180 / PI;
    v.fZ *= 180 / PI;
    return v;
}

void CV8Matrix::SetRotation(CMatrix* internalValue, CVector value)
{
    value *= PI / 180;
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

void CV8Matrix::MethodInverse(CV8FunctionCallback& info, Local<Object> self, CMatrix* internalValue)
{
    internalValue->Inverse();
}

void CV8Matrix::MethodInvert(CV8FunctionCallback& info, Local<Object> self, CMatrix* internalValue)
{
    internalValue->Invert();
}

CVector CV8Matrix::MethodTransformVector(CV8FunctionCallback& info, Local<Object> self, CMatrix* internalValue)
{
    CVector vector;
    if (!info.Read(vector))
        return {};
    return internalValue->TransformVector(vector);
}

bool CV8Matrix::ConstructorCall(CV8FunctionCallback& info, Local<Object> self, CMatrix* internalValue)
{
    CVector position, rotation, scale;
    if (!info.Read(position, rotation, scale))
        return false;

    rotation.fX *= PI / 180;
    rotation.fY *= PI / 180;
    rotation.fZ *= PI / 180;
    //ConvertDegreesToRadians(rotation);
    internalValue->SetPosition(position);
    internalValue->SetRotation(rotation);
    internalValue->SetScale(scale);
    self->SetInternalField(EInternalFieldPurpose::TypeOfClass, CV8Utils::ToV8Number((double)m_eClass));
    return true;
}

MaybeLocal<Object> CV8Matrix::New(CVector position, CVector rotation, CVector scale)
{
    Isolate* isolate = Isolate::GetCurrent();
    assert(isolate);
    Locker               lock(isolate);
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

    AddMethod(objectTemplate, "inverse", MethodInverse);

    objectTemplate->Set(Symbol::GetToStringTag(isolate), CV8Utils::ToV8String(m_szName));
    context->Global()->Set(context, CV8Utils::ToV8String(m_szName), matrixTemplate->GetFunction(context).ToLocalChecked());
    return handleScope.Escape(matrixTemplate);
}
