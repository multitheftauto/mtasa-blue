/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CColModelGuard.h
 *  PURPOSE:     Wrapper for collision model access with automatic reference counting
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CModelInfoSA.h"

// Guard: Protects collision model from streaming GC
class CColModelGuard
{
public:
    // Increment ref count for pModelInfo's collision
    explicit CColModelGuard(CModelInfoSA* pModelInfo) : m_pModelInfo(pModelInfo), m_bValid(false), m_pColModel(nullptr), m_pColData(nullptr)
    {
        if (!m_pModelInfo)
            return;

        // Check and protect collision atomically
        if (m_pModelInfo->IsCollisionLoaded())
        {
            m_pModelInfo->AddColRef();
            bool bRefAdded = true;

            try
            {
                // Cache pointers while protected
                CBaseModelInfoSAInterface* pInterface = m_pModelInfo->GetInterface();
                if (pInterface && pInterface->pColModel)
                {
                    m_pColModel = pInterface->pColModel;
                    m_pColData = m_pColModel->m_data;
                    m_bValid = (m_pColData != nullptr);
                }

                // Release ref if validation failed
                if (!m_bValid)
                {
                    m_pModelInfo->RemoveColRef();
                    bRefAdded = false;
                }
            }
            catch (...)
            {
                // Release reference on exception
                if (bRefAdded)
                    m_pModelInfo->RemoveColRef();
                throw;
            }

            // Prevent double-release in destructor
            if (!bRefAdded)
                m_pModelInfo = nullptr;
        }
    }

    // Decrement ref count on destruction
    ~CColModelGuard() noexcept
    {
        if (m_bValid && m_pModelInfo)
        {
            m_pModelInfo->RemoveColRef();
        }
    }

    // Prevent copying
    CColModelGuard(const CColModelGuard&) = delete;
    CColModelGuard& operator=(const CColModelGuard&) = delete;

    // Allow moving
    CColModelGuard(CColModelGuard&& other) noexcept
        : m_pModelInfo(other.m_pModelInfo), m_bValid(other.m_bValid), m_pColModel(other.m_pColModel), m_pColData(other.m_pColData)
    {
        // Transfer ownership
        other.m_pModelInfo = nullptr;
        other.m_bValid = false;
        other.m_pColModel = nullptr;
        other.m_pColData = nullptr;
    }

    CColModelGuard& operator=(CColModelGuard&& other) noexcept
    {
        if (this != &other)
        {
            // Release current, transfer from source
            if (m_bValid && m_pModelInfo)
                m_pModelInfo->RemoveColRef();

            // Transfer ownership from source
            m_pModelInfo = other.m_pModelInfo;
            m_bValid = other.m_bValid;
            m_pColModel = other.m_pColModel;
            m_pColData = other.m_pColData;

            // Prevent double-release
            other.m_pModelInfo = nullptr;
            other.m_bValid = false;
            other.m_pColModel = nullptr;
            other.m_pColData = nullptr;
        }
        return *this;
    }

    // Check if collision is protected
    [[nodiscard]] bool IsValid() const noexcept { return m_bValid; }

    // Get collision data (check IsValid first)
    // Returns cached pointer or nullptr
    [[nodiscard]] CColDataSA* GetColData() const noexcept { return m_bValid ? m_pColData : nullptr; }

    // Get collision model (check IsValid first)
    // Returns cached pointer or nullptr
    [[nodiscard]] CColModelSAInterface* GetColModel() const noexcept { return m_bValid ? m_pColModel : nullptr; }

    explicit operator bool() const noexcept { return m_bValid; }

private:
    CModelInfoSA*         m_pModelInfo;
    bool                  m_bValid;
    CColModelSAInterface* m_pColModel;  // Cached during construction
    CColDataSA*           m_pColData;   // Cached during construction
};
