using namespace v8;

class CV8Isolate : public CV8IsolateBase
{
public:
    CV8Isolate();
    ~CV8Isolate();

    void RunCode(std::string& code);

private:
    Isolate::CreateParams m_createParams;
    Isolate* m_pIsolate;
};