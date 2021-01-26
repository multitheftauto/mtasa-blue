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
    void                      TestMess(std::string mess);
    static MaybeLocal<Module> CV8Isolate::InstantiateModule(Local<Context> context, Local<String> specifier, Local<FixedArray> import_assertions,
                                                            Local<Module> referrer);

    MaybeLocal<Value> InitializeModuleExports(Local<Context> context, Local<Module> module);
    void              AddPromise(CV8Promise* pPromise) { m_vecPromises.push_back(pPromise); }

private:

    void                  ReportException(TryCatch* pTryCatch);
    std::string           m_strOriginResource;
    Isolate::CreateParams m_createParams;
    Isolate*              m_pIsolate;
    const CV8*            m_pCV8;

    std::unique_ptr<class InspectorClientImpl>              m_pClient;
    std::unique_ptr<class ChannelImpl>                      m_pChannel;
    std::unique_ptr<class v8_inspector::V8InspectorSession> m_pSession;
    std::unique_ptr<class v8_inspector::V8Inspector>        m_pInspector;
    std::vector<CV8Promise*>                                m_vecPromises;
};