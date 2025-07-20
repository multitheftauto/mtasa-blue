/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CModManagerInterface.h
 *  PURPOSE:     Game mod manager interface
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#if (__cplusplus >= 201703L)
    #include <string_view>
#endif

class CClientBase;

class CModManagerInterface
{
public:
    virtual void RequestLoad(const char* arguments) = 0;
    virtual void RequestUnload() = 0;

    virtual bool         IsLoaded() const = 0;
    virtual CClientBase* GetClient() = 0;

    virtual bool TriggerCommand(const char* commandName, size_t commandNameLength, const void* userdata, size_t userdataSize) const = 0;

#if (__cplusplus >= 201703L)
    bool TriggerCommand(std::string_view command, const void* userdata, size_t userdataSize) const
    {
        return TriggerCommand(command.data(), command.length(), userdata, userdataSize);
    }

    template <typename T>
    bool TriggerCommand(std::string_view command, const T& userdata) const
    {
        return TriggerCommand(command, &userdata, sizeof(T));
    }
#endif
};
