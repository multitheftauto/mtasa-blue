class CLuaManager;

class CModuleLoggerImpl : public ILogger
{
public:
    void Printf(const char* szFormat, ...);
    void ErrorPrintf(const char* szFormat, ...);
    void LogInformation(lua_State* luaVM, const char* szFormat, ...);
};

class CModuleInterfaceImpl : public IModuleInterface
{
public:
    CModuleInterfaceImpl(CLuaManager* pLuaManager, ILogger* pLoggerImpl);

    ILogger*      GetLogger();
    IResource*    GetResourceFromName(const char* name);
    const char*   GetServerPath();
    const char*   GetModulesPath();
    IResource*    GetResourceFromLuaState(lua_State* luaVM);
    unsigned long GetVersion();
    const char*   GetVersionString();
    const char*   GetVersionName();
    unsigned long GetNetcodeVersion();
    const char*   GetOperatingSystemName();
    void          AddFunction(lua_State* luaVM, const char* szName, lua_CFunction function);


private:
    CLuaManager* m_pLuaManager;
    ILogger*     m_pLogger;
};
