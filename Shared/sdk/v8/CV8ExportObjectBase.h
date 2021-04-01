class CV8ExportObjectBase
{
public:
    virtual void AddFunction(std::string name, void (*callback)(CV8FunctionCallbackBase*)) = 0;
};
