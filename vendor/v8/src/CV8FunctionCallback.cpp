#include "StdInc.h"

using namespace v8;

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
