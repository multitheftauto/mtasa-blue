using namespace v8;

class CV8Isolate : public CV8IsolateBase
{
public:
    CV8Isolate(const CV8* pCV8, std::string& originResource);
    ~CV8Isolate();

    void RunCode(std::string& code, bool bAsModule = false);
    static MaybeLocal<Value> InitializeModuleExports(Local<Context> context, Local<Module> module);

private:
    static MaybeLocal<Module> Resolve(Local<Context> context, Local<String> specifier, Local<Module> referrer);
    
    void                  ReportException(TryCatch* pTryCatch);
    std::string           m_strOriginResource;
    Isolate::CreateParams m_createParams;
    Isolate*              m_pIsolate;
    const CV8*            m_pCV8;
};