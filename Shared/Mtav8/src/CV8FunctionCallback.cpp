#include "StdInc.h"

using namespace v8;

#include "CVector.h";

CV8FunctionCallback::CV8FunctionCallback(const FunctionCallbackInfo<Value>& callback) : m_callback(callback)
{
}

const char* CV8FunctionCallback::GetType()
{
    Local<String>     typeName = m_callback[m_iIndex]->TypeOf(m_callback.GetIsolate());
    String::Utf8Value str(m_callback.GetIsolate(), typeName);
    return *str;
}

bool CV8FunctionCallback::ReadString(std::string& value, bool strict)
{
    if (m_callback.Length() > m_iIndex)
    {
        if (strict)
        {
            if (!m_callback[m_iIndex]->IsNumber() && !m_callback[m_iIndex]->IsString())
            {
                return false;
            }
        }
        String::Utf8Value str(m_callback.GetIsolate(), m_callback[m_iIndex].As<String>());
        m_iIndex++;
        value = std::string(*str);
        return true;
    }
    return false;
}

bool CV8FunctionCallback::ReadNumber(double& value)
{
    if (m_callback.Length() > m_iIndex)
    {
        if (m_callback[m_iIndex]->IsNumber())
        {
            value = m_callback[m_iIndex].As<Number>()->Value();
            m_iIndex++;
            return true;
        }
    }
    bHasError = true;
    return false;
}

bool CV8FunctionCallback::ReadVector(CVector& value)
{
    if (m_callback.Length() > m_iIndex)
    {
        if (m_callback[m_iIndex]->IsObject())
        {
            return ReadClass(CV8Vector3D::m_eClass, value);
        }
    }
    bHasError = true;
    return false;
}

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

void CV8FunctionCallback::Return(bool arg)
{
    m_callback.GetReturnValue().Set(Boolean::New(m_callback.GetIsolate(), arg));
}

void CV8FunctionCallback::ReturnPromise(std::unique_ptr<CV8AsyncFunction> pAsyncFunction)
{
    CV8Isolate* pThisIsolate = (CV8Isolate*)m_callback.GetIsolate()->GetData(0);
    std::unique_ptr<CV8Promise> pPromise = std::make_unique<CV8Promise>(pThisIsolate, std::move(pAsyncFunction));
    Local<Promise>              promise = pPromise->GetPromise();
    pThisIsolate->AddPromise(std::move(pPromise));
    m_callback.GetReturnValue().Set(promise);
}
