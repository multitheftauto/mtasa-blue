class CV8ExportEnumBase;
class CV8ExportObjectBase;

class CV8ModuleBase
{
public:
    virtual void AddFunction(std::string name, void (*args)(CV8FunctionCallbackBase*)) = 0;
    virtual void AddEnum(std::string name, CV8ExportEnumBase* pEnum) = 0;
    virtual void AddObject(std::string name, CV8ExportObjectBase* pObject) = 0;
};
