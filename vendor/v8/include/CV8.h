class CV8 : public CV8Base
{
    
public:
    CV8();
    ~CV8();
    CV8IsolateBase* CreateIsolate(std::string& strCode, std::string& originResource);

private:
    std::unique_ptr<v8::Platform> m_pPlatform;
};
