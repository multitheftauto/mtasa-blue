using namespace v8;

class CV8Promise;

class CV8Isolate : public CV8IsolateBase
{
public:
    CV8Isolate(const CV8* pCV8, std::string& originResource);
    ~CV8Isolate();
    void DoTaskPulse();
    void DoPulse();

    void                      RunCode(std::string& code, bool bAsModule = false);
    static MaybeLocal<Module> InstantiateModule(Local<Context> context, Local<String> specifier, Local<FixedArray> import_assertions,
                                                            Local<Module> referrer);

    void              EnqueueMicrotask(std::function<void(CV8Isolate*)> microtask);
    MaybeLocal<Value> InitializeModuleExports(Local<Context> context, Local<Module> module);
    void              AddPromise(std::unique_ptr<CV8Promise> pPromise);
    Isolate*          GetIsolate() const { return m_pIsolate; }

private:

    void                  ReportException(TryCatch* pTryCatch);
    std::string           m_strOriginResource;
    Isolate::CreateParams m_createParams;
    Isolate*              m_pIsolate;
    const CV8*            m_pCV8;

    //std::unique_ptr<class InspectorClientImpl>              m_pClient;
    //std::unique_ptr<class ChannelImpl>                      m_pChannel;
    //std::unique_ptr<class v8_inspector::V8InspectorSession> m_pSession;
    //std::unique_ptr<class v8_inspector::V8Inspector>        m_pInspector;
    std::vector<std::unique_ptr<JobHandle>>                 m_vecJobHandles;
    std::vector<std::unique_ptr<CV8Promise>>                m_vecPromises;
};