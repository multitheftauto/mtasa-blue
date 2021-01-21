class CV8FunctionCallbackBase
{
public:
    virtual int CountArguments() const = 0;

    virtual std::string ReadString() = 0;

    virtual void        Return(std::string arg) = 0;
    virtual void        Return(bool arg) = 0;
};
