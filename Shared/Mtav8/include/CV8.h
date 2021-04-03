using namespace v8;

class CV8Isolate;

class CV8 : public CV8Base
{
public:
    CV8();
    ~CV8();
    void Initialize();
    void Shutdown();

    CV8IsolateBase* CreateIsolate(std::string originResource);
    void            RemoveIsolate(CV8IsolateBase* pIsolate);

    void DoPulse();

    std::vector<CV8IsolateBase*> GetIsolates();
    Platform*                    GetPlatform() const { return m_pPlatform.get(); }

private:
    std::unique_ptr<Platform>                m_pPlatform;
    std::vector<std::unique_ptr<CV8Isolate>> m_vecIsolates;

    bool m_bDisposing = false;
};
