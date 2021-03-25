class CVector;
class CV8AsyncFunction;

class CV8FunctionCallback : public CV8FunctionCallbackBase
{
public:
    CV8FunctionCallback(const FunctionCallbackInfo<Value>& callback);
    int CountArguments() const { return m_callback.Length(); };
    // Returns current argument index for error purpose
    int GetArgumentIndex() const { return m_iIndex; };
    // Return true when one of "read" method failed while reading argument.
    bool     HasError() const { return bHasError; };
    Isolate* GetIsolate() const { return m_callback.GetIsolate(); };

    template <typename T>
    constexpr bool Read(T& arg)
    {
        if (CountArguments() == 0)
        {
            GetIsolate()->ThrowException(CV8Utils::ToV8String("Expected 1 argument, got 0 arguments."));

            return false;
        }
        using rawT = std::remove_reference_t<T>;
        if constexpr (std::is_same<rawT, double>::value)
        {
            return ReadNumber(arg);
        }
        else if constexpr (std::is_same<rawT, CVector2D>::value)
        {
            return ReadVector(arg);
        }
        else if constexpr (std::is_same<rawT, CVector>::value)
        {
            return ReadVector(arg);
        }
        else if constexpr (std::is_same<rawT, CVector4D>::value)
        {
            return ReadVector(arg);
        }
        //else
        //    static_assert(false && "Unimplemented read type");
    }

    template <typename T, typename... Ty>
    constexpr bool Read(T& arg, Ty&... args)
    {
        if (1 + sizeof...(args) > CountArguments())
        {
            auto exception = std::string("Expected ") + std::to_string(CountArguments()) + std::string(" argument, got ") + std::to_string(1 + sizeof...(args));
            GetIsolate()->ThrowException(CV8Utils::ToV8String(exception));
            return false;
        }
        if (Read(arg))
        {
            if (sizeof...(args) > 0)
            {
                return Read(args...);
            }
        }
        return true;
    }

    bool ReadString(std::string& value, bool strict = true);
    bool ReadNumber(double& value);
    bool ReadVector(CVector2D& value);
    bool ReadVector(CVector& value);
    bool ReadVector(CVector4D& value);

    const char* GetType();

    void Return(std::string arg);
    void Return(double arg);
    void Return(bool arg);

    void ReturnPromise(std::unique_ptr<CV8AsyncFunction> pAsyncFunction);

private:
    template <typename T>
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
        }
        else if constexpr (std::is_same<T, CVector2D>())
        {
            if (CV8Vector2D::Convert(obj, value))
            {
                m_iIndex++;
                return true;
            }
        }
        else if constexpr (std::is_same<T, CVector4D>())
        {
            if (CV8Vector4D::Convert(obj, value))
            {
                m_iIndex++;
                return true;
            }
        }
        else
            static_assert("Not implemented class");

        bHasError = true;
        m_iIndex++;
        return false;
    }
    const FunctionCallbackInfo<Value>& m_callback;
    mutable int                        m_iIndex = 0;
    bool                               bHasError = false;
};
