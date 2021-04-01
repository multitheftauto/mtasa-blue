#include "StdInc.h"

using namespace v8;

#include "CVector.h";

CV8FunctionCallback::CV8FunctionCallback(const FunctionCallbackInfo<Value>& callback) : m_callback(callback)
{
}

std::string CV8FunctionCallback::GetType(int index)
{
    if (m_callback.Length() > index)
    {
        throw new std::invalid_argument("Index");
    }
    Local<String>     typeName = m_callback[index]->TypeOf(m_callback.GetIsolate());
    String::Utf8Value str(m_callback.GetIsolate(), typeName);
    return *str;
}

bool CV8FunctionCallback::IsString(int index)
{
    if (m_callback.Length() < index)
    {
        throw new std::invalid_argument("Index");
    }
    return m_callback[index]->IsString();
}

bool CV8FunctionCallback::IsNumber(int index)
{
    if (m_callback.Length() < index)
    {
        throw new std::invalid_argument("Index");
    }
    return m_callback[index]->IsNumber();
}

bool CV8FunctionCallback::ReadString(std::string& value, int index)
{
    if (m_callback.Length() < index)
    {
        throw new std::invalid_argument("Index");
    }
    if (m_callback[index]->IsString())
    {
        String::Utf8Value str(m_callback.GetIsolate(), m_callback[index].As<String>());
        value = *str;
        return true;
    }
    return false;
}

bool CV8FunctionCallback::ReadAsString(std::string& value, int index)
{
    if (m_callback.Length() < index)
    {
        throw new std::invalid_argument("Index");
    }
    String::Utf8Value str(m_callback.GetIsolate(), m_callback[index].As<String>());
    value = *str;
    return true;
}

bool CV8FunctionCallback::ReadNumber(float& value, int index)
{
    if (m_callback.Length() < index)
    {
        throw new std::invalid_argument("Index");
    }
    if (m_callback[index]->IsNumber())
    {
        value = (float)m_callback[index].As<Number>()->Value();
        return true;
    }
    return false;
}

bool CV8FunctionCallback::ReadNumber(double& value, int index)
{
    if (m_callback.Length() < index)
    {
        throw new std::invalid_argument("Index");
    }
    if (m_callback[index]->IsNumber())
    {
        value = m_callback[index].As<Number>()->Value();
        return true;
    }
    return false;
}
//
// bool CV8FunctionCallback::ReadVector(CVector& value)
//{
//    if (m_callback.Length() > m_iIndex)
//    {
//        if (m_callback[m_iIndex]->IsObject())
//        {
//            return ReadClass(CV8Vector3D::m_eClass, value);
//        }
//    }
//    m_iIndex++;
//    m_callback.GetIsolate()->ThrowException(CV8Utils::ToV8String(std::string("Expected Vector3 at argument ") + std::to_string(m_iIndex)));
//    return false;
//}
//
// bool CV8FunctionCallback::ReadVector(CVector2D& value)
//{
//    if (m_callback.Length() > m_iIndex)
//    {
//        if (m_callback[m_iIndex]->IsObject())
//        {
//            return ReadClass(CV8Vector2D::m_eClass, value);
//        }
//    }
//    m_iIndex++;
//    m_callback.GetIsolate()->ThrowException(CV8Utils::ToV8String(std::string("Expected Vector3 at argument ") + std::to_string(m_iIndex)));
//    return false;
//}
//
// bool CV8FunctionCallback::ReadVector(CVector4D& value)
//{
//    if (m_callback.Length() > m_iIndex)
//    {
//        if (m_callback[m_iIndex]->IsObject())
//        {
//            return ReadClass(CV8Vector4D::m_eClass, value);
//        }
//    }
//    m_iIndex++;
//    m_callback.GetIsolate()->ThrowException(CV8Utils::ToV8String(std::string("Expected Vector4 at argument ") + std::to_string(m_iIndex)));
//    return false;
//}

void CV8FunctionCallback::Return(std::string arg)
{
    Local<String> result = CV8Utils::ToV8String(arg);
    m_callback.GetReturnValue().Set(result);
}

void CV8FunctionCallback::Return(double arg)
{
    Local<Number> result = Number::New(m_callback.GetIsolate(), arg);
    m_callback.GetReturnValue().Set(result);
}

void CV8FunctionCallback::Return(float arg)
{
    Local<Number> result = Number::New(m_callback.GetIsolate(), arg);
    m_callback.GetReturnValue().Set(result);
}

void CV8FunctionCallback::Return(int arg)
{
    Local<Number> result = Number::New(m_callback.GetIsolate(), arg);
    m_callback.GetReturnValue().Set(result);
}

void CV8FunctionCallback::Return(bool arg)
{
    m_callback.GetReturnValue().Set(Boolean::New(m_callback.GetIsolate(), arg));
}

void CV8FunctionCallback::ReturnUndefined()
{
    m_callback.GetReturnValue().SetUndefined();
}

void CV8FunctionCallback::ReturnPromise(std::function<void(CV8AsyncContextBase*)> pAsyncContext)
{
    CV8Isolate*                 pIsolate = (CV8Isolate*)m_callback.GetIsolate()->GetData(0);
    std::unique_ptr<CV8Promise> pPromise = std::make_unique<CV8Promise>(pIsolate, pAsyncContext);
    Local<Promise>              promise = pPromise->GetPromise();
    pIsolate->AddPromise(std::move(pPromise));
    m_callback.GetReturnValue().Set(promise);
}
