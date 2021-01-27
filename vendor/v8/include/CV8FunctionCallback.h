class CVector;

class CV8FunctionCallback : public CV8FunctionCallbackBase
{
public:
    CV8FunctionCallback(const FunctionCallbackInfo<Value>& callback);
    int CountArguments() const { return m_callback.Length(); };
    // Returns current argument index for error purpose
    int GetArgumentIndex() const { return m_iIndex; };
    // Return true when one of "read" method failed while reading argument.
    bool        HasError() const { return bHasError; };
    std::string ReadString();
    bool        ReadNumber(double& value);
    bool        ReadVector(CVector& value);

    const char* GetType();

    void Return(std::string arg);
    void Return(double arg);
    void Return(bool arg);

    void ReturnPromise(std::function<void(CV8PromiseBase*)> callback);

private:
    template<typename T>
    bool ReadClass(CV8BaseClass::EClass eClass, T& value)
    {
        Local<Object> obj = m_callback[m_iIndex]->ToObject(m_callback.GetIsolate()->GetCurrentContext()).ToLocalChecked();
        if constexpr (std::is_same<T, CVector>())
        {
            if (CV8Vector3D::Convert(obj, value))
            {
                m_iIndex++;
                return true;
            }
            else
            {
                bHasError = true;
            }
            m_iIndex++;
            return false;
        }
        static_assert("Not implemented class");
    }
    const FunctionCallbackInfo<Value>& m_callback;
    int                                m_iIndex = 0;
    bool                               bHasError = false;
};