/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPoolsSA.h
 *  PURPOSE:     Header file for pools interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
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
        bool bLooped = false;
        uint index = m_nFirstFree + 1;

        while (true)
        {
            if (index >= m_nSize)
            {
                if (bLooped)
                    return -1;

                index = 0;
                bLooped = true;
            }

            if (m_byteMap[index].bEmpty)
            {
                m_nFirstFree = index;
                return index;
            }
            index++;
        }

        return -1;
    };

    B* Allocate()
    {
        m_nFirstFree++;                     // Continue after the last allocated slot
        const auto sz = m_nSize;            // Storing size to avoid reloads from memory - should help out the optimizer
        for (auto i = 0u; i < sz; i++)
        {
            const auto slot = (m_nFirstFree + i) % sz;
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

    void Release(uint index)
    {
        m_byteMap[index].bEmpty = true;
        m_byteMap[index].nId = 0;
        if (index == m_nFirstFree)
            --m_nFirstFree;
    }

    void Delete(uint index) { Release(index); }

    std::int32_t Size() const noexcept { return m_nSize; };
    bool IsEmpty(std::int32_t objectIndex) const { return m_byteMap[objectIndex].bEmpty; }
    bool IsContains(uint index) const
    {
        if (m_nSize <= index)
            return false;
        return !IsEmpty(index);
    }

    B* GetObject(std::int32_t objectIndex) { return &m_pObjects[objectIndex]; }

    uint GetObjectIndex(B* pObject) { return ((DWORD)pObject - (DWORD)m_pObjects) / sizeof(B); }

    uint32_t GetObjectIndexSafe(B* pObject)
    {
        uint32_t index = GetObjectIndex(pObject);
        return index > m_nSize ? UINT_MAX : index;
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
    SVectorPoolData(size_t defaultSize) : count(0)
    {
        entities.resize(defaultSize, {nullptr, nullptr});
    }
};
