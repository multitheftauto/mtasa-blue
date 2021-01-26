class CV8Module;

class CV8 : public CV8Base
{
    
public:
    CV8();
    ~CV8();
    CV8IsolateBase* CreateIsolate(std::string& strCode, std::string& originResource);
    void            RemoveIsolate(CV8IsolateBase* pIsolate);

    CV8ModuleBase*  CreateModule(const char* name);
    void            DoPulse();
    void            DoTaskPulse();

    static CV8Module*  GetModuleByName(const char* name);
    static std::unordered_map<std::string, std::unique_ptr<CV8Module>> m_mapModules;
    std::vector<CV8IsolateBase*>                                       GetIsolates();
private:
    std::unique_ptr<v8::Platform> m_pPlatform;
    std::vector<std::unique_ptr<class CV8Isolate>> m_vecIsolates;
};
