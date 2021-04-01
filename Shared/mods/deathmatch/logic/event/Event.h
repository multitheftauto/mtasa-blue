#pragma once

#include <string_view>
#include <string>

class CLuaMain;

// Virtual base class
struct Event
{
    Event(std::string name) :
        m_name(std::move(name))
    {
    }

    virtual ~Event() = default;

    virtual bool IsBuiltIn() const = 0;

    const std::string& GetName() const { return m_name; }
    std::string m_name;
};

struct BuiltInEvent final : Event
{
    enum class ID
    {
        ON_ELEMENT_DATA_CHANGE,
        
        // ACHTUNG: Add new events above this
        COUNT
    };

    BuiltInEvent(std::string name, ID id) :
        Event(std::move(name)),
        m_id(id)
    {
    }

    virtual bool IsBuiltIn() const override { return true; }

    ID GetID() const { return m_id; }
protected:
    ID m_id;
};

struct CustomEvent final : Event
{
    CustomEvent(std::string name, CLuaMain* createdBy, bool remoteTrigger) :
        Event(std::move(name)),
        m_createdBy(createdBy),
        m_allowRemoteTrigger(remoteTrigger)
    {
    }

    virtual bool IsBuiltIn() const override { return false; }

    auto GetCreatedBy() const { return m_createdBy; }
    auto IsRemoteTriggerAllowed() const { return m_allowRemoteTrigger; }

    CLuaMain* m_createdBy; // Where the event was registered in
    bool m_allowRemoteTrigger;
};
