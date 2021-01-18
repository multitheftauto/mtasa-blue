class CV8Base
{
public:
    virtual CV8IsolateBase* CreateIsolate(std::string& strCode, std::string& originResource) = 0;
};
