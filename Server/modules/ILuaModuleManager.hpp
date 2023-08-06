#pragma once

#include <list>

#include "ILuaModule.hpp"

class ILuaModuleManager
{
public:
    virtual std::list<ILuaModule*>::iterator       begin() noexcept = 0;
    virtual std::list<ILuaModule*>::iterator       end() noexcept = 0;
    virtual std::list<ILuaModule*>::const_iterator cbegin() const noexcept = 0;
    virtual std::list<ILuaModule*>::const_iterator cend() const noexcept = 0;

    virtual std::list<ILuaModule*> GetLoadedModules() const noexcept = 0;
};
