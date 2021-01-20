using namespace v8;

class CV8Isolate : public CV8IsolateBase
{
public:
    CV8Isolate(std::string& originResource);
    ~CV8Isolate();

    void RunCode(std::string& code, bool bAsModule = false);

private:
    void                  ReportException(TryCatch* pTryCatch);
    std::string           m_strOriginResource;
    Isolate::CreateParams m_createParams;
    Isolate*              m_pIsolate;
    Local<Module>         m_mtaModule;
};