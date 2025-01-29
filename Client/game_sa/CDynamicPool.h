/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CDynamicPool.h
 *  PURPOSE:     Custom implementation for SA pools
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <vector>
#include <array>
#include <memory>

template <typename PoolObjT>
class CDynamicPoolPart
{
public:
    explicit CDynamicPoolPart(std::size_t size) : m_size{size}, m_usedSize{0}, m_lastFreeSlot{0}
    {
        m_items = std::make_unique<PoolObjT[]>(size);
        m_usedSlots = std::make_unique<bool[]>(size);
    }

    PoolObjT* AllocateItem()
    {
        bool flipped = false;
        while (true)
        {
            if (m_usedSlots[m_lastFreeSlot])
            {
                m_lastFreeSlot++;
                if (m_lastFreeSlot >= m_size)
                {
                    if (flipped)
                        return nullptr;
                    m_lastFreeSlot = 0;
                    flipped = true;
                }
            }
            else
            {
                m_usedSlots[m_lastFreeSlot] = true;
                m_usedSize++;
                return &m_items[m_lastFreeSlot];
            }
        }
    }

    void RemoveItem(PoolObjT* item)
    {
        auto pos = item - m_items.get();
        assert(m_usedSlots[pos], "Invalid item for CDynamicPoolPart::RemoveItem");

        m_usedSlots[pos] = false;
        m_usedSize--;
    }

    bool        OwnsItem(PoolObjT* item) const noexcept { return item >= m_items.get() && item < m_items.get() + m_size * sizeof(PoolObjT); }
    bool        HasFreeSize() const noexcept { return m_size != m_usedSize; }
    std::size_t GetCapacity() const noexcept { return m_size; }
    std::size_t GetUsedSize() const noexcept { return m_usedSize; }

private:
    std::unique_ptr<PoolObjT[]> m_items;
    std::unique_ptr<bool[]>     m_usedSlots;
    const std::size_t           m_size;
    std::size_t                 m_usedSize;
    std::size_t                 m_lastFreeSlot;
};

template <std::size_t InitialSize>
struct PoolGrownByHalfStrategy
{
    static constexpr std::size_t GetInitialSize() { return InitialSize; }
    static constexpr std::size_t GetNextSize(std::size_t index) { return InitialSize / 2; }
};

template <typename PoolObjT, typename GrownStrategy>
class CDynamicPool
{
public:
    CDynamicPool()
    {
        constexpr size_t initialSize = GrownStrategy::GetInitialSize();
        m_poolParts.emplace_back(std::make_unique<CDynamicPoolPart<PoolObjT>>(initialSize));
    }

    PoolObjT* AllocateItem()
    {
        for (auto& pool : m_poolParts)
        {
            if (pool->HasFreeSize())
                return pool->AllocateItem();
        }

        return AllocateNewPart()->AllocateItem();
    }

    void RemoveItem(PoolObjT* item)
    {
        for (auto& pool : m_poolParts)
        {
            if (pool->OwnsItem(item))
            {
                pool->RemoveItem(item);
                return;
            }
        }

        throw std::exception("Invalid item for CDynamicPool::RemoveItem");
    }

    std::size_t GetCapacity() const noexcept
    {
        std::size_t size = 0;
        for (auto& pool : m_poolParts)
            size += pool->GetCapacity();

        return size;
    }

    std::size_t GetUsedSize() const noexcept
    {
        std::size_t size = 0;
        for (auto& pool : m_poolParts)
            size += pool->GetUsedSize();

        return size;
    }

    bool SetCapacity(std::size_t newSize) {
        if (newSize == 0)
            return false;

        std::size_t currentSize = GetCapacity();

        if (currentSize == newSize)
            return false;
        else if (currentSize < newSize)
        {
            // Grown
            while (currentSize < newSize)
            {
                try
                {
                    auto* nextPart = AllocateNewPart();
                    currentSize += nextPart->GetCapacity();
                }
                catch (const std::bad_alloc& ex)
                {
                    return false;
                }
            }
        }
        else
        {
            // Shrink
            while (true)
            {
                auto* part = m_poolParts.back().get();
                if (part->GetUsedSize() != 0)
                    return false;

                currentSize -= part->GetCapacity();
                if (currentSize < newSize)
                    return false;

                m_poolParts.pop_back();

                if (currentSize == newSize)
                    return true;
            }
        }
        
        return true;
    }

private:
    CDynamicPoolPart<PoolObjT>* AllocateNewPart()
    {
        const std::size_t nextSize = GrownStrategy::GetNextSize(m_poolParts.size());
        m_poolParts.emplace_back(std::make_unique<CDynamicPoolPart<PoolObjT>>(nextSize));
        return m_poolParts.back().get();
    }

private:
    std::vector<std::unique_ptr<CDynamicPoolPart<PoolObjT>>> m_poolParts;
};
