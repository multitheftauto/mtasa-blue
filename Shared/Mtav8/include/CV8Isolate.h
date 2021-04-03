using namespace v8;

class CV8Isolate : public CV8IsolateBase
{
public:

    CV8Isolate(CV8* pCV8, std::string originResource);
    ~CV8Isolate();

    void InitSecurity();
    void InitClasses();

    void DoPulse();
    void RequestGC();
    void Shutdown();

    bool RunCode(std::string& code, std::string originFileName);

    Isolate*           GetIsolate() const { return m_pIsolate; }

    bool GetErrorMessage(std::string& error);

    void Evaluate();

    void ReportMissingModule(std::string name);

    void        TerminateExecution();
    void        SetEvalEnabled(bool value);
    std::string GetModuleName(Local<Module> module);

    // May be used as a namespace for module imports. Equivalent of js "let object = {}"
    Local<Object> CreateGlobalObject(std::string mapName);
    // Equivalent of js "object[key] = value"
    void SetObjectKeyValue(Local<Object> object, std::string key, Local<Value> value);
    // Equivalent of js "let key = value"
    void SetKeyValue(std::string key, Local<Value> value);

    // Stopping initialization, prevents scripts from starting.
    template <typename... Args>
    void RaiseInitializationError(const std::string& format, Args&&... args)
    {
        printf(format.c_str(), std::forward<Args>(args)...);
        m_bHasInitializationError = true;
    }

    bool HasInitializationError() const { return m_bHasInitializationError; }

    Local<Function> CreateFunction(void (*callback)(CV8FunctionCallbackBase*));

private:
    // TryCatch can't be copied
    struct STryCatch
    {
        std::string location;
        std::string exception;
        int         line;
        int         column;
        STryCatch(std::string location, std::string&& exception, int line, int column)
            : location(location), exception(std::move(exception)), line(line), column(column)
        {
        }
    };

    void                   ReportException(TryCatch* pTryCatch);
    std::string            m_strOriginResource;
    std::string            m_strCurrentOriginFileName;
    Isolate::CreateParams  m_createParams;
    Isolate*               m_pIsolate;
    CV8*                   m_pCV8;
    Global<ObjectTemplate> m_global;

    std::map<uint16_t, Global<FunctionTemplate>> m_classInstances;

    Global<Context> m_rootContext;

    ResourceConstraints m_constraints;

    std::unordered_set<std::string> m_loadedModules;

    std::vector<std::unique_ptr<STryCatch>> m_vecCompilationErrors;
    int                                     m_iRunCodeCount = 0;

    bool m_bHasInitializationError = false;
};
