class CV8AsyncContextBase;

// Manual argument parser
class CV8FunctionCallbackBase
{
public:
    virtual int CountArguments() const = 0;

    virtual bool ReadString(std::string& value, int index) = 0;
    virtual bool ReadAsString(std::string& value, int index) = 0;
    virtual bool ReadNumber(float& value, int index) = 0;
    virtual bool ReadNumber(double& value, int index) = 0;

    virtual std::string GetType(int index) = 0;

    virtual bool IsString(int index) = 0;
    virtual bool IsNumber(int index) = 0;

    virtual void Return(std::string arg) = 0;
    virtual void Return(double arg) = 0;
    virtual void Return(float arg) = 0;
    virtual void Return(bool arg) = 0;
    virtual void Return(void* arg) = 0;
    virtual void ReturnUndefined() = 0;

    virtual void ReturnPromise(std::function<void(CV8AsyncContextBase*)> pAsyncFunction) = 0;
    virtual void ThrowException(std::string exception) = 0;
};
