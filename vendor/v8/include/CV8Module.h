class CV8Module : public CV8ModuleBase
{
public:
    CV8Module(const char* name);
    ~CV8Module();
    void AddFunction(const char* name, void (*callback)(CV8FunctionCallbackBase*)) { m_mapFunctions[name] = callback; }
    std::unordered_map<const char*, void (*)(CV8FunctionCallbackBase*)> GetFunctions() const { return m_mapFunctions; }

    std::vector<Local<String>> GetExports(Isolate* pIsolate);
private:
    const char* m_strName;
    std::unordered_map<const char*, void (*)(CV8FunctionCallbackBase*)> m_mapFunctions;
};