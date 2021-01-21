class CV8FunctionCallbackBase
{
public:
    virtual std::string ReadString() = 0;
    virtual void Return(std::string str) = 0;
};
