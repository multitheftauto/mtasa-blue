class CV8ExportObject : public CV8ExportObjectBase
{
public:
    CV8ExportObject();
    void AddFunction(std::string name, void (*callback)(CV8FunctionCallbackBase*));
    Local<Object> Initialize(CV8Isolate* pV8Isolate, std::string name);
    Local<Function> CreateFunction(Isolate* pIsolate, void (*callback)(CV8FunctionCallbackBase*));

    std::unordered_map<std::string, void (*)(CV8FunctionCallbackBase*)> GetFunctions() const;

private:
    std::unordered_map<std::string, void (*)(CV8FunctionCallbackBase*)> m_mapFunctions;
};
