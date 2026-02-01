/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPoolsSA.h
 *  PURPOSE:     Header file for pools interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPools.h>

// size of tPoolObjectFlags is 1 byte only
union tPoolObjectFlags
{
    struct
    {
        unsigned char nId : 7;
        bool          bEmpty : 1;
    };

private:
    unsigned char nValue;
};

template <class A, class B = A>
class CPoolSAInterface
{
public:
    // m_pObjects contains all interfaces. 140 maximum for ped objects.
    B*                m_pObjects;
    tPoolObjectFlags* m_byteMap;
    int               m_nSize;
    int               m_nFirstFree;
    bool              m_bOwnsAllocations;
    bool              field_11;

    // Default constructor for statically allocated pools
    CPoolSAInterface()
    {
        // Remember to call CPool::Init to fill in the fields!
        m_pObjects = nullptr;
        m_byteMap = nullptr;
        m_nSize = 0;
        m_bOwnsAllocations = false;
    }

    uint GetFreeSlot()
    {
        if (m_nSize <= 0 || m_byteMap == nullptr)
            return static_cast<uint>(-1);

        bool bLooped = false;
        int  index = m_nFirstFree + 1;

        if (index < 0 || index >= m_nSize)
            index = 0;

        while (true)
        {
            if (index >= m_nSize)
            {
                if (bLooped)
                    return static_cast<uint>(-1);

                index = 0;
                bLooped = true;
            }

            if (m_byteMap[index].bEmpty)
            {
                m_nFirstFree = index;
                return static_cast<uint>(index);
            }
            index++;
        }

        return static_cast<uint>(-1);
    };

    B* Allocate()
    {
        if (m_nSize <= 0 || m_pObjects == nullptr || m_byteMap == nullptr)
            return nullptr;

        m_nFirstFree++;           // Continue after the last allocated slot
        const auto sz = m_nSize;  // Storing size to avoid reloads from memory - should help out the optimizer

        if (m_nFirstFree < 0 || m_nFirstFree >= sz)
            m_nFirstFree = 0;

        for (int i = 0; i < sz; i++)
        {
            int slot = m_nFirstFree + i;
            if (slot >= sz)
                slot -= sz;
            const auto e = &m_byteMap[slot];
            if (!e->bEmpty)
            {
                continue;
            }
            m_nFirstFree = slot;
            e->bEmpty = false;
            e->nId++;
            return &m_pObjects[slot];
        }
        return nullptr;
    }

    B* AllocateAt(uint uiSlot)
    {
        m_pObjects[uiSlot] = B();
        m_byteMap[uiSlot].bEmpty = false;
        m_byteMap[uiSlot].nId ^= uiSlot ^ (uiSlot + 1);

        return &m_pObjects[uiSlot];
    }

    B* AllocateAtNoInit(std::uint32_t uiSlot)
    {
        m_byteMap[uiSlot].bEmpty = false;
        m_byteMap[uiSlot].nId ^= uiSlot ^ (uiSlot + 1);

        return &m_pObjects[uiSlot];
    }

    void Release(uint index)
    {
        m_byteMap[index].bEmpty = true;
        m_byteMap[index].nId = 0;
        if (index == m_nFirstFree)
            --m_nFirstFree;
    }

    void Delete(uint index) { Release(index); }

    std::int32_t Size() const noexcept { return m_nSize; };
    bool         IsEmpty(std::int32_t objectIndex) const { return m_byteMap[objectIndex].bEmpty; }
    bool         IsContains(std::int32_t index) const
    {
        if (m_nSize <= 0)
            return false;

        if (index < 0 || index >= m_nSize)
            return false;
        return !IsEmpty(index);
    }

    B* GetObject(std::int32_t objectIndex) { return &m_pObjects[objectIndex]; }

    std::int32_t GetObjectIndex(B* pObject) { return ((DWORD)pObject - (DWORD)m_pObjects) / sizeof(B); }

    std::int32_t GetObjectIndexSafe(B* pObject)
    {
        uint32_t index = GetObjectIndex(pObject);
        if (m_nSize <= 0)
            return UINT_MAX;

        return index >= static_cast<uint32_t>(m_nSize) ? UINT_MAX : index;
    }
};

// Generic container for pools
template <class T, class I, unsigned long MAX>
struct SPoolData
{
    std::array<SClientEntity<T>, MAX> arrayOfClientEntities;
    unsigned long                     ulCount;

public:
    SPoolData() : ulCount(0UL)
    {
        for (unsigned int i = 0; i < MAX; ++i)
        {
            arrayOfClientEntities[i] = {nullptr, nullptr};
        }
    }
};

template <class T>
struct SVectorPoolData
{
    std::vector<SClientEntity<T>> entities;
    size_t                        count;

public:
    SVectorPoolData(size_t defaultSize) : count(0) { entities.resize(defaultSize, {nullptr, nullptr}); }
};
