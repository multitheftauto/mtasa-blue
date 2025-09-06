/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CCStringName.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <string>
#include <string_view>

#include "CIntrusiveDoubleLinkedList.h"

// unsigned here is the intentional choice because it is a type of hash that Lua basically uses
using StringNameHash = unsigned;

class CStringNameData : public CIntrusiveDoubleLinkedListNode
{
public:
    CStringNameData() = default;

    CStringNameData(const std::string_view& name, StringNameHash hash, size_t refs = 0) :
        m_name(name),
        m_hash(hash),
        m_refs(refs)
    {
    }

    void AddRef();

    void RemoveRef();

    std::string m_name;

    StringNameHash m_hash{};

    std::size_t m_refs{0u};
};

class CStringName
{
    friend class CStringNameStorage;

    enum {
		STRING_TABLE_BITS = 16,
		STRING_TABLE_LEN = 1 << STRING_TABLE_BITS,
		STRING_TABLE_MASK = STRING_TABLE_LEN - 1
	};

public:
    // Default constructor
    CStringName();

    // Construct from a C string.
    CStringName(const char* str);

    // Construct from a string.
    explicit CStringName(const std::string& str);

    // Construct from a string.
    explicit CStringName(const std::string_view& str);

    // Copy-construct from another string name.
    CStringName(const CStringName& name);

    // Copy-assignment from another string name.
    CStringName& operator =(const CStringName& name);

    // Copy-assignment from string.
    CStringName& operator =(const std::string& str);

    // Copy-assignment from string view.
    CStringName& operator =(const std::string_view& str);

    // Destruct
    ~CStringName();

    // Test for equality with another string name.
    bool operator ==(const CStringName& rhs) const { return m_data == rhs.m_data; }

    // Test for inequality with another string name.
    bool operator !=(const CStringName& rhs) const { return m_data != rhs.m_data; }

    // Test if less than another string name.
    bool operator <(const CStringName& rhs) const { return m_data->m_hash < rhs.m_data->m_hash; }

    // Test if greater than another string name.
    bool operator >(const CStringName& rhs) const { return m_data->m_hash > rhs.m_data->m_hash; }

    const std::string* operator ->() const { return &m_data->m_name; }

    const std::string& operator *() const { return m_data->m_name; }

    // Return true if nonzero string name value.
    explicit operator bool() const { return m_data != nullptr; }

    // Return as string.
    const std::string& ToString() const { return m_data->m_name; }

    // Return as C-string
    const char* ToCString() const { return m_data != nullptr ? m_data->m_name.c_str() : nullptr; }

    StringNameHash Hash() const noexcept { return m_data->m_hash; }

    bool Empty() const { return m_data->m_hash == 0u; }

    void Clear();

    // Return name data
    const CStringNameData* GetNameData() const { return m_data; }

    // Zero hash.
    static const CStringName ZERO;

    static CStringName FromStringAndHash(const std::string_view& str, StringNameHash hash);

private:
    explicit CStringName(CStringNameData* data);

private:
    // String name data
    CStringNameData* m_data{};    
};

template <>
struct std::hash<CStringName>
{
    std::size_t operator()(const CStringName& k) const
    {
        return static_cast<std::size_t>(k.Hash());
    }
};
