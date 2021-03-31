class CV8EnumBase;

class CV8ModuleBase
{
public:
    virtual void AddFunction(std::string name, void (*args)(CV8FunctionCallbackBase*)) = 0;
    virtual void AddEnum(std::string name, CV8EnumBase* pEnum) = 0;
};
