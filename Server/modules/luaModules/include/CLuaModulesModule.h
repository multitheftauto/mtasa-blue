#include "interfaces/ILuaModule.h"

class CLuaModuleManager;

class CLuaModulesModule : public IModule
{
public:
    CLuaModulesModule(IModuleInterface* pModuleInterface) : m_pModuleInterface(pModuleInterface) {}

    void                      Load();
    void                      UnLoad();
    void                      DoPulse();
    void                      ResourceStarted(IResource* pResource);
    void                      ResourceStopped(IResource* pResource);
    void                      ResourceStopping(IResource* pResource);
    int                       LoadModule(const char* szShortFileName, const char* szFileName, bool bLateLoad);
    int                       UnloadModule(const char* szShortFileName);
    int                       ReloadModule(const char* szShortFileName, const char* szFileName, bool bLateLoad);
    ILuaModule*               GetModuleByName(const char* szName);
    std::vector<ILuaModule*>  GetModules();
    std::vector<SFoundModule> GetAllModules();
    bool                      IsModuleRunning(std::string moduleName);

private:
    IModuleInterface*  m_pModuleInterface;
    CLuaModuleManager* m_pLuaModuleManager;
};
