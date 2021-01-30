class CV8FunctionCallbackBase
{
public:
    virtual int CountArguments() const = 0;

    virtual std::string ReadString() = 0;
    virtual bool        ReadNumber(double& value) = 0;

    virtual const char* GetType() = 0;

    virtual void Return(std::string arg) = 0;
    virtual void Return(double arg) = 0;
    virtual void Return(bool arg) = 0;
    virtual void ReturnPromise(std::function<void(CV8PromiseBase*)> callback) = 0;

    virtual void ReturnPromiseNew(std::unique_ptr<class CV8AsyncFunction> pAsyncFunction) = 0;
};
