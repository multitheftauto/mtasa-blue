using namespace v8;

class CV8Isolate : public CV8IsolateBase
{
public:
    CV8Isolate(std::string& originResource);
    ~CV8Isolate();

    void RunCode(std::string& code);

private:
    void                  ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch);
    std::string           m_strOriginResource;
    Isolate::CreateParams m_createParams;
    Isolate* m_pIsolate;
};