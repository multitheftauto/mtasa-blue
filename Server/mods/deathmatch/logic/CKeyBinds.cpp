/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CKeyBinds.cpp
 *  PURPOSE:     Server keybind manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CKeyBinds.h"

// Note std::less<> is for transparent lookuo, so std::string_view can be used
// directly without converting it to an std::string
std::set<std::string, std::less<>> CKeyBinds::ms_bindableKeys;
std::set<std::string, std::less<>> CKeyBinds::ms_bindableGTAControls;


void CKeyBinds::IsBindableKey(std::string_view key)
{
    return s_bindableKeys.find(key) != s_bindableKeys.end();
}

void CKeyBinds::IsBindableControl(std::string_view control)
{
    return s_bindableGTAControls.find(control) != s_bindableGTAControls.end();
}

