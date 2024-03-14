/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CInputRPCs.cpp
 *  PURPOSE:     Input remote procedure calls
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#include <StdInc.h>
#include "CInputRPCs.h"

void CInputRPCs::LoadFunctions()
{
    AddHandler(BIND_KEY, BindKey, "BindKey");
    AddHandler(BIND_COMMAND, BindCommand, "BindCommand");
    AddHandler(UNBIND_KEY, UnbindKey, "UnbindKey");
    AddHandler(UNBIND_COMMAND, UnbindCommand, "UnbindCommand");
    AddHandler(TOGGLE_CONTROL_ABILITY, ToggleControl, "ToggleControl");
    AddHandler(TOGGLE_ALL_CONTROL_ABILITY, ToggleAllControls, "ToggleAllControls");
    AddHandler(SET_CONTROL_STATE, SetControlState, "SetControlState");
    AddHandler(FORCE_RECONNECT, ForceReconnect, "ForceReconnect");
    AddHandler(SHOW_CURSOR, ShowCursor, "ShowCursor");
    AddHandler(SHOW_CHAT, ShowChat, "ShowChat");
}

void CInputRPCs::BindKey(NetBitStreamInterface& bitStream)
{
    CKeyBindsInterface* pKeyBinds = g_pCore->GetKeyBinds();
    if (pKeyBinds)
    {
        std::uint8_t ucKeyLength = 0;
        if (bitStream.Read(ucKeyLength))
        {
            char* szKey = new char[ucKeyLength + 1];
            szKey[ucKeyLength] = NULL;

            std::uint8_t ucKeyState;
            if (bitStream.Read(szKey, ucKeyLength) && bitStream.Read(ucKeyState))
            {
                bool                bState = (ucKeyState == 1);
                const SBindableKey* pKey = pKeyBinds->GetBindableFromKey(szKey);
                if (pKey)
                {
                    // Make sure we only have it once
                    if (!pKeyBinds->FunctionExists(szKey, CClientGame::StaticProcessServerKeyBind, true, bState))
                    {
                        pKeyBinds->AddFunction(pKey, CClientGame::StaticProcessServerKeyBind, bState, false);
                    }
                }
                else
                {
                    SBindableGTAControl* pControl = pKeyBinds->GetBindableFromControl(szKey);
                    if (pControl)
                    {
                        // Make sure we only have it once
                        if (!pKeyBinds->ControlFunctionExists(szKey, CClientGame::StaticProcessServerControlBind, true, bState))
                        {
                            pKeyBinds->AddControlFunction(pControl, CClientGame::StaticProcessServerControlBind, bState);
                        }
                    }
                }
            }
            delete[] szKey;
        }
    }
}

void CInputRPCs::BindCommand(NetBitStreamInterface& bitStream)
{
    CKeyBindsInterface* pKeyBinds = g_pCore->GetKeyBinds();
    if (pKeyBinds)
    {
        std::uint8_t ucLength = 0;
        if (bitStream.Read(ucLength))
        {
            char* szKey = new char[ucLength + 1];
            szKey[ucLength] = NULL;
            bitStream.Read(szKey, ucLength);

            const char* szHitState;
            char        ucHitState;
            bitStream.Read(ucHitState);
            szHitState = ucHitState == 0 ? "down" : (ucHitState == 1 ? "up" : (ucHitState == 3 ? "both" : "down"));

            bitStream.Read(ucLength);
            char* szCommandName = new char[ucLength + 1];
            szCommandName[ucLength] = NULL;
            bitStream.Read(szCommandName, ucLength);

            bitStream.Read(ucLength);
            char* szArguments = new char[ucLength + 1];
            szArguments[ucLength] = NULL;
            bitStream.Read(szArguments, ucLength);

            bitStream.Read(ucLength);
            char* szResource = new char[ucLength + 1];
            szResource[ucLength] = NULL;
            bitStream.Read(szResource, ucLength);

            CStaticFunctionDefinitions::BindKey(szKey, szHitState, szCommandName, szArguments, szResource);
            delete[] szKey;
            delete[] szCommandName;
            delete[] szArguments;
            delete[] szResource;
        }
    }
}

void CInputRPCs::UnbindKey(NetBitStreamInterface& bitStream)
{
    CKeyBindsInterface* pKeyBinds = g_pCore->GetKeyBinds();
    if (pKeyBinds)
    {
        std::uint8_t ucKeyLength = 0;
        if (bitStream.Read(ucKeyLength))
        {
            char* szKey = new char[ucKeyLength + 1];
            szKey[ucKeyLength] = NULL;

            std::uint8_t ucKeyState;
            if (bitStream.Read(szKey, ucKeyLength) && bitStream.Read(ucKeyState))
            {
                bool                bState = (ucKeyState == 1);
                const SBindableKey* pKey = pKeyBinds->GetBindableFromKey(szKey);
                if (pKey)
                {
                    pKeyBinds->RemoveFunction(szKey, CClientGame::StaticProcessServerKeyBind, true, bState);
                }
                else
                {
                    SBindableGTAControl* pControl = pKeyBinds->GetBindableFromControl(szKey);
                    if (pControl)
                    {
                        pKeyBinds->RemoveControlFunction(szKey, CClientGame::StaticProcessServerControlBind, true, bState);
                    }
                }
            }
            delete[] szKey;
        }
    }
}

void CInputRPCs::UnbindCommand(NetBitStreamInterface& bitStream)
{
    CKeyBindsInterface* pKeyBinds = g_pCore->GetKeyBinds();
    if (pKeyBinds)
    {
        std::uint8_t ucLength = 0;
        if (bitStream.Read(ucLength))
        {
            char* szKey = new char[ucLength + 1];
            szKey[ucLength] = NULL;
            bitStream.Read(szKey, ucLength);

            const char* szHitState;
            char        ucHitState;
            bitStream.Read(ucHitState);
            szHitState = ucHitState == 0 ? "down" : (ucHitState == 1 ? "up" : (ucHitState == 3 ? "both" : "down"));

            bitStream.Read(ucLength);
            char* szCommandName = new char[ucLength + 1];
            szCommandName[ucLength] = NULL;
            bitStream.Read(szCommandName, ucLength);

            bitStream.Read(ucLength);
            char* szResource = new char[ucLength + 1];
            szResource[ucLength] = NULL;
            bitStream.Read(szResource, ucLength);

            CStaticFunctionDefinitions::UnbindKey(szKey, szHitState, szCommandName, szResource);
            delete[] szKey;
            delete[] szCommandName;
            delete[] szResource;
        }
    }
}

void CInputRPCs::ToggleControl(NetBitStreamInterface& bitStream)
{
    std::uint8_t ucControlLength = 0;
    if (bitStream.Read(ucControlLength))
    {
        char* szControl = new char[ucControlLength + 1];
        szControl[ucControlLength] = NULL;

        std::uint8_t ucEnabled;
        if (bitStream.Read(szControl, ucControlLength) && bitStream.Read(ucEnabled))
        {
            if (szControl[0])
            {
                g_pCore->GetKeyBinds()->SetControlEnabled(szControl, ucEnabled == 1);
            }
        }
        delete[] szControl;
    }
}

void CInputRPCs::ToggleAllControls(NetBitStreamInterface& bitStream)
{
    std::uint8_t ucGTAControls, ucMTAControls, ucEnabled;
    if (bitStream.Read(ucGTAControls) && bitStream.Read(ucMTAControls) && bitStream.Read(ucEnabled))
    {
        g_pCore->GetKeyBinds()->SetAllControlsEnabled(ucGTAControls == 1, ucMTAControls == 1, ucEnabled == 1);
    }
}

void CInputRPCs::SetControlState(NetBitStreamInterface& bitStream)
{
    std::uint8_t ucControlLength = 0;
    if (bitStream.Read(ucControlLength))
    {
        char* szControl = new char[ucControlLength + 1];
        szControl[ucControlLength] = NULL;

        std::uint8_t ucState;
        if (bitStream.Read(szControl, ucControlLength) && bitStream.Read(ucState))
        {
            if (*szControl)
            {
                CStaticFunctionDefinitions::SetControlState(szControl, ucState == 1);
            }
        }
        delete[] szControl;
    }
}

void CInputRPCs::ForceReconnect(NetBitStreamInterface& bitStream)
{
    std::uint8_t  ucHost, ucPassword;
    std::uint16_t usPort;

    if (bitStream.Read(ucHost))
    {
        char* szHost = new char[ucHost + 1];
        szHost[ucHost] = NULL;

        bitStream.Read(szHost, ucHost);

        if (bitStream.Read(usPort))
        {
            if (bitStream.Read(ucPassword))
            {
                char* szPassword = new char[ucPassword + 1];
                szPassword[ucPassword] = NULL;

                bitStream.Read(szPassword, ucPassword);

                if (szPassword[0])
                {
                    g_pCore->Reconnect(szHost, usPort, szPassword, false);
                    delete[] szPassword;
                    delete[] szHost;
                    return;
                }
                delete[] szPassword;
            }

            g_pCore->Reconnect(szHost, usPort, NULL);
        }
        delete[] szHost;
    }
}

void CInputRPCs::ShowCursor(NetBitStreamInterface& bitStream)
{
    std::uint8_t  ucShow;
    std::uint16_t usResource;
    std::uint8_t  ucToggleControls;
    if (bitStream.Read(ucShow) && bitStream.Read(usResource) && bitStream.Read(ucToggleControls))
    {
        CResource* pResource = g_pClientGame->GetResourceManager()->GetResourceFromNetID(usResource);
        if (pResource)
        {
            pResource->ShowCursor(ucShow == 1, ucToggleControls == 1);
        }
    }
}

void CInputRPCs::ShowChat(NetBitStreamInterface& bitStream)
{
    std::uint8_t ucShow;
    std::uint8_t ucInputBlocked;
    if (bitStream.Read(ucShow))
    {
        if (bitStream.Read(ucInputBlocked))
            g_pCore->SetChatVisible(ucShow == 1, ucInputBlocked == 1);
        else
            g_pCore->SetChatVisible(ucShow == 1, ucShow != 1);
    }
}
