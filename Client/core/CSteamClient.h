/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CSteamClient.h
 *  PURPOSE:     Header file for Steam client integration
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <cstdint>
#include <string>

class ISteamClient;

using HSteamPipe = std::int32_t;
using HSteamUser = std::int32_t;

using HINSTANCE = struct HINSTANCE__*;
using HMODULE = HINSTANCE;

class CSteamClient final
{
public:
    /**
     * @brief Loads the steamclient.dll to allow communication with the Steam client.
     * @return A boolean whether the operation was successful.
     */
    static bool Load();

    /**
     * @brief Establishes a connection with the local Steam process, and starts the GTA:SA activity.
     * @return A boolean whether the operation was successful.
     */
    bool Connect();

private:
    ISteamClient* m_client{};
    HSteamPipe    m_pipe{};
    HSteamUser    m_user{};

private:
    class Native final
    {
    public:
        static inline HMODULE Handle{};
        static inline ISteamClient* (*CreateInterface)(const char*, int*) = nullptr;
    };
};
