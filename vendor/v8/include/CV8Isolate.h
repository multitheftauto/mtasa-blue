using namespace v8;

class CV8Promise;

class CV8Isolate : public CV8IsolateBase
{
public:
    CV8Isolate(const CV8* pCV8, std::string& originResource);
    ~CV8Isolate();
    void DoTaskPulse();
    void DoPulse();

    void                      RunCode(std::string& code, std::string& originFileName);
    static MaybeLocal<Module> InstantiateModule(Local<Context> context, Local<String> specifier, Local<FixedArray> import_assertions, Local<Module> referrer);

    void              EnqueueMicrotask(std::function<void(CV8Isolate*)> microtask);
    MaybeLocal<Value> InitializeModuleExports(Local<Context> context, Local<Module> module);
    void              AddPromise(std::unique_ptr<CV8Promise> pPromise);
    Isolate*          GetIsolate() const { return m_pIsolate; }

    bool GetErrorMessage(std::string& error);
    void Evaluate();

private:
    struct SModule
    {
        Global<Module> m_module;
    };

    // v8::TryCatch can't be copied
    struct STryCatch
    {
        std::string location;
        std::string exception;
        int         line;
        int         column;
        STryCatch(std::string location, std::string&& exception, int line, int column) : location(location), exception(std::move(exception)), line(line), column(column) {}
    };

    void                   ReportException(TryCatch* pTryCatch);
    std::string            m_strOriginResource;
    Isolate::CreateParams  m_createParams;
    Isolate*               m_pIsolate;
    const CV8*             m_pCV8;
    Global<ObjectTemplate> m_global;
    Global<Context>        m_context;

    std::vector<std::unique_ptr<CV8Promise>> m_vecPromises;
    std::vector<std::unique_ptr<SModule>>    m_vecModules;
    std::vector<std::unique_ptr<STryCatch>>  m_vecCompilationErrors;
    int                                      m_iRunCodeCount = 0;
};