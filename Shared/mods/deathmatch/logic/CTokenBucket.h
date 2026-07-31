/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/CTokenBucket.h
 *  PURPOSE:     Fixed rate token bucket for per-player packet throttling
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

// Bounds how often a player may send a given packet, while still absorbing the bunching
// that follows a lag spike. The caller supplies the time elapsed since the previous
// attempt, so the bucket keeps no clock of its own and can be tested directly.
class CTokenBucket
{
public:
    constexpr CTokenBucket(unsigned int capacity, unsigned int refillIntervalMs) noexcept
        : m_capacity(capacity ? capacity : 1u), m_refillIntervalMs(refillIntervalMs ? refillIntervalMs : 1u), m_tokens(capacity ? capacity : 1u)
    {
    }

    // Returns false when the bucket is empty, in which case the caller should drop the packet.
    bool Consume(unsigned long long elapsedMs) noexcept
    {
        if (m_tokens < m_capacity)
        {
            // Preserve sub-token time so normal packet jitter doesn't reduce the refill rate.
            m_refillRemainderMs += elapsedMs;

            if (const unsigned int refill = static_cast<unsigned int>(m_refillRemainderMs / m_refillIntervalMs))
            {
                const unsigned int tokens = m_tokens + refill;
                m_tokens = (tokens > m_capacity) ? m_capacity : tokens;
                m_refillRemainderMs %= m_refillIntervalMs;

                // Only forget the drops once the bucket is fully restored. Clearing them on any
                // partial refill lets a sender pace itself and never accumulate a single drop.
                if (m_tokens == m_capacity)
                {
                    m_drops = 0;
                    m_refillRemainderMs = 0;
                }
            }
        }
        else
        {
            // A sender that went quiet shouldn't bank burst capacity beyond the cap.
            m_refillRemainderMs = 0;
        }

        if (m_tokens == 0)
        {
            ++m_drops;
            return false;
        }

        --m_tokens;
        return true;
    }

    unsigned int GetDrops() const noexcept { return m_drops; }
    unsigned int GetTokens() const noexcept { return m_tokens; }

private:
    const unsigned int m_capacity;
    const unsigned int m_refillIntervalMs;
    unsigned int       m_tokens;
    unsigned int       m_drops{};
    unsigned long long m_refillRemainderMs{};
};
