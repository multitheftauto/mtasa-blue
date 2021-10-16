class CLuaManager;

class CModuleLoggerImpl : ILogger
{
    void Printf(const char* szFormat, ...);
    void ErrorPrintf(const char* szFormat, ...);
    void LogInformation(lua_State* luaVM, const char* szFormat, ...);
};

class CModuleInterfaceImpl : public IModuleInterface
{
public:
    CModuleInterfaceImpl(CLuaManager* pLuaManager);

    ILogger*      GetLogger();
    IResource*    GetResourceFromName(const char* name);
    const char*   GetServerPath();
    IResource*    GetResourceFromLuaState(lua_State* luaVM);
    unsigned long GetVersion();
    const char*   GetVersionString();
    const char*   GetVersionName();
    unsigned long GetNetcodeVersion();
    const char*   GetOperatingSystemName();
    void          AddFunction(lua_State* luaVM, const char* szName, lua_CFunction function);


private:
    CLuaManager* m_pLuaManager;
};
