class CV8ModuleBase
{
public:
    virtual void AddFunction(std::string name, void (*args)(CV8FunctionCallbackBase*)) = 0;
};
