#pragma once

// Don't use directly, use `CJsClassConstructionInfo` in scope variant.
class CV8ExportClassBase
{
public:
    virtual void SetSizeOf(size_t count) = 0;
    virtual void SetConstructorFunction(std::function<void(CV8FunctionCallbackBase*)> func) = 0;
    virtual void AddAccessor(std::string name, float (*getter)(void*), void (*setter)(void*, float)) = 0;
    virtual void SetInheritance(uint16_t baseClass) = 0;
};
