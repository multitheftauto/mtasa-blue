/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CKeyBinds.cpp
 *  PURPOSE:     Server keybind manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CKeyBinds.h"
#include "CPlayer.h"

static const SFixedArray<SBindableKey, 106> g_bkKeys = {{{"mouse1"},
                                                         {"mouse2"},
                                                         {"mouse3"},
                                                         {"mouse_wheel_up"},
                                                         {"mouse_wheel_down"},
                                                         {"mouse4"},
                                                         {"mouse5"},
                                                         {"backspace"},
                                                         {"tab"},
                                                         {"lshift"},
                                                         {"rshift"},
                                                         {"lctrl"},
                                                         {"rctrl"},
                                                         {"lalt"},
                                                         {"ralt"},
                                                         {"pause"},
                                                         {"capslock"},
                                                         {"enter"},
                                                         {"space"},
                                                         {"pgup"},
                                                         {"pgdn"},
                                                         {"end"},
                                                         {"home"},
                                                         {"arrow_l"},
                                                         {"arrow_u"},
                                                         {"arrow_r"},
                                                         {"arrow_d"},
                                                         {"insert"},
                                                         {"delete"},
                                                         {"0"},
                                                         {"1"},
                                                         {"2"},
                                                         {"3"},
                                                         {"4"},
                                                         {"5"},
                                                         {"6"},
                                                         {"7"},
                                                         {"8"},
                                                         {"9"},
                                                         {"a"},
                                                         {"b"},
                                                         {"c"},
                                                         {"d"},
                                                         {"e"},
                                                         {"f"},
                                                         {"g"},
                                                         {"h"},
                                                         {"i"},
                                                         {"j"},
                                                         {"k"},
                                                         {"l"},
                                                         {"m"},
                                                         {"n"},
                                                         {"o"},
                                                         {"p"},
                                                         {"q"},
                                                         {"r"},
                                                         {"s"},
                                                         {"t"},
                                                         {"u"},
                                                         {"v"},
                                                         {"w"},
                                                         {"x"},
                                                         {"y"},
                                                         {"z"},
                                                         {"num_0"},
                                                         {"num_1"},
                                                         {"num_2"},
                                                         {"num_3"},
                                                         {"num_4"},
                                                         {"num_5"},
                                                         {"num_6"},
                                                         {"num_7"},
                                                         {"num_8"},
                                                         {"num_9"},
                                                         {"num_mul"},
                                                         {"num_add"},
                                                         {"num_sep"},
                                                         {"num_sub"},
                                                         {"num_dec"},
                                                         {"num_div"},
                                                         {"F1"},
                                                         {"F2"},
                                                         {"F3"},
                                                         {"F4"},
                                                         {"F5"},
                                                         {"F6"},
                                                         {"F7"}, /* { "F8" },  * Used for console */
                                                         {"F9"},
                                                         {"F10"},
                                                         {"F11"},
                                                         {"F12"},
                                                         {"scroll"},
                                                         {";"},
                                                         {"="},
                                                         {","},
                                                         {"-"},
                                                         {"."},
                                                         {"/"},
                                                         {"'"},
                                                         {"["},
                                                         {"\\"},
                                                         {"]"},
                                                         {"#"},
                                                         {"num_enter"},
                                                         {""}}};

static const SFixedArray<SBindableGTAControl, 45> g_bcControls = {{{"fire"},
                                                                   {"next_weapon"},
                                                                   {"previous_weapon"},
                                                                   {"forwards"},
                                                                   {"backwards"},
                                                                   {"left"},
                                                                   {"right"},
                                                                   {"zoom_in"},
                                                                   {"zoom_out"},
                                                                   {"enter_exit"},
                                                                   {"change_camera"},
                                                                   {"jump"},
                                                                   {"sprint"},
                                                                   {"look_behind"},
                                                                   {"crouch"},
                                                                   {"action"},
                                                                   {"walk"},
                                                                   {"vehicle_fire"},
                                                                   {"vehicle_secondary_fire"},
                                                                   {"vehicle_left"},
                                                                   {"vehicle_right"},
                                                                   {"steer_forward"},
                                                                   {"steer_back"},
                                                                   {"accelerate"},
                                                                   {"brake_reverse"},
                                                                   {"radio_next"},
                                                                   {"radio_previous"},
                                                                   {"radio_user_track_skip"},
                                                                   {"horn"},
                                                                   {"sub_mission"},
                                                                   {"handbrake"},
                                                                   {"vehicle_look_left"},
                                                                   {"vehicle_look_right"},
                                                                   {"vehicle_look_behind"},
                                                                   {"vehicle_mouse_look"},
                                                                   {"special_control_left"},
                                                                   {"special_control_right"},
                                                                   {"special_control_down"},
                                                                   {"special_control_up"},
                                                                   {"aim_weapon"},
                                                                   {"conversation_yes"},
                                                                   {"conversation_no"},
                                                                   {"group_control_forwards"},
                                                                   {"group_control_back"},
                                                                   {""}}};

CKeyBinds::CKeyBinds(CPlayer* pPlayer)
{
    m_bProcessingKey = false;
    m_pPlayer = pPlayer;
}

CKeyBinds::~CKeyBinds()
{
    Clear();
}

const SBindableKey* CKeyBinds::GetBindableFromKey(const char* szKey)
{
    // Map for faster lookup
    static std::map<SString, const SBindableKey*> bindableKeyMap;

    // Init map if required
    if (bindableKeyMap.empty())
        for (int i = 0; *g_bkKeys[i].szKey != 0; i++)
            MapSet(bindableKeyMap, SStringX(g_bkKeys[i].szKey).ToLower(), &g_bkKeys[i]);

    return MapFindRef(bindableKeyMap, SStringX(szKey).ToLower());
}

const SBindableGTAControl* CKeyBinds::GetBindableFromControl(const char* szControl)
{
    // Map for faster lookup
    static std::map<SString, const SBindableGTAControl*> bindableControlMap;

    // Init map if required
    if (bindableControlMap.empty())
        for (int i = 0; *g_bcControls[i].szControl != 0; i++)
            MapSet(bindableControlMap, SStringX(g_bcControls[i].szControl).ToLower(), &g_bcControls[i]);

    return MapFindRef(bindableControlMap, SStringX(szControl).ToLower());
}

void CKeyBinds::Add(CKeyBind* pKeyBind)
{
    if (pKeyBind)
    {
        m_List.push_back(pKeyBind);
    }
}

bool CKeyBinds::Remove(CKeyBind* pKeyBind)
{
    if (!m_bProcessingKey)
    {
        m_List.remove(pKeyBind);
        delete pKeyBind;
        return true;
    }

    pKeyBind->beingDeleted = true;
    return false;
}

void CKeyBinds::Clear(eKeyBindType bindType)
{
    std::list<CKeyBind*>::iterator iter = m_List.begin();
    while (iter != m_List.end())
    {
        if ((!(*iter)->IsBeingDeleted() && bindType == KEY_BIND_UNDEFINED) || (*iter)->GetType() == bindType)
        {
            if (m_bProcessingKey)
                (*iter)->beingDeleted = true;
            else
            {
                delete *iter;
                iter = m_List.erase(iter);
                continue;
            }
        }
        iter++;
    }
}

void CKeyBinds::Call(CKeyBind* pKeyBind)
{
    if (pKeyBind && !pKeyBind->IsBeingDeleted())
    {
        switch (pKeyBind->GetType())
        {
            case KEY_BIND_FUNCTION:
            {
                CKeyFunctionBind* pBind = static_cast<CKeyFunctionBind*>(pKeyBind);
                if (pBind->luaMain && VERIFY_FUNCTION(pBind->m_iLuaFunction))
                {
                    CLuaArguments Arguments;
                    Arguments.PushElement(m_pPlayer);
                    Arguments.PushString(pBind->boundKey->szKey);
                    Arguments.PushString((pBind->bHitState) ? "down" : "up");
                    Arguments.PushArguments(pBind->m_Arguments);
                    Arguments.Call(pBind->luaMain, pBind->m_iLuaFunction);
                }
                break;
            }
            case KEY_BIND_CONTROL_FUNCTION:
            {
                CControlFunctionBind* pBind = static_cast<CControlFunctionBind*>(pKeyBind);
                if (pBind->luaMain && VERIFY_FUNCTION(pBind->m_iLuaFunction))
                {
                    CLuaArguments Arguments;
                    Arguments.PushElement(m_pPlayer);
                    Arguments.PushString(pBind->boundControl->szControl);
                    Arguments.PushString((pBind->bHitState) ? "down" : "up");
                    Arguments.PushArguments(pBind->m_Arguments);
                    Arguments.Call(pBind->luaMain, pBind->m_iLuaFunction);
                }
                break;
            }
            default:
                break;
        }
    }
}

bool CKeyBinds::ProcessKey(const char* szKey, bool bHitState, eKeyBindType bindType)
{
    const SBindableKey*        pKey = NULL;
    const SBindableGTAControl* pControl = NULL;
    if (bindType == KEY_BIND_FUNCTION)
    {
        pKey = GetBindableFromKey(szKey);
        if (!pKey)
            return false;
    }
    else if (bindType == KEY_BIND_CONTROL_FUNCTION)
    {
        pControl = GetBindableFromControl(szKey);
        if (!pControl)
            return false;
    }

    m_bProcessingKey = true;
    bool                           bFound = false;
    CKeyBind*                      pKeyBind = NULL;
    std::list<CKeyBind*>           cloneList = m_List;
    std::list<CKeyBind*>::iterator iter = cloneList.begin();
    for (; iter != cloneList.end(); ++iter)
    {
        pKeyBind = *iter;
        if (!pKeyBind->IsBeingDeleted() && pKeyBind->GetType() == bindType)
        {
            switch (bindType)
            {
                case KEY_BIND_FUNCTION:
                {
                    CKeyFunctionBind* pBind = static_cast<CKeyFunctionBind*>(pKeyBind);
                    if (pBind->boundKey == pKey)
                    {
                        if (pBind->bHitState == bHitState)
                        {
                            Call(pBind);
                            bFound = true;
                        }
                    }
                    break;
                }
                case KEY_BIND_CONTROL_FUNCTION:
                {
                    CControlFunctionBind* pBind = static_cast<CControlFunctionBind*>(pKeyBind);
                    if (pBind->boundControl == pControl)
                    {
                        if (pBind->bHitState == bHitState)
                        {
                            Call(pBind);
                            bFound = true;
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
    m_bProcessingKey = false;
    RemoveDeletedBinds();
    return bFound;
}

bool CKeyBinds::AddKeyFunction(const char* szKey, bool bHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments& Arguments)
{
    if (szKey == NULL || IS_REFNIL(iLuaFunction))
        return false;

    const SBindableKey* pKey = GetBindableFromKey(szKey);
    if (pKey)
    {
        CKeyFunctionBind* pBind = new CKeyFunctionBind;
        pBind->boundKey = pKey;
        pBind->bHitState = bHitState;
        pBind->luaMain = pLuaMain;
        pBind->m_iLuaFunction = iLuaFunction;
        pBind->m_Arguments = Arguments;

        m_List.push_back(pBind);

        return true;
    }
    return false;
}

bool CKeyBinds::AddKeyFunction(const SBindableKey* pKey, bool bHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments& Arguments)
{
    if (pKey)
    {
        CKeyFunctionBind* pBind = new CKeyFunctionBind;
        pBind->boundKey = pKey;
        pBind->bHitState = bHitState;
        pBind->luaMain = pLuaMain;
        pBind->m_iLuaFunction = iLuaFunction;
        pBind->m_Arguments = Arguments;

        m_List.push_back(pBind);

        return true;
    }
    return false;
}

bool CKeyBinds::RemoveKeyFunction(const char* szKey, CLuaMain* pLuaMain, bool bCheckHitState, bool bHitState, const CLuaFunctionRef& iLuaFunction)
{
    bool                           bFound = false;
    CKeyFunctionBind*              pBind = NULL;
    std::list<CKeyBind*>           cloneList = m_List;
    std::list<CKeyBind*>::iterator iter = cloneList.begin();
    while (iter != cloneList.end())
    {
        if (!(*iter)->IsBeingDeleted() && (*iter)->GetType() == KEY_BIND_FUNCTION)
        {
            pBind = static_cast<CKeyFunctionBind*>(*iter);
            if (!stricmp(szKey, pBind->boundKey->szKey))
            {
                if (pBind->luaMain == pLuaMain)
                {
                    if (!bCheckHitState || pBind->bHitState == bHitState)
                    {
                        if (IS_REFNIL(iLuaFunction) || pBind->m_iLuaFunction == iLuaFunction)
                        {
                            bFound = true;

                            if (m_bProcessingKey)
                                pBind->beingDeleted = true;
                            else
                            {
                                m_List.remove(pBind);
                                delete pBind;
                                iter = cloneList.erase(iter);
                                continue;
                            }
                        }
                    }
                }
            }
        }
        ++iter;
    }
    return bFound;
}

bool CKeyBinds::KeyFunctionExists(const char* szKey, CLuaMain* pLuaMain, bool bCheckHitState, bool bHitState, const CLuaFunctionRef& iLuaFunction)
{
    bool                           bFound = false;
    std::list<CKeyBind*>           cloneList = m_List;
    std::list<CKeyBind*>::iterator iter = cloneList.begin();
    for (; iter != cloneList.end(); ++iter)
    {
        if ((*iter)->GetType() == KEY_BIND_FUNCTION)
        {
            CKeyFunctionBind* pBind = static_cast<CKeyFunctionBind*>(*iter);
            if (stricmp(szKey, pBind->boundKey->szKey) == 0)
            {
                if (pLuaMain == NULL || pBind->luaMain == pLuaMain)
                {
                    if (!bCheckHitState || pBind->bHitState == bHitState)
                    {
                        if (IS_REFNIL(iLuaFunction) || pBind->m_iLuaFunction == iLuaFunction)
                        {
                            bFound = true;
                        }
                    }
                }
            }
        }
    }
    return bFound;
}

void CKeyBinds::RemoveAllKeys(CLuaMain* pLuaMain)
{
    CKeyBind*                      pBind = NULL;
    std::list<CKeyBind*>           cloneList = m_List;
    std::list<CKeyBind*>::iterator iter = cloneList.begin();
    while (iter != cloneList.end())
    {
        pBind = *iter;
        if (!pBind->IsBeingDeleted())
        {
            if (pBind->luaMain == pLuaMain)
            {
                if (m_bProcessingKey)
                    pBind->beingDeleted = true;
                else
                {
                    m_List.remove(pBind);
                    delete pBind;
                    iter = cloneList.erase(iter);
                    continue;
                }
            }
        }
        ++iter;
    }
}

bool CKeyBinds::AddControlFunction(const char* szControl, bool bHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments& Arguments)
{
    if (szControl == NULL)
        return false;

    const SBindableGTAControl* pControl = GetBindableFromControl(szControl);
    if (pControl)
    {
        CControlFunctionBind* pBind = new CControlFunctionBind;
        pBind->boundKey = NULL;
        pBind->boundControl = pControl;
        pBind->bHitState = bHitState;
        pBind->luaMain = pLuaMain;
        pBind->m_iLuaFunction = iLuaFunction;
        pBind->m_Arguments = Arguments;

        m_List.push_back(pBind);

        return true;
    }
    return false;
}

bool CKeyBinds::AddControlFunction(const SBindableGTAControl* pControl, bool bHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction,
                                   CLuaArguments& Arguments)
{
    if (pControl)
    {
        CControlFunctionBind* pBind = new CControlFunctionBind;
        pBind->boundKey = NULL;
        pBind->boundControl = pControl;
        pBind->bHitState = bHitState;
        pBind->luaMain = pLuaMain;
        pBind->m_iLuaFunction = iLuaFunction;
        pBind->m_Arguments = Arguments;

        m_List.push_back(pBind);

        return true;
    }
    return false;
}

bool CKeyBinds::RemoveControlFunction(const char* szControl, CLuaMain* pLuaMain, bool bCheckHitState, bool bHitState, const CLuaFunctionRef& iLuaFunction)
{
    bool                           bFound = false;
    CControlFunctionBind*          pBind = NULL;
    std::list<CKeyBind*>           cloneList = m_List;
    std::list<CKeyBind*>::iterator iter = cloneList.begin();
    while (iter != cloneList.end())
    {
        if ((*iter)->GetType() == KEY_BIND_CONTROL_FUNCTION)
        {
            pBind = static_cast<CControlFunctionBind*>(*iter);
            if (stricmp(szControl, pBind->boundControl->szControl) == 0)
            {
                if (pBind->luaMain == pLuaMain)
                {
                    if (!bCheckHitState || pBind->bHitState == bHitState)
                    {
                        if (IS_REFNIL(iLuaFunction) || pBind->m_iLuaFunction == iLuaFunction)
                        {
                            bFound = true;
                            if (m_bProcessingKey)
                                (*iter)->beingDeleted = true;
                            else
                            {
                                m_List.remove(*iter);
                                delete *iter;
                                iter = cloneList.erase(iter);
                                continue;
                            }
                        }
                    }
                }
            }
        }
        ++iter;
    }
    return bFound;
}

bool CKeyBinds::ControlFunctionExists(const char* szControl, CLuaMain* pLuaMain, bool bCheckHitState, bool bHitState, const CLuaFunctionRef& iLuaFunction)
{
    bool                           bFound = false;
    std::list<CKeyBind*>           cloneList = m_List;
    std::list<CKeyBind*>::iterator iter = cloneList.begin();
    for (; iter != cloneList.end(); ++iter)
    {
        if ((*iter)->GetType() == KEY_BIND_CONTROL_FUNCTION)
        {
            CControlFunctionBind* pBind = static_cast<CControlFunctionBind*>(*iter);
            if (stricmp(szControl, pBind->boundControl->szControl) == 0)
            {
                if (pLuaMain == NULL || pBind->luaMain == pLuaMain)
                {
                    if (!bCheckHitState || pBind->bHitState == bHitState)
                    {
                        if (IS_REFNIL(iLuaFunction) || pBind->m_iLuaFunction == iLuaFunction)
                        {
                            bFound = true;
                        }
                    }
                }
            }
        }
    }
    return bFound;
}

bool CKeyBinds::IsMouse(const SBindableKey* pKey)
{
    if (pKey)
    {
        // All the mouse keys start with "mouse.."
        char szTemp[50];
        strcpy(szTemp, pKey->szKey);

        szTemp[5] = 0;
        if (strcmp(szTemp, "mouse") == 0)
        {
            return true;
        }
    }
    return false;
}

void CKeyBinds::RemoveDeletedBinds()
{
    std::list<CKeyBind*>::iterator iter = m_List.begin();
    while (iter != m_List.end())
    {
        if ((*iter)->IsBeingDeleted())
        {
            delete *iter;
            iter = m_List.erase(iter);
        }
        else
            ++iter;
    }
}
