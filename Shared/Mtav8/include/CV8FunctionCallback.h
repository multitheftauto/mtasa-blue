class CVector;
class CV8AsyncFunction;

class CV8FunctionCallback : public CV8FunctionCallbackBase
{
public:
    CV8FunctionCallback(const FunctionCallbackInfo<Value>& callback);
    int CountArguments() const { return m_callback.Length(); };
    // Returns current argument index for error purpose
    // Return true when one of "read" method failed while reading argument.
    bool     HasError() const { return bHasError; };
    Isolate* GetIsolate() const { return m_callback.GetIsolate(); };

    bool ReadString(std::string& value, int index);
    bool ReadAsString(std::string& value, int index);

    bool ReadNumber(float& value, int index);
    bool ReadNumber(double& value, int index);
    //bool ReadVector(CVector2D& value);
    //bool ReadVector(CVector& value);
    //bool ReadVector(CVector4D& value);

    std::string GetType(int index);
    bool IsString(int index);
    bool IsNumber(int index);

    void Return(std::string arg);
    void Return(double arg);
    void Return(float arg);
    void Return(int arg);
    void Return(bool arg);
    void ReturnUndefined();

    void ReturnPromise(std::function<void(CV8AsyncContextBase*)> pAsyncFunction);

private:
    const FunctionCallbackInfo<Value>& m_callback;
    bool                               bHasError = false;
};
