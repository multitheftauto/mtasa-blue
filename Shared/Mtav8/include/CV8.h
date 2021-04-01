using namespace v8;

class CV8Module;
class CV8Enum;
class CV8Isolate;
class CV8Class;
class CV8ExportObject;

class CV8 : public CV8Base
{
public:
    enum EInternalFieldPurpose
    {
        TypeOfClass,               // Value is one of value from EClass
        PointerToValue,            // A pointer to external pointer.
        Count,
    };

    CV8();
    ~CV8();
    void Initialize(int iThreadPool);
    void Shutdown();

    void SetExecutionTimeLimit(int iTime) { m_iTime = iTime; }
    void EnterExecution(CV8Isolate* pIsolate);
    void ExitExecution(CV8Isolate* pIsolate);

    CV8IsolateBase* CreateIsolate(std::string originResource);
    void            RemoveIsolate(CV8IsolateBase* pIsolate);

    CV8ModuleBase* CreateModule(std::string name);
    CV8EnumBase*   CreateEnum();

    CV8ClassBase*        CreateClass(std::string name, size_t classId);
    CV8ExportObjectBase* CreateExportObject();

    void DoPulse();

    static Local<Module>         GetDummyModule(Isolate* pIsolate);
    static void                  RegisterAllModules(CV8Isolate* pIsolate);
    static void                  RegisterAllModulesInGlobalNamespace(CV8Isolate* pIsolate);
    CV8Module*                   GetModuleByName(std::string name);
    std::vector<CV8IsolateBase*> GetIsolates();
    std::vector<CV8Class*>       GetClasses();
    Platform*                    GetPlatform() const { return m_pPlatform.get(); }

    static std::unordered_map<std::string, std::unique_ptr<CV8Module>> m_mapModules;

private:
    std::unique_ptr<Platform>                     m_pPlatform;
    std::vector<std::unique_ptr<CV8Isolate>>      m_vecIsolates;
    std::vector<std::unique_ptr<CV8Class>>        m_vecClasses;
    std::vector<std::unique_ptr<CV8Enum>>         m_vecEnums;
    std::vector<std::unique_ptr<CV8ExportObject>> m_vecObjects;

    std::thread m_longExecutionGuardThread;
    std::mutex  m_lock;
    bool        m_bDisposing = false;

    std::mutex  m_executionGuard;
    CV8Isolate* m_pCurrentExecutionIsolate = nullptr;
    int         m_pIsolateExecutionTicks = 0;
    int         m_iTime = 2000;
};
