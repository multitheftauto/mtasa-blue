/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/CEventContext.h
 *  PURPOSE:     Per-event context for event dispatch and cancellation
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>

class CEventContext
{
public:
    CEventContext() noexcept : m_isCancelled(false) {}

    void Cancel(const char* reason = nullptr) noexcept
    {
        m_isCancelled = true;
        if (reason)
            m_cancelReason = reason;
    }

    bool               IsCancelled() const noexcept { return m_isCancelled; }
    const std::string& GetCancelReason() const noexcept { return m_cancelReason; }

    void Reset() noexcept
    {
        m_isCancelled = false;
        m_cancelReason.clear();
    }

private:
    bool        m_isCancelled;
    std::string m_cancelReason;
};
