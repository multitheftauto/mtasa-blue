#pragma once

#include <string_view>
#include <string>
#include <variant>

#include "BuiltInEventsListMacroApply.h"

class CLuaMain;

// Virtual base class
class Event
{
public:
    virtual ~Event() = default;

    virtual bool IsBuiltIn() const = 0;

    const std::string& GetName() const { return m_name; }

    static const Event* Get(const std::string& name);
protected:
    Event(std::string name) :
        m_name(std::move(name))
    {
    }
private:
    std::string m_name;
};

// Built-in-events defined in BuiltInEvents.h
class BuiltInEvent final : public Event
{
    friend class BuiltInEvents;
public:

    enum class ID
    {
        #define ENUM_DEFINE(name, enumName) enumName,
        BUILTINEVENT_LIST_APPLY_MACRO(ENUM_DEFINE)
        #undef ENUM_DECL
        #undef BUILTINEVENT_ID_EXPAND
        #undef BUILTINEVENT_LIST_APPLY_MACRO

        COUNT
    };

    virtual bool IsBuiltIn() const override { return true; }
    ID GetID() const { return m_id; }

    // Wrappers around BuiltInEvents::Get
    static const BuiltInEvent* Get(const std::string& name);
    static const BuiltInEvent* Get(BuiltInEvent::ID id); // Intended for API uses only. To access a builtin event, just use BuiltInEvents
protected:
    // Only BuiltInEvents is allowed to construct
    BuiltInEvent(std::string name, ID id) :
        Event(std::move(name)),
        m_id(id)
    {
    }
private:
    ID m_id;
};

class CustomEvent final : public Event
{
    friend class CustomEvents; // Allow usage of constructor
public:
    virtual bool IsBuiltIn() const override { return false; }

    auto GetCreatedBy() const { return m_createdBy; }
    auto IsRemoteTriggerAllowed() const { return m_allowRemoteTrigger; }

    // Wrappers around CustomEvents::Add and Get
    static const CustomEvent* Get(const std::string& name);
    static bool Add(std::string name, CLuaMain* lmain, bool allowRemoteTrigger);
protected:
    CustomEvent(std::string name, CLuaMain* createdBy, bool remoteTrigger) :
        Event(std::move(name)),
        m_createdBy(createdBy),
        m_allowRemoteTrigger(remoteTrigger)
    {
    }
private:
    CLuaMain* m_createdBy; // Where the event was registered in
    bool m_allowRemoteTrigger;
};


