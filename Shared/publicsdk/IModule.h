class lua_State;

class ILogger
{
public:
    virtual void Printf(const char* szFormat, ...) = 0;
    virtual void ErrorPrintf(const char* szFormat, ...) = 0;
    virtual void LogInformation(lua_State* luaVM, const char* szFormat, ...) = 0;
};

class IElement
{
public:
};

class IResource : public IElement
{
public:
    virtual lua_State*         GetLuaState() = 0;
    virtual const CChecksum&   GetLastMetaChecksum() = 0;
    virtual CChecksum          GetFileChecksum(const char* szFile) = 0;
    virtual const std::string& GetName() const = 0;
    virtual bool               GetFilePath(const char* szFilename, std::string& strPath) = 0;
};

class IModuleInterface
{
public:
    virtual ILogger*    GetLogger() = 0;
    virtual IResource*  GetResourceFromName(const char* name) = 0;
    virtual const char* GetServerPath() = 0;
    virtual IResource*  GetResourceFromLuaState(lua_State* luaVM) = 0;

    virtual unsigned long GetVersion() = 0;
    virtual const char*   GetVersionString() = 0;
    virtual const char*   GetVersionName() = 0;
    virtual unsigned long GetNetcodeVersion() = 0;
    virtual const char*   GetOperatingSystemName() = 0;
    virtual void          AddFunction(lua_State* luaVM, const char* szName, lua_CFunction function) = 0;
};

class IModule
{
public:
    virtual void Load(IModuleInterface* moduleInterface) = 0;
    virtual void UnLoad() = 0;
    virtual void DoPulse() = 0;
    virtual void ResourceStarted(IResource* resource) = 0;
    virtual void ResourceStopped(IResource* resource) = 0;
    virtual void ResourceStopping(IResource* resource) = 0;
};
