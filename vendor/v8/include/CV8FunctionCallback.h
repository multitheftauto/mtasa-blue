class CV8FunctionCallback : public CV8FunctionCallbackBase
{
public:
    CV8FunctionCallback(const FunctionCallbackInfo<Value>& callback);
    int         CountArguments() const { return m_callback.Length(); };
    std::string ReadString();

    const char* GetType();

    void Return(std::string arg);
    void Return(double arg);
    void Return(bool arg);

    void ReturnPromise(std::function<void(CV8PromiseBase*)> callback);

private:
    const FunctionCallbackInfo<Value>& m_callback;
    int                                m_iIndex = 0;
};