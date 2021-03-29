class CV8Module : public CV8ModuleBase
{
public:
    CV8Module(std::string name);
    ~CV8Module();
    void                                                                AddFunction(std::string name, void (*callback)(CV8FunctionCallbackBase*));
    std::unordered_map<std::string, void (*)(CV8FunctionCallbackBase*)> GetFunctions() const;

    std::vector<Local<String>> GetExports(Isolate* pIsolate);
private:
    std::string                                                         m_strName;
    std::unordered_map<std::string, void (*)(CV8FunctionCallbackBase*)> m_mapFunctions;
};
