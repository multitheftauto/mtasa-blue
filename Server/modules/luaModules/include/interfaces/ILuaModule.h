#pragma once

struct SFoundModule
{
    std::string name;
    std::string state;
};

class ILuaModule
{
public:
    virtual const char* GetName() const = 0;
    virtual const char* GetAuthor() const = 0;
    virtual float       GetVersion() const = 0;
    virtual const char* GetFileName() const = 0;
};
