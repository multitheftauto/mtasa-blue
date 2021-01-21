class CV8ModuleBase
{
public:
    virtual void AddFunction(const char* name, void (*args)(CV8FunctionCallbackBase*)) = 0;
};
