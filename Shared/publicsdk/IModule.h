#include "IModuleInterfaces.h"

class IModule
{
public:
    virtual void Load() = 0;
    virtual void UnLoad() = 0;
    virtual void DoPulse() = 0;
    virtual void ResourceStarted(IResource* resource) = 0;
    virtual void ResourceStopped(IResource* resource) = 0;
    virtual void ResourceStopping(IResource* resource) = 0;
};
