using namespace v8;

class CV8Isolate : public CV8IsolateBase
{
public:

    CV8Isolate(CV8* pCV8, std::string originResource);
    ~CV8Isolate();

    void InitSecurity();

    void DoPulse();
    void Shutdown();

    bool RunCode(std::string& code, std::string originFileName);

    Isolate*           GetIsolate() const { return m_pIsolate; }


private:
    std::string            m_strOriginResource;
    std::string            m_strCurrentOriginFileName;
    Isolate::CreateParams  m_createParams;
    Isolate*               m_pIsolate;
    CV8*                   m_pCV8;
    Global<ObjectTemplate> m_global;

    std::map<uint16_t, Global<FunctionTemplate>> m_classInstances;

    Global<Context> m_rootContext;

    std::unordered_set<std::string> m_loadedModules;
};
