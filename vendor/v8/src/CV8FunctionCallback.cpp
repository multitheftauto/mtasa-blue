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

std::string CV8FunctionCallback::ReadString()
{
    if (m_callback.Length() > m_iIndex)
    {
        String::Utf8Value str(m_callback.GetIsolate(), m_callback[m_iIndex].As<String>());
        m_iIndex++;
        return std::string(*str);
    }
    return "";
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
    Local<String> result = String::NewFromUtf8(m_callback.GetIsolate(), arg.c_str()).ToLocalChecked();
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

void CV8FunctionCallback::ReturnPromise(std::function<void(CV8PromiseBase*)> callback)
{
    CV8Promise* pPromise = new CV8Promise(m_callback.GetIsolate());
    CV8Isolate* pThisIsolate = (CV8Isolate*)m_callback.GetIsolate()->GetData(0);

    pPromise->m_callback = callback;
    pThisIsolate->AddPromise(pPromise);
    m_callback.GetReturnValue().Set(pPromise->GetPromise());
}
