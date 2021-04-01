class CV8Module : public CV8ModuleBase
{
public:
    CV8Module(std::string name);
    ~CV8Module();
    void AddFunction(std::string name, void (*callback)(CV8FunctionCallbackBase*));
    void AddEnum(std::string name, CV8EnumBase* pEnum);
    void AddObject(std::string name, CV8ExportObjectBase* pObject);

    std::unordered_map<std::string, void (*)(CV8FunctionCallbackBase*)> GetFunctions() const;
    std::unordered_map<std::string, CV8Enum*>                           GetEnums() const;
    std::unordered_map<std::string, CV8ExportObject*>                   GetObjects() const;

    std::vector<Local<String>> GetExports(Isolate* pIsolate);

private:
    std::string                                                         m_strName;
    std::unordered_map<std::string, void (*)(CV8FunctionCallbackBase*)> m_mapFunctions;
    std::unordered_map<std::string, CV8Enum*>                           m_enums;
    std::unordered_map<std::string, CV8ExportObject*>                   m_objects;
};
