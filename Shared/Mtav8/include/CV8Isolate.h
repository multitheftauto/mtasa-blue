using namespace v8;

class CV8Promise;

class CV8GC : public PersistentHandleVisitor
{
public:
    Isolate* m_pIsolate;

    CV8GC(Isolate* isolate) : m_pIsolate(isolate) {}
    ~CV8GC() {}

    void VisitPersistentHandle(Persistent<Value>* persistent, uint16_t usClassId);
};

// Remember to implement all classes in VisitPersistentHandle and bump number.
static_assert((int)CV8BaseClass::EClass::Count == 5 && "Missing implementation for GC");

class CV8Isolate : public CV8IsolateBase
{
public:
    CV8Isolate(CV8* pCV8, std::string& originResource);
    ~CV8Isolate();

    void InitSecurity();
    void InitClasses();

    void DoPulse();

    void               RunCode(std::string& code, std::string& originFileName);
    MaybeLocal<Module> InstantiateModule(Local<Context> context, Local<String> specifier, Local<FixedArray> import_assertions, Local<Module> referrer);

    void               EnqueueMicrotask(std::function<void(CV8Isolate*)> microtask);
    MaybeLocal<Value>  InitializeModuleExports(Local<Context> context, Local<Module> module);
    MaybeLocal<Module> GetScriptModule(const char* name);
    void               AddPromise(std::unique_ptr<CV8Promise> pPromise);
    Isolate*           GetIsolate() const { return m_pIsolate; }

    bool GetErrorMessage(std::string& error);
    bool GetMissingModulesErrorMessage(std::string& error);

    void InitializeModules();
    void Evaluate();

    void ReportMissingModule(std::string name);

    void TerminateExecution();
    void SetJsEvalSetting(eJsEval value);
    std::string GetModuleName(Local<Module> module);

    // May be used as a namespace for module imports. Equivalent of js "let object = {}"
    Local<Object> CreateGlobalObject(const char* mapName);
    // Equivalent of js "object[key] = value"
    void SetObjectKeyValue(Local<Object> object, const char* key, Local<Value> value);
    // Equivalent of js "let key = value"
    void SetKeyValue(const char* key, Local<Value> value);

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
    // Perform common execution checks, long execution protection.
    // Use before each time js starts to execute
    class Execution
    {
    public:
        Execution(CV8Isolate* pIsolate) : m_pIsolate(pIsolate) { pIsolate->m_pCV8->EnterExecution(pIsolate); }
        ~Execution() { m_pIsolate->m_pCV8->ExitExecution(m_pIsolate); }

    private:
        CV8Isolate* m_pIsolate;
    };

    struct SModule
    {
        Global<Module> m_module;
    };

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

    class CMicrotask
    {
    public:
        CMicrotask(std::function<void(CV8Isolate*)> microtask, CV8Isolate* isolate) : m_microtask(microtask), m_isolate(isolate) {}
        std::function<void(CV8Isolate*)> m_microtask;
        CV8Isolate*                      m_isolate;
    };
    void                   ReportException(TryCatch* pTryCatch);
    std::string            m_strOriginResource;
    std::string            m_strCurrentOriginFileName;
    Isolate::CreateParams  m_createParams;
    Isolate*               m_pIsolate;
    CV8*                   m_pCV8;
    Global<ObjectTemplate> m_global;
    Global<Context>        m_context;
    ResourceConstraints    m_constraints;

    std::unordered_set<std::string> m_loadedModules;

    // Module from index X can only import modules at index < X. eg index 3 can import index 0,1 or 2 only.
    // It prevents from error: ReferenceError: Cannot access 'ImportName' before initialization
    std::vector<std::string>                                  m_loadingOrder;
    std::unordered_map<std::string, Global<Module>>           m_mapScriptModules;
    std::vector<std::unique_ptr<CV8Promise>>                  m_vecPromises;
    std::vector<std::unique_ptr<SModule>>                     m_vecModules;
    std::vector<std::unique_ptr<STryCatch>>                   m_vecCompilationErrors;
    std::unordered_map<std::string, std::vector<std::string>> m_mapMissingModules;
    int                                                       m_iRunCodeCount = 0;

    std::queue<std::string> modulesListName;
    eJsEval                 m_eJsEval;
    bool                    m_bHasInitializationError = false;

    std::unique_ptr<CV8GC> m_pGC;

    static ModifyCodeGenerationFromStringsResult Eval(Local<Context> context, Local<Value> source, bool is_code_like);
};
