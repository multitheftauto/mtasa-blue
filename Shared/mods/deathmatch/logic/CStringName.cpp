/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CStringName.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"
#include "CStringName.h"

#include <array>
#include <string_view>

extern "C"
{
    #include "lua/src/lstring.h"
}

static StringNameHash MakeStringNameHash(const std::string_view& str)
{
    return luaS_hash(str.data(), str.length());
}

/*
    CStringNameStorage
*/
class CStringNameStorage
{
public:
    CStringNameData* Get(const std::string_view& str)
    {
        if (str.empty())
        {
            ZERO_NAME_DATA.AddRef();
            return &ZERO_NAME_DATA;
        }

        const StringNameHash hash = MakeStringNameHash(str);
        const uint32_t idx = hash & CStringName::STRING_TABLE_MASK;
        CIntrusiveDoubleLinkedList<CStringNameData>& list = m_table[idx];

        CStringNameData* data = list.First();
        while (data)
        {
            if (data->m_hash == hash && data->m_name == str)
                break;

            data = list.Next(data);
        }

        if (!data || data->m_refs == 0)
        {
            data = new CStringNameData(str, hash);
            list.InsertFront(data);
        }

        data->AddRef();

        return data;
    } 

    void Release(CStringNameData* data)
    {
        const uint32_t idx = data->m_hash & CStringName::STRING_TABLE_MASK;

        if (data->m_refs == 0)
            m_table[idx].Erase(data);
    }

    CStringNameData* Find(const std::string_view& str, StringNameHash hash)
    {
        const uint32_t idx = hash & CStringName::STRING_TABLE_MASK;
        CIntrusiveDoubleLinkedList<CStringNameData>& list = m_table[idx];

        for (CStringNameData& data : list)
        {
            if (data.m_hash == hash && data.m_name == str)
                return &data;
        }

        return nullptr;
    }

    static CStringNameStorage& Instance()
    {
        static CStringNameStorage storage{};
        return storage;
    }

    static CStringNameData ZERO_NAME_DATA;

private:
    std::array<CIntrusiveDoubleLinkedList<CStringNameData>, CStringName::STRING_TABLE_LEN> m_table;
};

CStringNameData CStringNameStorage::ZERO_NAME_DATA{ {}, 0u, 1 };

/*
    CStringNameData
*/
void CStringNameData::AddRef()
{
    ++m_refs;
}

void CStringNameData::RemoveRef()
{
    if (m_hash == 0u)
        return;

    if (--m_refs == 0)
        CStringNameStorage::Instance().Release(this);
}

/*
    CStringName
*/
const CStringName CStringName::ZERO{};

CStringName::CStringName() :
    m_data(CStringNameStorage::Instance().Get({}))
{
}

CStringName::CStringName(const char* str) :
    m_data(CStringNameStorage::Instance().Get(str))
{
}

CStringName::CStringName(const std::string& str) :
    m_data(CStringNameStorage::Instance().Get(str))
{
}

CStringName::CStringName(const std::string_view& str) :
     m_data(CStringNameStorage::Instance().Get(str))
{
}

CStringName::CStringName(const CStringName& name) :
    m_data(name.m_data)
{
    if (m_data)
        m_data->AddRef();
}

CStringName::CStringName(CStringNameData* data) :
    m_data(data)
{
    if (m_data)
        m_data->AddRef();
}

CStringName& CStringName::operator=(const CStringName& name)
{
    if (m_data == name.m_data)
        return *this;

    m_data->RemoveRef();
    m_data = name.m_data;
    m_data->AddRef();

    return *this;
}

CStringName& CStringName::operator=(const std::string& str)
{
    *this = CStringName(str);
    return *this;
}

CStringName& CStringName::operator=(const std::string_view& str)
{
    *this = CStringName(str);
    return *this;
}

CStringName::~CStringName()
{
    if (m_data)
        m_data->RemoveRef();
}

void CStringName::Clear()
{
    *this = CStringName::ZERO;
}

CStringName CStringName::FromStringAndHash(const std::string_view& str, StringNameHash hash)
{
    if (CStringNameData* data = CStringNameStorage::Instance().Find(str, hash))
        return CStringName{data};

    // Create a new name
    return CStringName{str};
}
