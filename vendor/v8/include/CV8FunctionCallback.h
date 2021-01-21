class CV8FunctionCallback : public CV8FunctionCallbackBase
{
public:
    CV8FunctionCallback(const FunctionCallbackInfo<Value>& callback);
    std::string ReadString();

    void Return(std::string str);

private:
    const FunctionCallbackInfo<Value>& m_callback;
    int                                m_iIndex = 0;
};