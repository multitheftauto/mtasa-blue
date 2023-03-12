/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CKeyBinds.cpp
 *  PURPOSE:     Core keybind manager
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>
#include <game/CPed.h>
#include <game/CPedIntelligence.h>
#include <game/CPools.h>
#include <game/CTaskManager.h>
#include <game/Task.h>

const SBindableKey g_bkKeys[] = {{"mouse1", VK_LBUTTON, GTA_KEY_LMOUSE, DATA_NONE, 0},
                                 {"mouse2", VK_RBUTTON, GTA_KEY_RMOUSE, DATA_NONE, 0},
                                 {"mouse3", VK_MBUTTON, GTA_KEY_MMOUSE, DATA_NONE, 0},
                                 {"mouse4", 0x00, GTA_KEY_MXB1, DATA_NONE, 0},
                                 {"mouse5", 0x00, GTA_KEY_MXB2, DATA_NONE, 0},
                                 {"mouse_wheel_up", 0x00, GTA_KEY_MSCROLLUP, DATA_NONE, 0},
                                 {"mouse_wheel_down", 0x00, GTA_KEY_MSCROLLDOWN, DATA_NONE, 0},
                                 {"backspace", VK_BACK, GTA_KEY_BACK, DATA_NONE, 0},
                                 {"tab", VK_TAB, GTA_KEY_TAB, DATA_NONE, 0},

                                 // TODO: Consider using VK_(L|R)(SHIFT|CONTROL|MENU)
                                 {"lshift", VK_SHIFT, GTA_KEY_LSHIFT, DATA_NOT_EXTENDED, 0},
                                 {"rshift", VK_SHIFT, GTA_KEY_RSHIFT, DATA_EXTENDED, 0},            // 10
                                 {"lctrl", VK_CONTROL, GTA_KEY_LCONTROL, DATA_NOT_EXTENDED, 0},
                                 {"rctrl", VK_CONTROL, GTA_KEY_RCONTROL, DATA_EXTENDED, 0},
                                 {"lalt", VK_MENU, GTA_KEY_LMENU, DATA_NOT_EXTENDED, 0, true},
                                 {"ralt", VK_MENU, GTA_KEY_RMENU, DATA_EXTENDED, 0, true},

                                 {"pause", VK_PAUSE, GTA_KEY_PAUSE, DATA_NONE, 0, true},
                                 {"capslock", VK_CAPITAL, GTA_KEY_CAPSLOCK, DATA_NONE, 0},
                                 {"enter", VK_RETURN, GTA_KEY_RETURN, DATA_NOT_EXTENDED, 0},
                                 {"space", VK_SPACE, GTA_KEY_SPACE, DATA_NONE, 0},
                                 {"pgup", VK_PRIOR, GTA_KEY_PGUP, DATA_NUMPAD, 74, true},
                                 {"pgdn", VK_NEXT, GTA_KEY_PGDN, DATA_NUMPAD, 68, true},            // 20
                                 {"end", VK_END, GTA_KEY_END, DATA_NUMPAD, 66},
                                 {"home", VK_HOME, GTA_KEY_HOME, DATA_NUMPAD, 72},
                                 {"arrow_l", VK_LEFT, GTA_KEY_LEFT, DATA_NUMPAD, 69},
                                 {"arrow_u", VK_UP, GTA_KEY_UP, DATA_NUMPAD, 73},
                                 {"arrow_r", VK_RIGHT, GTA_KEY_RIGHT, DATA_NUMPAD, 71},
                                 {"arrow_d", VK_DOWN, GTA_KEY_DOWN, DATA_NUMPAD, 67},
                                 {"insert", VK_INSERT, GTA_KEY_INSERT, DATA_NUMPAD, 65, true},
                                 {"delete", VK_DELETE, GTA_KEY_DELETE, DATA_NUMPAD, 79},
                                 {"0", 0x30, GTA_KEY_0, DATA_NONE, 0},
                                 {"1", 0x31, GTA_KEY_1, DATA_NONE, 0},            // 30
                                 {"2", 0x32, GTA_KEY_2, DATA_NONE, 0},
                                 {"3", 0x33, GTA_KEY_3, DATA_NONE, 0},
                                 {"4", 0x34, GTA_KEY_4, DATA_NONE, 0},
                                 {"5", 0x35, GTA_KEY_5, DATA_NONE, 0},
                                 {"6", 0x36, GTA_KEY_6, DATA_NONE, 0},
                                 {"7", 0x37, GTA_KEY_7, DATA_NONE, 0},
                                 {"8", 0x38, GTA_KEY_8, DATA_NONE, 0},
                                 {"9", 0x39, GTA_KEY_9, DATA_NONE, 0},
                                 {"a", 0x41, GTA_KEY_A, DATA_NONE, 0},
                                 {"b", 0x42, GTA_KEY_B, DATA_NONE, 0},            // 40
                                 {"c", 0x43, GTA_KEY_C, DATA_NONE, 0},
                                 {"d", 0x44, GTA_KEY_D, DATA_NONE, 0},
                                 {"e", 0x45, GTA_KEY_E, DATA_NONE, 0},
                                 {"f", 0x46, GTA_KEY_F, DATA_NONE, 0},
                                 {"g", 0x47, GTA_KEY_G, DATA_NONE, 0},
                                 {"h", 0x48, GTA_KEY_H, DATA_NONE, 0},
                                 {"i", 0x49, GTA_KEY_I, DATA_NONE, 0},
                                 {"j", 0x4A, GTA_KEY_J, DATA_NONE, 0},
                                 {"k", 0x4B, GTA_KEY_K, DATA_NONE, 0},
                                 {"l", 0x4C, GTA_KEY_L, DATA_NONE, 0},            // 50
                                 {"m", 0x4D, GTA_KEY_M, DATA_NONE, 0},
                                 {"n", 0x4E, GTA_KEY_N, DATA_NONE, 0},
                                 {"o", 0x4F, GTA_KEY_O, DATA_NONE, 0},
                                 {"p", 0x50, GTA_KEY_P, DATA_NONE, 0},
                                 {"q", 0x51, GTA_KEY_Q, DATA_NONE, 0},
                                 {"r", 0x52, GTA_KEY_R, DATA_NONE, 0},
                                 {"s", 0x53, GTA_KEY_S, DATA_NONE, 0},
                                 {"t", 0x54, GTA_KEY_T, DATA_NONE, 0},
                                 {"u", 0x55, GTA_KEY_U, DATA_NONE, 0},
                                 {"v", 0x56, GTA_KEY_V, DATA_NONE, 0},            // 60
                                 {"w", 0x57, GTA_KEY_W, DATA_NONE, 0},
                                 {"x", 0x58, GTA_KEY_X, DATA_NONE, 0},
                                 {"y", 0x59, GTA_KEY_Y, DATA_NONE, 0},
                                 {"z", 0x5A, GTA_KEY_Z, DATA_NONE, 0},
                                 {"num_0", VK_NUMPAD0, GTA_KEY_NUMPAD0, DATA_NONE, 0},
                                 {"num_1", VK_NUMPAD1, GTA_KEY_NUMPAD1, DATA_NONE, 0},
                                 {"num_2", VK_NUMPAD2, GTA_KEY_NUMPAD2, DATA_NONE, 0},
                                 {"num_3", VK_NUMPAD3, GTA_KEY_NUMPAD3, DATA_NONE, 0},
                                 {"num_4", VK_NUMPAD4, GTA_KEY_NUMPAD4, DATA_NONE, 0},
                                 {"num_5", VK_NUMPAD5, GTA_KEY_NUMPAD5, DATA_NONE, 0},            // 70
                                 {"num_6", VK_NUMPAD6, GTA_KEY_NUMPAD6, DATA_NONE, 0},
                                 {"num_7", VK_NUMPAD7, GTA_KEY_NUMPAD7, DATA_NONE, 0},
                                 {"num_8", VK_NUMPAD8, GTA_KEY_NUMPAD8, DATA_NONE, 0},
                                 {"num_9", VK_NUMPAD9, GTA_KEY_NUMPAD9, DATA_NONE, 0},
                                 {"num_mul", VK_MULTIPLY, GTA_KEY_MULTIPLY, DATA_NONE, 0},
                                 {"num_add", VK_ADD, GTA_KEY_ADD, DATA_NONE, 0},
                                 {"num_sep", VK_SEPARATOR, NO_KEY_DEFINED, DATA_NONE, 0},
                                 {"num_sub", VK_SUBTRACT, GTA_KEY_SUBTRACT, DATA_NONE, 0},
                                 {"num_dec", VK_DECIMAL, GTA_KEY_DECIMAL, DATA_NONE, 0},
                                 {"num_div", VK_DIVIDE, GTA_KEY_DIVIDE, DATA_NONE, 0},            // 80
                                 {"F1", VK_F1, GTA_KEY_F1, DATA_NONE, 0, true},
                                 {"F2", VK_F2, GTA_KEY_F2, DATA_NONE, 0, true},
                                 {"F3", VK_F3, GTA_KEY_F3, DATA_NONE, 0, true},
                                 {"F4", VK_F4, GTA_KEY_F4, DATA_NONE, 0, true},
                                 {"F5", VK_F5, GTA_KEY_F5, DATA_NONE, 0, true},
                                 {"F6", VK_F6, GTA_KEY_F6, DATA_NONE, 0, true},
                                 {"F7", VK_F7, GTA_KEY_F7, DATA_NONE, 0, true},
                                 //  {"F8", VK_F8, GTA_KEY_F8, DATA_NONE, 0, true}, * Used for console
                                 {"F9", VK_F9, GTA_KEY_F9, DATA_NONE, 0, true},
                                 {"F10", VK_F10, GTA_KEY_F10, DATA_NONE, 0, true},
                                 {"F11", VK_F11, GTA_KEY_F11, DATA_NONE, 0, true},            // 90
                                 {"F12", VK_F12, GTA_KEY_F12, DATA_NONE, 0, true},
                                 {"scroll", VK_SCROLL, GTA_KEY_SCROLL, DATA_NONE, 0, true},
                                 {";", 0xBA, GTA_KEY_SEMICOLON, DATA_NONE, 0},
                                 {"=", 0xBB, GTA_KEY_EQUALS, DATA_NONE, 0},
                                 {",", 0xBC, GTA_KEY_COMMA, DATA_NONE, 0},
                                 {"-", 0xBD, GTA_KEY_MINUS, DATA_NONE, 0},
                                 {".", 0xBE, GTA_KEY_PERIOD, DATA_NONE, 0},
                                 {"/", 0xBF, GTA_KEY_SLASH, DATA_NONE, 0},
                                 {"'", 0xC0, GTA_KEY_APOSTROPHE, DATA_NONE, 0},
                                 {"[", 0xDB, GTA_KEY_LBRACKET, DATA_NONE, 0},            // 100
                                 {"\\", 0xDC, GTA_KEY_BACKSLASH, DATA_NONE, 0},
                                 {"]", 0xDD, GTA_KEY_RBRACKET, DATA_NONE, 0},
                                 {"#", 0xDE, GTA_KEY_HASH, DATA_NONE, 0},
                                 {"num_enter", VK_RETURN, GTA_KEY_NUMPADENTER, DATA_EXTENDED, 0},
                                 {"clear", VK_CLEAR, NO_KEY_DEFINED, DATA_NUMPAD, 70},

                                 {"joy1", VK_JOY(1), GTA_KEY_JOY(1), DATA_NONE, 0},
                                 {"joy2", VK_JOY(2), GTA_KEY_JOY(2), DATA_NONE, 0},
                                 {"joy3", VK_JOY(3), GTA_KEY_JOY(3), DATA_NONE, 0},
                                 {"joy4", VK_JOY(4), GTA_KEY_JOY(4), DATA_NONE, 0},
                                 {"joy5", VK_JOY(5), GTA_KEY_JOY(5), DATA_NONE, 0},
                                 {"joy6", VK_JOY(6), GTA_KEY_JOY(6), DATA_NONE, 0},
                                 {"joy7", VK_JOY(7), GTA_KEY_JOY(7), DATA_NONE, 0},
                                 {"joy8", VK_JOY(8), GTA_KEY_JOY(8), DATA_NONE, 0},
                                 {"joy9", VK_JOY(9), GTA_KEY_JOY(9), DATA_NONE, 0},
                                 {"joy10", VK_JOY(10), GTA_KEY_JOY(10), DATA_NONE, 0},
                                 {"joy11", VK_JOY(11), GTA_KEY_JOY(11), DATA_NONE, 0},
                                 {"joy12", VK_JOY(12), GTA_KEY_JOY(12), DATA_NONE, 0},
                                 {"joy13", VK_JOY(13), GTA_KEY_JOY(13), DATA_NONE, 0},
                                 {"joy14", VK_JOY(14), GTA_KEY_JOY(14), DATA_NONE, 0},
                                 {"joy15", VK_JOY(15), GTA_KEY_JOY(15), DATA_NONE, 0},
                                 {"joy16", VK_JOY(16), GTA_KEY_JOY(16), DATA_NONE, 0},
                                 {"joy17", VK_JOY(17), GTA_KEY_JOY(17), DATA_NONE, 0},
                                 {"joy18", VK_JOY(18), GTA_KEY_JOY(18), DATA_NONE, 0},
                                 {"joy19", VK_JOY(19), GTA_KEY_JOY(19), DATA_NONE, 0},
                                 {"joy20", VK_JOY(20), GTA_KEY_JOY(20), DATA_NONE, 0},
                                 {"joy21", VK_JOY(21), GTA_KEY_JOY(21), DATA_NONE, 0},
                                 {"joy22", VK_JOY(22), GTA_KEY_JOY(22), DATA_NONE, 0},
                                 {"joy23", VK_JOY(23), GTA_KEY_JOY(23), DATA_NONE, 0},
                                 {"joy24", VK_JOY(24), GTA_KEY_JOY(24), DATA_NONE, 0},
                                 {"joy25", VK_JOY(25), GTA_KEY_JOY(25), DATA_NONE, 0},
                                 {"joy26", VK_JOY(26), GTA_KEY_JOY(26), DATA_NONE, 0},
                                 {"joy27", VK_JOY(27), GTA_KEY_JOY(27), DATA_NONE, 0},
                                 {"joy28", VK_JOY(28), GTA_KEY_JOY(28), DATA_NONE, 0},
                                 {"joy29", VK_JOY(29), GTA_KEY_JOY(29), DATA_NONE, 0},
                                 {"joy30", VK_JOY(30), GTA_KEY_JOY(30), DATA_NONE, 0},
                                 {"joy31", VK_JOY(31), GTA_KEY_JOY(31), DATA_NONE, 0},
                                 {"joy32", VK_JOY(32), GTA_KEY_JOY(32), DATA_NONE, 0},

                                 {"pov_up", VK_POV(1), NO_KEY_DEFINED, DATA_NONE, 0},
                                 {"pov_right", VK_POV(2), NO_KEY_DEFINED, DATA_NONE, 0},
                                 {"pov_down", VK_POV(3), NO_KEY_DEFINED, DATA_NONE, 0},
                                 {"pov_left", VK_POV(4), NO_KEY_DEFINED, DATA_NONE, 0},

                                 {"axis_1", VK_AXIS(1), NO_KEY_DEFINED, DATA_NONE, 0},
                                 {"axis_2", VK_AXIS(2), NO_KEY_DEFINED, DATA_NONE, 0},
                                 {"axis_3", VK_AXIS(3), NO_KEY_DEFINED, DATA_NONE, 0},
                                 {"axis_4", VK_AXIS(4), NO_KEY_DEFINED, DATA_NONE, 0},
                                 {"axis_5", VK_AXIS(5), NO_KEY_DEFINED, DATA_NONE, 0},
                                 {"axis_6", VK_AXIS(6), NO_KEY_DEFINED, DATA_NONE, 0},
                                 {"axis_7", VK_AXIS(7), NO_KEY_DEFINED, DATA_NONE, 0},
                                 {"axis_8", VK_AXIS(8), NO_KEY_DEFINED, DATA_NONE, 0},
                                 {"axis_9", VK_AXIS(9), NO_KEY_DEFINED, DATA_NONE, 0},
                                 {"axis_10", VK_AXIS(10), NO_KEY_DEFINED, DATA_NONE, 0},
                                 {"axis_11", VK_AXIS(11), NO_KEY_DEFINED, DATA_NONE, 0},
                                 {"axis_12", VK_AXIS(12), NO_KEY_DEFINED, DATA_NONE, 0},
                                 {"axis_13", VK_AXIS(13), NO_KEY_DEFINED, DATA_NONE, 0},
                                 {"axis_14", VK_AXIS(14), NO_KEY_DEFINED, DATA_NONE, 0},

                                 {"", 0, NO_KEY_DEFINED, DATA_NONE}};

SBindableGTAControl g_bcControls[] = {{"fire", FIRE, CONTROL_FOOT, false, true, _td("Fire")},
                                      {"next_weapon", NEXT_WEAPON, CONTROL_FOOT, false, true, _td("Next weapon")},
                                      {"previous_weapon", PREVIOUS_WEAPON, CONTROL_FOOT, false, true, _td("Previous weapon")},
                                      {"forwards", FORWARDS, CONTROL_FOOT, false, true, _td("Forwards")},
                                      {"backwards", BACKWARDS, CONTROL_FOOT, false, true, _td("Backwards")},
                                      {"left", LEFT, CONTROL_FOOT, false, true, _td("Left")},
                                      {"right", RIGHT, CONTROL_FOOT, false, true, _td("Right")},
                                      {"zoom_in", ZOOM_IN, CONTROL_FOOT, false, true, _td("Zoom in")},
                                      {"zoom_out", ZOOM_OUT, CONTROL_FOOT, false, true, _td("Zoom out")},
                                      {"enter_exit", ENTER_EXIT, CONTROL_BOTH, false, true, _td("Enter/Exit")},
                                      {"change_camera", CHANGE_CAMERA, CONTROL_BOTH, false, true, _td("Change camera")},            // 10
                                      {"jump", JUMP, CONTROL_FOOT, false, true, _td("Jump")},
                                      {"sprint", SPRINT, CONTROL_FOOT, false, true, _td("Sprint")},
                                      {"look_behind", LOOK_BEHIND, CONTROL_FOOT, false, true, _td("Look behind")},
                                      {"crouch", CROUCH, CONTROL_FOOT, false, true, _td("Crouch")},
                                      {"action", ACTION, CONTROL_FOOT, false, true, _td("Action")},
                                      {"walk", WALK, CONTROL_FOOT, false, true, _td("Walk")},
                                      {"vehicle_fire", VEHICLE_FIRE, CONTROL_VEHICLE, false, true, _td("Vehicle fire")},
                                      {"vehicle_secondary_fire", VEHICLE_SECONDARY_FIRE, CONTROL_VEHICLE, false, true, _td("Vehicle secondary fire")},
                                      {"vehicle_left", VEHICLE_LEFT, CONTROL_VEHICLE, false, true, _td("Vehicle left")},
                                      {"vehicle_right", VEHICLE_RIGHT, CONTROL_VEHICLE, false, true, _td("Vehicle right")},            // 20
                                      {"steer_forward", STEER_FORWARDS_DOWN, CONTROL_VEHICLE, false, true, _td("Steer forwards/down")},
                                      {"steer_back", STEER_BACK_UP, CONTROL_VEHICLE, false, true, _td("Steer backwards/up")},
                                      {"accelerate", ACCELERATE, CONTROL_VEHICLE, false, true, _td("Accelerate")},
                                      {"brake_reverse", BRAKE_REVERSE, CONTROL_VEHICLE, false, true, _td("Brake/Reverse")},
                                      {"radio_next", RADIO_NEXT, CONTROL_VEHICLE, false, true, _td("Radio next")},
                                      {"radio_previous", RADIO_PREVIOUS, CONTROL_VEHICLE, false, true, _td("Radio previous")},
                                      {"radio_user_track_skip", RADIO_USER_TRACK_SKIP, CONTROL_VEHICLE, false, true, _td("Radio user track skip")},
                                      {"horn", HORN, CONTROL_VEHICLE, false, true, _td("Horn")},
                                      {"sub_mission", SUB_MISSION, CONTROL_VEHICLE, false, true, _td("Sub-mission")},
                                      {"handbrake", HANDBRAKE, CONTROL_VEHICLE, false, true, _td("Handbrake")},            // 30
                                      {"vehicle_look_left", VEHICLE_LOOK_LEFT, CONTROL_VEHICLE, false, true, _td("Vehicle look left")},
                                      {"vehicle_look_right", VEHICLE_LOOK_RIGHT, CONTROL_VEHICLE, false, true, _td("Vehicle look right")},
                                      {"vehicle_look_behind", VEHICLE_LOOK_BEHIND, CONTROL_VEHICLE, false, true, _td("Vehicle look behind")},
                                      {"vehicle_mouse_look", VEHICLE_MOUSE_LOOK, CONTROL_VEHICLE, false, true, _td("Vehicle mouse look")},
                                      {"special_control_left", SPECIAL_CONTROL_LEFT, CONTROL_VEHICLE, false, true, _td("Special control left")},
                                      {"special_control_right", SPECIAL_CONTROL_RIGHT, CONTROL_VEHICLE, false, true, _td("Special control right")},
                                      {"special_control_down", SPECIAL_CONTROL_DOWN, CONTROL_VEHICLE, false, true, _td("Special control down")},
                                      {"special_control_up", SPECIAL_CONTROL_UP, CONTROL_VEHICLE, false, true, _td("Special control up")},
                                      {"aim_weapon", AIM_WEAPON, CONTROL_FOOT, false, true, _td("Aim weapon")},
                                      {"conversation_yes", CONVERSATION_YES, CONTROL_FOOT, false, true, _td("Conversation yes")},            // 40
                                      {"conversation_no", CONVERSATION_NO, CONTROL_FOOT, false, true, _td("Conversation no")},
                                      {"group_control_forwards", GROUP_CONTROL_FORWARDS, CONTROL_FOOT, false, true, _td("Group control forwards")},
                                      {"group_control_back", GROUP_CONTROL_BACK, CONTROL_FOOT, false, true, _td("Group control backwards")},

                                      {"", (eControllerAction)0, (eControlType)0, false, false}};

const SDefaultCommandBind g_dcbDefaultCommands[] = {{"g", true, "enter_passenger", NULL},
                                                    {"F12", true, "screenshot", NULL},
                                                    {"t", true, "chatbox", "chatboxsay"},
                                                    {"y", true, "chatbox", "teamsay 255 0 0"},
                                                    {"F11", true, "radar", "-1"},
                                                    {"num_add", true, "radar_zoom_in", NULL},
                                                    {"num_sub", true, "radar_zoom_out", NULL},
                                                    {"num_8", true, "radar_move_north", NULL},
                                                    {"num_2", true, "radar_move_south", NULL},
                                                    {"num_6", true, "radar_move_east", NULL},
                                                    {"num_4", true, "radar_move_west", NULL},
                                                    {"num_0", true, "radar_attach", NULL},
                                                    {"num_div", true, "radar_opacity_down", NULL},
                                                    {"num_mul", true, "radar_opacity_up", NULL},
                                                    {"num_1", true, "radar_help", NULL},
                                                    {"z", true, "voiceptt", "1"},
                                                    {"z", false, "voiceptt", "0"},
                                                    {"pgup", true, "chatscrollup", "1"},
                                                    {"pgup", false, "chatscrollup", "0"},
                                                    {"pgdn", true, "chatscrolldown", "-1"},
                                                    {"pgdn", false, "chatscrolldown", "0"},
                                                    {"pgup", true, "debugscrollup", "1"},
                                                    {"pgup", false, "debugscrollup", "0"},
                                                    {"pgdn", true, "debugscrolldown", "-1"},
                                                    {"pgdn", false, "debugscrolldown", "0"},

                                                    {"", false, NULL, NULL}};

static bool bindableKeyStates[std::size(g_bkKeys)];

// HACK: our current shift key states
bool bPreLeftShift = false, bPreRightShift = false;

enum eBindableKeys
{
    BK_MOUSE_WHEEL_UP = 5,
    BK_MOUSE_WHEEL_DOWN = 6,
};

static bool& GetBindableKeyState(const SBindableKey* key)
{
    intptr_t base = reinterpret_cast<intptr_t>(&g_bkKeys[0]);
    intptr_t offset = reinterpret_cast<intptr_t>(key);
    size_t   index = (offset - base) / sizeof(SBindableKey);
    return bindableKeyStates[index];
}

// Ensure zero length strings are NULL
static void NullEmptyStrings(const char*& a, const char*& b = *(const char**)NULL, const char*& c = *(const char**)NULL, const char*& d = *(const char**)NULL,
                             const char*& e = *(const char**)NULL)
{
    if (&a && a && a[0] == 0)
        a = NULL;
    if (&b && b && b[0] == 0)
        b = NULL;
    if (&c && c && c[0] == 0)
        c = NULL;
    if (&d && d && d[0] == 0)
        d = NULL;
    if (&e && e && e[0] == 0)
        e = NULL;
}

template <typename Container, typename Predicate, typename UnaryFunction>
bool ForEachBind(Container& binds, Predicate predicate, UnaryFunction action)
{
    if (binds.empty())
        return false;

    if (auto iter = std::find_if(binds.begin(), binds.end(), predicate); iter != binds.end())
    {
        while (iter != binds.end())
        {
            auto current = iter++;
            action(current);
            iter = std::find_if(iter, binds.end(), predicate);
        }

        return true;
    }

    return false;
}

template <typename Container, typename Predicate>
bool RemoveBinds(Container& binds, bool isContainerMutable, Predicate predicate)
{
    if (isContainerMutable)
        return ForEachBind(binds, predicate, [&binds](const auto& iter) { binds.erase(iter); });
    else
        return ForEachBind(binds, predicate, [](const auto& iter) { (*iter)->isBeingDeleted = true; });
}

template <typename Container>
bool RemoveBindTypeBinds(Container& binds, bool isContainerMutable, KeyBindType bindType)
{
    return RemoveBinds(binds, isContainerMutable, [bindType](const auto& bind) { return bind->type == bindType; });
}

bool CKeyBinds::ProcessMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (g_pCore->IsWebCoreLoaded() && !m_pCore->IsMenuVisible() && !m_pCore->GetConsole()->IsVisible() && !m_pCore->IsChatInputEnabled())
        g_pCore->GetWebCore()->ProcessInputMessage(uMsg, wParam, lParam);

    // Don't process Shift keys here, we have a hack for that
    if (wParam == 0x10 && (uMsg == WM_KEYDOWN || uMsg == WM_KEYUP || uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP))
        return false;

    bool                bState;
    const SBindableKey* pKey = GetBindableFromMessage(uMsg, wParam, lParam, bState);
    if (pKey)
        return ProcessKeyStroke(pKey, bState);

    if (uMsg == WM_CHAR)
        return ProcessCharacter(wParam);
    return false;
}

void CKeyBinds::OnLoseFocus()
{
    for (size_t i = 0; i < std::size(bindableKeyStates); ++i)
    {
        if (bindableKeyStates[i])
        {
            const SBindableKey* key = &g_bkKeys[i];
            ProcessKeyStroke(key, false);
        }
    }
}

bool CKeyBinds::ProcessCharacter(WPARAM wChar)
{
    return m_CharacterKeyHandler && m_CharacterKeyHandler(wChar);
}

bool CKeyBinds::ProcessKeyStroke(const SBindableKey* pKey, bool bState)
{
    m_bProcessingKeyStroke = true;
    // If the console, chat input or menu is up, ignore any messages and unset
    // any already pressed */
    static bool bInputGoesToGUI = false;
    static bool bIsCursorForced = false;
    if (m_pCore->IsMenuVisible() ||
        (m_pCore->GetConsole()->IsVisible() || m_pCore->IsChatInputEnabled() || m_pCore->GetGUI()->GetGUIInputEnabled()) && !pKey->bIgnoredByGUI)
    {
        if (!bInputGoesToGUI)
        {
            SetAllBindStates(false, KeyBindType::COMMAND);
            SetAllBindStates(false, KeyBindType::FUNCTION);
            bInputGoesToGUI = true;
        }
    }
    else
        bInputGoesToGUI = false;

    if (m_pCore->IsCursorForcedVisible())
    {
        if (!bIsCursorForced)
        {
            if (m_pCore->IsCursorControlsToggled())
            {
                SetAllControls(false);
            }
            bIsCursorForced = true;
        }
    }
    else
        bIsCursorForced = false;

    if (pKey->iGTARelative == GTA_KEY_MSCROLLUP || pKey->iGTARelative == GTA_KEY_MSCROLLDOWN)
        m_bMouseWheel = true;

    // Allow some keys to trigger onClientKey even when console input has focus
    bool bIsConsoleInputKey = true;
    if ((pKey->ulCode >= VK_F1 && pKey->ulCode <= VK_F12) || (pKey->ulCode <= VK_MBUTTON))
        bIsConsoleInputKey = false;

    bool& keyState = GetBindableKeyState(pKey);
    keyState = bState;

    bool bAllowed = TriggerKeyStrokeHandler(pKey->szKey, bState, bIsConsoleInputKey);

    // Search through binds
    bool                     wasBindFound = false;
    std::list<CCommandBind*> processedCommandBinds;

    auto wasCommandBindProcessed = [&processedCommandBinds](CCommandBind* commandBind) {
        auto iter = std::find_if(processedCommandBinds.begin(), processedCommandBinds.end(), [&](CCommandBind* processedCommandBind) {
            if (processedCommandBind->triggerState != commandBind->triggerState)
                return false;

            if (processedCommandBind->command != commandBind->command)
                return false;

            return commandBind->arguments.empty() || processedCommandBind->arguments == commandBind->arguments;
        });
        return iter != processedCommandBinds.end();
    };

    for (KeyBindPtr& bind : m_binds)
    {
        if (bind->isBeingDeleted || !bind->isActive || bind->boundKey != pKey)
            continue;

        switch (bind->type)
        {
            case KeyBindType::GTA_CONTROL:
            {
                if (bAllowed)
                {
                    if (!bState || (!bInputGoesToGUI && (!m_pCore->IsCursorForcedVisible() || !m_pCore->IsCursorControlsToggled())))
                    {
                        CallGTAControlBind(static_cast<CGTAControlBind*>(bind.get()), bState);
                        wasBindFound = true;
                    }
                }
                break;
            }
            case KeyBindType::COMMAND:
            case KeyBindType::FUNCTION:
            {
                auto bindWithState = static_cast<CKeyBindWithState*>(bind.get());

                if (bindWithState->state == bState)
                    break;

                wasBindFound = true;
                bindWithState->state = bState;

                // Does it match our up/down state?
                if (bindWithState->triggerState != bState)
                    break;

                if (bindWithState->type == KeyBindType::COMMAND)
                {
                    if (bInputGoesToGUI)
                        break;

                    auto commandBind = static_cast<CCommandBind*>(bind.get());

                    if (!bAllowed && commandBind->command != "screenshot")
                        break;

                    // HACK: Call chatbox commands on the next frame to stop a translated WM_CHAR key to pop up
                    if (commandBind->command == "chatbox")
                    {
                        m_pChatBoxBind = commandBind;
                        break;
                    }

                    if (!wasCommandBindProcessed(commandBind))
                    {
                        if (commandBind->wasCreatedByScript || processedCommandBinds.empty())
                            Call(commandBind);
                        else
                            m_vecBindQueue.push_back(commandBind);

                        processedCommandBinds.push_back(commandBind);
                    }
                }
                else            // bindWithState->type == KeyBindType::FUNCTION
                {
                    if (bAllowed)
                    {
                        auto functionBind = static_cast<CKeyFunctionBind*>(bind.get());

                        if (functionBind->handler && (!bInputGoesToGUI || functionBind->ignoresGUI))
                            functionBind->handler(functionBind);
                    }
                }
                break;
            }
            default:
                break;
        }
    }

    if (bAllowed)
    {
        // Check for pasting the clipboard
        if (bState && strcmp(pKey->szKey, "v") == 0 && (GetKeyState(VK_CONTROL) & 0x8000))
            OnPaste(SharedUtil::GetClipboardText());
    }

    m_bProcessingKeyStroke = false;
    RemoveDeletedBinds();
    return wasBindFound;
}

void CKeyBinds::Remove(CKeyBind* keyBind)
{
    const auto predicate = [keyBind](const KeyBindPtr& bind) { return bind.get() == keyBind; };

    if (auto iter = std::find_if(m_binds.begin(), m_binds.end(), predicate); iter != m_binds.end())
        Remove(iter);
}

void CKeyBinds::Remove(KeyBindContainer::iterator& iter)
{
    KeyBindPtr& keyBind = *iter;

    // If this is an active chatbox bind, delete it so it won't be called on next frame
    if (m_pChatBoxBind == keyBind.get())
        m_pChatBoxBind = nullptr;

    if (m_bProcessingKeyStroke)
        keyBind->isBeingDeleted = true;
    else
        m_binds.erase(iter);
}

void CKeyBinds::RemoveDeletedBinds()
{
    const auto predicate = [](const KeyBindPtr& bind) { return bind->isBeingDeleted; };
    m_binds.erase(std::remove_if(m_binds.begin(), m_binds.end(), predicate), m_binds.end());
}

void CKeyBinds::ClearCommandsAndControls()
{
    const auto predicate = [](const KeyBindPtr& bind) {
        return !bind->isBeingDeleted && bind->type != KeyBindType::FUNCTION && bind->type != KeyBindType::CONTROL_FUNCTION;
    };
    RemoveBinds(m_binds, !m_bProcessingKeyStroke, predicate);
}

bool CKeyBinds::Call(CKeyBind* keyBind)
{
    if (!keyBind)
        return false;

    switch (keyBind->type)
    {
        case KeyBindType::COMMAND:
        {
            auto commandBind = static_cast<CCommandBind*>(keyBind);
            if (commandBind->isActive)
            {
                const char* arguments = commandBind->arguments.empty() ? nullptr : commandBind->arguments.c_str();
                m_pCore->GetCommands()->Execute(commandBind->command.c_str(), arguments, /*bHandleRemotely=*/false, !commandBind->resource.empty());
            }
            break;
        }
        case KeyBindType::FUNCTION:
        {
            auto functionBind = static_cast<CKeyFunctionBind*>(keyBind);
            if (functionBind->handler)
                functionBind->handler(functionBind);
            break;
        }
        case KeyBindType::CONTROL_FUNCTION:
        {
            auto functionBind = static_cast<CControlFunctionBind*>(keyBind);
            if (functionBind->handler)
                functionBind->handler(functionBind);
            break;
        }
        default:
            return false;
    }

    return true;
}

bool CKeyBinds::AddCommand(const char* szKey, const char* szCommand, const char* szArguments, bool bState, const char* szResource, bool bScriptCreated,
                           const char* szOriginalScriptKey)
{
    NullEmptyStrings(szCommand, szArguments, szResource);

    if (!szKey || !szCommand)
        return false;

    const SBindableKey* bindableKey = GetBindableFromKey(szKey);

    if (!bindableKey)
        return false;

    if (szResource && bScriptCreated)
    {
        // Check if there is a waiting replacement
        CCommandBind* pUserAddedBind = FindCommandMatch(NULL, szCommand, szArguments, szResource, szKey, true, bState, true, false);
        if (pUserAddedBind)
        {
            // Upgrade
            pUserAddedBind->wasCreatedByScript = true;
            pUserAddedBind->isReplacingScriptKey = true;
            assert(pUserAddedBind->originalScriptKey == szKey);
            return true;
        }
    }

    auto bind = std::make_unique<CCommandBind>();
    bind->boundKey = bindableKey;
    bind->command = szCommand;
    bind->triggerState = bState;

    if (szArguments)
        bind->arguments = szArguments;

    if (szResource)
    {
        bind->resource = szResource;
        bind->wasCreatedByScript = bScriptCreated;

        if (bScriptCreated)
            bind->originalScriptKey = szKey;
        else if (szOriginalScriptKey)
            bind->originalScriptKey = szOriginalScriptKey;            // Will wait for script to addcommand before doing replace
    }

    m_binds.emplace_back(bind.release());
    return true;
}

bool CKeyBinds::AddCommand(const SBindableKey* pKey, const char* szCommand, const char* szArguments, bool bState)
{
    NullEmptyStrings(szCommand, szArguments);

    if (!pKey || !szCommand)
        return false;

    auto bind = std::make_unique<CCommandBind>();
    bind->boundKey = pKey;
    bind->command = szCommand;
    bind->triggerState = bState;

    if (szArguments)
        bind->arguments = szArguments;

    m_binds.emplace_back(bind.release());
    return true;
}

bool CKeyBinds::RemoveCommand(const char* szKey, const char* szCommand, bool bCheckState, bool bState)
{
    if (!szKey || !szCommand)
        return false;

    std::string_view command{szCommand};

    const auto predicate = [&](const KeyBindPtr& bind) {
        if (bind->isBeingDeleted || bind->type != KeyBindType::COMMAND)
            return false;

        if (stricmp(szKey, bind->boundKey->szKey))
            return false;

        auto commandBind = static_cast<const CCommandBind*>(bind.get());

        if (!commandBind->resource.empty() || commandBind->command != command)
            return false;

        return !bCheckState || commandBind->triggerState == bState;
    };

    return ForEachBind(m_binds, predicate, [this](KeyBindContainer::iterator& iter) { Remove(iter); });
}

bool CKeyBinds::RemoveAllCommands(const char* szKey, bool bCheckState, bool bState)
{
    if (!szKey)
        return false;

    const auto predicate = [&](const KeyBindPtr& bind) {
        if (bind->isBeingDeleted || bind->type != KeyBindType::COMMAND)
            return false;

        if (stricmp(szKey, bind->boundKey->szKey))
            return false;

        auto commandBind = static_cast<CCommandBind*>(bind.get());
        return !bCheckState || commandBind->triggerState == bState;
    };

    return ForEachBind(m_binds, predicate, [this](KeyBindContainer::iterator& iter) { Remove(iter); });
}

bool CKeyBinds::RemoveAllCommands()
{
    const auto predicate = [](const KeyBindPtr& bind) { return !bind->isBeingDeleted && bind->type == KeyBindType::COMMAND; };
    return ForEachBind(m_binds, predicate, [this](KeyBindContainer::iterator& iter) { Remove(iter); });
}

bool CKeyBinds::CommandExists(const char* szKey, const char* szCommand, bool bCheckState, bool bState, const char* szArguments, const char* szResource,
                              bool bCheckScriptCreated, bool bScriptCreated)
{
    const char* szOriginalScriptKey = NULL;
    if (bCheckScriptCreated && bScriptCreated)
    {
        // If looking for script created command, check original key instead of current key
        szOriginalScriptKey = szKey;
        szKey = NULL;
    }

    if (FindCommandMatch(szKey, szCommand, szArguments, szResource, szOriginalScriptKey, bCheckState, bState, bCheckScriptCreated, bScriptCreated))
        return true;

    return false;
}

bool CKeyBinds::SetCommandActive(const char* szKey, const char* szCommand, bool bState, const char* szArguments, const char* szResource, bool bActive,
                                 bool checkHitState, bool bConsiderDefaultKey)
{
    NullEmptyStrings(szKey, szCommand, szArguments);

    for (KeyBindPtr& bind : m_binds)
    {
        if (bind->isBeingDeleted || bind->type != KeyBindType::COMMAND)
            continue;

        auto commandBind = static_cast<CCommandBind*>(bind.get());

        if (szKey && stricmp(szKey, commandBind->boundKey->szKey) &&
            !(bConsiderDefaultKey && commandBind->isReplacingScriptKey && !stricmp(szKey, commandBind->originalScriptKey.c_str())))
            continue;

        if (commandBind->resource != szResource)
            continue;

        if (szCommand && commandBind->command != szCommand)
            continue;

        if (checkHitState && commandBind->triggerState != bState)
            continue;

        if (szArguments && commandBind->arguments != szArguments)
            continue;

        commandBind->isActive = true;
        return true;
    }

    return false;
}

void CKeyBinds::SetAllCommandsActive(const char* szResource, bool bActive, const char* szCommand, bool bState, const char* szArguments, bool checkHitState,
                                     const char* szOnlyWithDefaultKey)
{
    NullEmptyStrings(szCommand, szArguments);

    for (KeyBindPtr& bind : m_binds)
    {
        if (bind->isBeingDeleted || bind->type != KeyBindType::COMMAND)
            continue;

        auto commandBind = static_cast<CCommandBind*>(bind.get());

        if (szOnlyWithDefaultKey && !(commandBind->isReplacingScriptKey && !stricmp(szOnlyWithDefaultKey, commandBind->originalScriptKey.c_str())) &&
            stricmp(szOnlyWithDefaultKey, commandBind->boundKey->szKey))
            continue;

        if (commandBind->resource != szResource)
            continue;

        if (szCommand && commandBind->command != szCommand)
            continue;

        if (checkHitState && commandBind->triggerState != bState)
            continue;

        if (szArguments && commandBind->arguments != szArguments)
            continue;

        commandBind->isActive = bActive;
    }
}

CCommandBind* CKeyBinds::GetBindFromCommand(const char* szCommand, const char* szArguments, bool bMatchCase, const char* szKey, bool bCheckHitState,
                                            bool bState)
{
    NullEmptyStrings(szKey, szArguments);

    for (KeyBindPtr& bind : m_binds)
    {
        if (bind->isBeingDeleted || bind->type != KeyBindType::COMMAND)
            continue;

        auto commandBind = static_cast<CCommandBind*>(bind.get());

        if (szKey && stricmp(szKey, commandBind->boundKey->szKey))
            continue;

        if (bMatchCase && commandBind->command != szCommand)
            continue;

        if (!bMatchCase && stricmp(szCommand, commandBind->command.c_str()))
            continue;

        if (szArguments)
        {
            if (bMatchCase && commandBind->arguments != szArguments)
                continue;

            if (!bMatchCase && stricmp(szArguments, commandBind->arguments.c_str()))
                continue;
        }

        if (bCheckHitState && commandBind->triggerState != bState)
            continue;

        return commandBind;
    }

    return nullptr;
}

CCommandBind* CKeyBinds::FindCommandMatch(const char* szKey, const char* szCommand, const char* szArguments, const char* szResource,
                                          const char* szOriginalScriptKey, bool bCheckState, bool bState, bool bCheckScriptCreated, bool bScriptCreated)
{
    NullEmptyStrings(szKey, szArguments, szResource, szOriginalScriptKey);

    std::string arguments = szArguments ? szArguments : "";
    szArguments = SharedUtil::Trim(arguments.data());

    for (KeyBindPtr& bind : m_binds)
    {
        if (bind->isBeingDeleted || bind->type != KeyBindType::COMMAND)
            continue;

        auto commandBind = static_cast<CCommandBind*>(bind.get());

        if (szKey && stricmp(szKey, commandBind->boundKey->szKey))
            continue;

        if (szArguments && commandBind->arguments != szArguments)
            continue;

        if (commandBind->command != szCommand)
            continue;

        if (szResource && commandBind->resource != szResource)
            continue;

        if (bCheckScriptCreated && commandBind->wasCreatedByScript != bScriptCreated)
            continue;

        if (szOriginalScriptKey && commandBind->originalScriptKey != szOriginalScriptKey)
            continue;

        if (bCheckState && commandBind->triggerState != bState)
            continue;

        return commandBind;
    }

    return nullptr;
}

//
// Get up-bind version of down-bind
//
CCommandBind* CKeyBinds::FindMatchingUpBind(CCommandBind* pBind)
{
    const char* szResource = pBind->resource.empty() ? nullptr : pBind->resource.c_str();
    return FindCommandMatch(pBind->boundKey->szKey, pBind->command.c_str(), /*arguments=*/nullptr, szResource, pBind->originalScriptKey.c_str(),
                            /*bCheckState=*/true, /*bState=*/false, /*bCheckScriptCreated=*/true, pBind->wasCreatedByScript);
}

//
// Called when the user removes an existing command bind
//
void CKeyBinds::UserRemoveCommandBoundKey(CCommandBind* pBind)
{
    if (pBind->wasCreatedByScript)
    {
        if (pBind->isReplacingScriptKey)
        {
            // Remove user added key change
            pBind->isReplacingScriptKey = false;
            const SBindableKey* boundKey = GetBindableFromKey(pBind->originalScriptKey.c_str());
            if (boundKey)
                pBind->boundKey = boundKey;
        }
        // Can't manually remove script added command binds, as they will reappear when the resource re-starts
        return;
    }
    Remove(pBind);
}

//
// Called when the user changes the key of an existing command bind
//
void CKeyBinds::UserChangeCommandBoundKey(CCommandBind* pBind, const SBindableKey* pNewBoundKey)
{
    if (pBind->wasCreatedByScript)
    {
        // Set replacing flag if was script bind (and key is different)
        if (pBind->originalScriptKey != pNewBoundKey->szKey)
            pBind->isReplacingScriptKey = true;
        else
            pBind->isReplacingScriptKey = false;
    }
    // Do change
    pBind->boundKey = pNewBoundKey;
}

//
// Sort command binds for consistency in settings gui
//
void CKeyBinds::SortCommandBinds()
{
    auto compare = [](const KeyBindPtr& lhs, const KeyBindPtr& rhs) {
        // Group command binds last
        if (lhs->type != KeyBindType::COMMAND && rhs->type == KeyBindType::COMMAND)
            return true;
        if (lhs->type != KeyBindType::COMMAND || rhs->type != KeyBindType::COMMAND)
            return false;

        auto lhsCommand = static_cast<const CCommandBind*>(lhs.get());
        auto rhsCommand = static_cast<const CCommandBind*>(rhs.get());

        // Command binds with resource name last
        if (lhsCommand->resource.empty() && !rhsCommand->resource.empty())
            return true;
        if (!lhsCommand->resource.empty() && rhsCommand->resource.empty())
            return false;

        // Alpha sort by resource name
        if (int result = stricmp(lhsCommand->resource.c_str(), rhsCommand->resource.c_str()); result != 0)
            return result < 0;

        // Script added before user added
        if (lhsCommand->wasCreatedByScript && !rhsCommand->wasCreatedByScript)
            return true;
        if (!lhsCommand->wasCreatedByScript && rhsCommand->wasCreatedByScript)
            return false;

        // Replacers before norms
        return !lhsCommand->originalScriptKey.empty() && rhsCommand->originalScriptKey.empty();
    };

    m_binds.sort(compare);
}

bool CKeyBinds::GetBoundCommands(const char* szCommand, std::list<CCommandBind*>& commandsList)
{
    std::size_t sizeBefore = commandsList.size();

    for (KeyBindPtr& bind : m_binds)
    {
        if (bind->isBeingDeleted || bind->type != KeyBindType::COMMAND)
            continue;

        auto commandBind = static_cast<CCommandBind*>(bind.get());

        if (!stricmp(commandBind->command.c_str(), szCommand))
            commandsList.push_back(commandBind);
    }

    return commandsList.size() != sizeBefore;
}

bool CKeyBinds::AddGTAControl(const char* szKey, const char* szControl)
{
    if (!szKey || !szControl)
        return false;

    const SBindableKey* bindableKey = GetBindableFromKey(szKey);

    if (!bindableKey)
        return false;

    SBindableGTAControl* bindableControl = GetBindableFromControl(szControl);

    if (!bindableControl)
        return false;

    auto bind = std::make_unique<CGTAControlBind>();
    bind->boundKey = bindableKey;
    bind->control = bindableControl;
    m_binds.emplace_back(bind.release());
    return true;
}

bool CKeyBinds::AddGTAControl(const SBindableKey* pKey, SBindableGTAControl* pControl)
{
    if (!pKey || !pControl)
        return false;

    auto bind = std::make_unique<CGTAControlBind>();
    bind->boundKey = pKey;
    bind->control = pControl;
    m_binds.emplace_back(bind.release());
    return true;
}

bool CKeyBinds::RemoveGTAControl(const char* szKey, const char* szControl)
{
    if (!szKey || !szControl)
        return false;

    const auto predicate = [&](const KeyBindPtr& bind) {
        if (bind->isBeingDeleted || bind->type != KeyBindType::GTA_CONTROL)
            return false;

        auto controlBind = static_cast<CGTAControlBind*>(bind.get());
        return !stricmp(szKey, controlBind->boundKey->szKey) && !strcmp(szControl, controlBind->control->szControl);
    };

    return RemoveBinds(m_binds, !m_bProcessingKeyStroke, predicate);
}

bool CKeyBinds::RemoveAllGTAControls(const char* szKey)
{
    if (!szKey)
        return false;

    const auto predicate = [&](const KeyBindPtr& bind) {
        if (bind->isBeingDeleted || bind->type != KeyBindType::GTA_CONTROL)
            return false;

        auto controlBind = static_cast<CGTAControlBind*>(bind.get());
        return !stricmp(szKey, controlBind->boundKey->szKey);
    };

    return RemoveBinds(m_binds, !m_bProcessingKeyStroke, predicate);
}

bool CKeyBinds::RemoveAllGTAControls()
{
    return RemoveBindTypeBinds(m_binds, !m_bProcessingKeyStroke, KeyBindType::GTA_CONTROL);
}

bool CKeyBinds::GTAControlExists(const char* szKey, const char* szControl)
{
    const SBindableKey*  bindableKey = GetBindableFromKey(szKey);
    SBindableGTAControl* bindableControl = GetBindableFromControl(szControl);
    return GTAControlExists(bindableKey, bindableControl);
}

bool CKeyBinds::GTAControlExists(const SBindableKey* pKey, SBindableGTAControl* pControl)
{
    for (const KeyBindPtr& bind : m_binds)
    {
        if (bind->isBeingDeleted || bind->type != KeyBindType::GTA_CONTROL)
            continue;

        auto controlBind = static_cast<CGTAControlBind*>(bind.get());

        if (controlBind->boundKey != pKey || controlBind->control != pControl)
            continue;

        return true;
    }

    return false;
}

void CKeyBinds::CallGTAControlBind(CGTAControlBind* pBind, bool bState)
{
    if (!pBind)
        return;

    // Don't allow custom track skips. Crashes for some reason.
    if (pBind->control->action == RADIO_USER_TRACK_SKIP)
        return;

    // Set this binds state
    pBind->state = bState;

    // If its keydown, or there isnt another bind for this control down
    if (bState || !GetMultiGTAControlState(pBind))
    {
        // HACK: Are we're trying to ZOOM IN or ZOOM OUT using the mouse wheel?
        bool bZoomWheel = m_bMouseWheel && (pBind->control->action == ZOOM_IN || pBind->control->action == ZOOM_OUT);

        // If this control is enabled
        // HACK: Prevent the game from using the mouse wheel to zoom *again* (it still does this itself)
        if (pBind->control->bEnabled && !bZoomWheel)
        {
            pBind->control->bState = bState;
        }

        for (KeyBindPtr& bind : m_binds)
        {
            if (bind->isBeingDeleted || bind->type != KeyBindType::CONTROL_FUNCTION)
                continue;

            auto functionBind = static_cast<CControlFunctionBind*>(bind.get());

            if (functionBind->control != pBind->control)
                continue;

            // If its a key up or our player's state matches the control
            eControlType controlType = functionBind->control->controlType;

            if (bState)
            {
                if ((controlType == CONTROL_FOOT && m_bInVehicle) || (controlType == CONTROL_VEHICLE && !m_bInVehicle))
                    continue;
            }

            if (functionBind->state != bState && functionBind->triggerState == bState)
                Call(functionBind);

            functionBind->state = bState;
        }
    }
}

void CKeyBinds::CallAllGTAControlBinds(eControlType controlType, bool bState)
{
    for (KeyBindPtr& bind : m_binds)
    {
        if (bind->isBeingDeleted || bind->type != KeyBindType::GTA_CONTROL)
            continue;

        auto controlBind = static_cast<CGTAControlBind*>(bind.get());

        if (controlType == CONTROL_BOTH || controlBind->control->controlType == controlType)
            CallGTAControlBind(controlBind, bState);
    }
}

bool CKeyBinds::GetMultiGTAControlState(CGTAControlBind* pBind)
{
    eControllerAction action = pBind->control->action;

    for (KeyBindPtr& bind : m_binds)
    {
        if (bind->isBeingDeleted || bind->type != KeyBindType::GTA_CONTROL)
            continue;

        auto controlBind = reinterpret_cast<CGTAControlBind*>(bind.get());

        if (controlBind == pBind || controlBind->control->action != action)
            continue;

        if (controlBind->state)
            return true;
    }

    return false;
}

bool CKeyBinds::IsControlEnabled(const char* szControl)
{
    SBindableGTAControl* pControl = GetBindableFromControl(szControl);
    if (pControl)
    {
        return pControl->bEnabled;
    }
    else
    {
        COMMANDENTRY* pCommand = m_pCore->GetCommands()->Get(szControl, true, true);
        if (pCommand)
        {
            return pCommand->bEnabled;
        }
    }

    return false;
}

bool CKeyBinds::SetControlEnabled(const char* szControl, bool bEnabled)
{
    SBindableGTAControl* pControl = GetBindableFromControl(szControl);
    if (pControl)
    {
        if (pControl->bEnabled != bEnabled)
        {
            pControl->bEnabled = bEnabled;

            if (bEnabled)
                ResetGTAControlState(pControl);
            else
                pControl->bState = false;

            return true;
        }
    }

    COMMANDENTRY* pCommand = m_pCore->GetCommands()->Get(szControl, true, true);
    if (pCommand)
    {
        pCommand->bEnabled = bEnabled;
        return true;
    }

    return false;
}

void CKeyBinds::SetAllControlsEnabled(bool bGameControls, bool bMTAControls, bool bEnabled)
{
    if (bGameControls)
    {
        for (int i = 0; *g_bcControls[i].szControl != NULL; i++)
        {
            SBindableGTAControl* temp = &g_bcControls[i];

            temp->bEnabled = bEnabled;

            if (!bEnabled)
                temp->bState = false;
        }
        if (bEnabled)
        {
            ResetAllGTAControlStates();
        }
    }
    if (bMTAControls)
    {
        std::list<COMMANDENTRY*>::iterator iter = m_pCore->GetCommands()->IterBegin();
        for (; iter != m_pCore->GetCommands()->IterEnd(); iter++)
        {
            if ((*iter)->bModCommand)
            {
                (*iter)->bEnabled = bEnabled;
            }
        }
    }
}

void CKeyBinds::ResetGTAControlState(SBindableGTAControl* pControl)
{
    // Reset the control state
    pControl->bState = false;

    // Search for a pressed bind for this control and set the control state accordingly
    for (KeyBindPtr& bind : m_binds)
    {
        if (bind->isBeingDeleted || bind->type != KeyBindType::GTA_CONTROL)
            continue;

        auto controlBind = static_cast<CGTAControlBind*>(bind.get());

        if (controlBind->control == pControl && controlBind->state)
        {
            pControl->bState = true;
            return;
        }
    }
}

void CKeyBinds::ResetAllGTAControlStates()
{
    // Set all our control states to false
    for (int i = 0; *g_bcControls[i].szControl != NULL; i++)
        g_bcControls[i].bState = false;

    // Go through all our control binds, if they're state is true, turn their control on
    for (KeyBindPtr& bind : m_binds)
    {
        if (bind->isBeingDeleted || bind->type != KeyBindType::GTA_CONTROL)
            continue;

        auto controlBind = static_cast<CGTAControlBind*>(bind.get());

        if (controlBind->state)
            controlBind->control->bState = true;
    }
}

bool CKeyBinds::GetBoundControls(SBindableGTAControl* pControl, std::list<CGTAControlBind*>& controlsList)
{
    for (KeyBindPtr& bind : m_binds)
    {
        if (bind->isBeingDeleted || bind->type != KeyBindType::GTA_CONTROL)
            continue;

        auto controlBind = static_cast<CGTAControlBind*>(bind.get());

        if (controlBind->control == pControl)
            controlsList.push_back(controlBind);
    }

    return true;
}

bool CKeyBinds::AddFunction(const char* szKey, KeyFunctionBindHandler Handler, bool bState, bool bIgnoreGUI)
{
    if (!szKey)
        return false;

    const SBindableKey* bindableKey = GetBindableFromKey(szKey);
    return AddFunction(bindableKey, Handler, bState, bIgnoreGUI);
}

bool CKeyBinds::AddFunction(const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bState, bool bIgnoreGUI)
{
    if (!pKey || !Handler)
        return false;

    auto bind = std::make_unique<CKeyFunctionBind>();
    bind->boundKey = pKey;
    bind->triggerState = bState;
    bind->handler = Handler;
    bind->ignoresGUI = bIgnoreGUI;
    m_binds.emplace_back(bind.release());
    return true;
}

bool CKeyBinds::RemoveFunction(const char* szKey, KeyFunctionBindHandler Handler, bool bCheckState, bool bState)
{
    if (const SBindableKey* bindableKey = GetBindableFromKey(szKey); bindableKey != nullptr)
        return RemoveFunction(bindableKey, Handler, bCheckState, bState);

    return false;
}

bool CKeyBinds::RemoveFunction(const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bCheckState, bool bState)
{
    const auto predicate = [&](const KeyBindPtr& bind) {
        if (bind->isBeingDeleted || bind->type != KeyBindType::FUNCTION)
            return false;

        auto functionBind = static_cast<const CKeyFunctionBind*>(bind.get());

        if (bCheckState && functionBind->triggerState != bState)
            return false;

        return functionBind->handler == Handler && functionBind->boundKey == pKey;
    };

    return RemoveBinds(m_binds, !m_bProcessingKeyStroke, predicate);
}

bool CKeyBinds::RemoveAllFunctions(KeyFunctionBindHandler Handler)
{
    const auto predicate = [&](const KeyBindPtr& bind) {
        if (bind->isBeingDeleted || bind->type != KeyBindType::FUNCTION)
            return false;

        auto functionBind = static_cast<const CKeyFunctionBind*>(bind.get());
        return functionBind->handler == Handler;
    };

    return RemoveBinds(m_binds, !m_bProcessingKeyStroke, predicate);
}

bool CKeyBinds::RemoveAllFunctions()
{
    return RemoveBindTypeBinds(m_binds, !m_bProcessingKeyStroke, KeyBindType::FUNCTION);
}

bool CKeyBinds::FunctionExists(const char* szKey, KeyFunctionBindHandler Handler, bool bCheckState, bool bState)
{
    if (const SBindableKey* bindableKey = GetBindableFromKey(szKey); bindableKey != nullptr)
        return FunctionExists(bindableKey, Handler, bCheckState, bState);

    return false;
}

bool CKeyBinds::FunctionExists(const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bCheckState, bool bState)
{
    for (const KeyBindPtr& bind : m_binds)
    {
        if (bind->isBeingDeleted || bind->type != KeyBindType::FUNCTION)
            continue;

        auto functionBind = static_cast<const CKeyFunctionBind*>(bind.get());

        if (Handler && functionBind->handler != Handler)
            continue;

        if (bCheckState && functionBind->triggerState != bState)
            continue;

        if (functionBind->boundKey == pKey)
            return true;
    }

    return false;
}

bool CKeyBinds::AddControlFunction(const char* szControl, ControlFunctionBindHandler Handler, bool bState)
{
    if (!szControl || !Handler)
        return false;

    SBindableGTAControl* bindableControl = GetBindableFromControl(szControl);

    if (!bindableControl)
        return false;

    auto bind = std::make_unique<CControlFunctionBind>();
    bind->control = bindableControl;
    bind->handler = Handler;
    bind->triggerState = bState;
    m_binds.emplace_back(bind.release());
    return true;
}

bool CKeyBinds::AddControlFunction(SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bState)
{
    if (!Handler || !pControl)
        return false;

    auto bind = std::make_unique<CControlFunctionBind>();
    bind->control = pControl;
    bind->handler = Handler;
    bind->triggerState = bState;
    m_binds.emplace_back(bind.release());
    return true;
}

bool CKeyBinds::RemoveControlFunction(const char* szControl, ControlFunctionBindHandler Handler, bool bCheckState, bool bState)
{
    if (SBindableGTAControl* bindableControl = GetBindableFromControl(szControl); bindableControl != nullptr)
        return RemoveControlFunction(bindableControl, Handler, bCheckState, bState);

    return false;
}

bool CKeyBinds::RemoveControlFunction(SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bCheckState, bool bState)
{
    const auto predicate = [&](const KeyBindPtr& bind) {
        if (bind->isBeingDeleted || bind->type != KeyBindType::CONTROL_FUNCTION)
            return false;

        auto functionBind = static_cast<const CControlFunctionBind*>(bind.get());

        if (bCheckState && functionBind->triggerState != bState)
            return false;

        return functionBind->handler == Handler && functionBind->control == pControl;
    };

    return RemoveBinds(m_binds, !m_bProcessingKeyStroke, predicate);
}

bool CKeyBinds::RemoveAllControlFunctions(ControlFunctionBindHandler Handler)
{
    const auto predicate = [&](const KeyBindPtr& bind) {
        if (bind->isBeingDeleted || bind->type != KeyBindType::CONTROL_FUNCTION)
            return false;

        auto functionBind = static_cast<const CControlFunctionBind*>(bind.get());
        return functionBind->handler == Handler;
    };

    return RemoveBinds(m_binds, !m_bProcessingKeyStroke, predicate);
}

bool CKeyBinds::RemoveAllControlFunctions()
{
    return RemoveBindTypeBinds(m_binds, !m_bProcessingKeyStroke, KeyBindType::CONTROL_FUNCTION);
}

bool CKeyBinds::ControlFunctionExists(const char* szControl, ControlFunctionBindHandler Handler, bool bCheckState, bool bState)
{
    if (SBindableGTAControl* bindableControl = GetBindableFromControl(szControl); bindableControl != nullptr)
        return ControlFunctionExists(bindableControl, Handler, bCheckState, bState);

    return false;
}

bool CKeyBinds::ControlFunctionExists(SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bCheckState, bool bState)
{
    for (const KeyBindPtr& bind : m_binds)
    {
        if (bind->isBeingDeleted || bind->type != KeyBindType::CONTROL_FUNCTION)
            continue;

        auto functionBind = static_cast<const CControlFunctionBind*>(bind.get());

        if (bCheckState && functionBind->triggerState != bState)
            continue;

        if (functionBind->handler == Handler && functionBind->control == pControl)
            return true;
    }

    return false;
}

const SBindableKey* CKeyBinds::GetBindableFromKey(const char* szKey) const
{
    for (int i = 0; *g_bkKeys[i].szKey != 0; i++)
    {
        const SBindableKey* temp = &g_bkKeys[i];

        if (!stricmp(temp->szKey, szKey))
            return temp;
    }

    return nullptr;
}

SBindableGTAControl* CKeyBinds::GetBindableFromAction(eControllerAction action)
{
    for (int i = 0; *g_bcControls[i].szControl != NULL; i++)
    {
        SBindableGTAControl* temp = &g_bcControls[i];

        if (temp->action == action)
            return temp;
    }

    return NULL;
}

bool CKeyBinds::IsKey(const char* szKey)
{
    for (int i = 0; *g_bkKeys[i].szKey != NULL; i++)
    {
        const SBindableKey* temp = &g_bkKeys[i];

        if (!stricmp(temp->szKey, szKey))
            return true;
    }

    return false;
}

const SBindableKey* CKeyBinds::GetBindableFromMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bState)
{
    if (uMsg != WM_KEYDOWN && uMsg != WM_KEYUP && uMsg != WM_SYSKEYDOWN && uMsg != WM_SYSKEYUP && uMsg != WM_LBUTTONDOWN && uMsg != WM_LBUTTONUP &&
        uMsg != WM_RBUTTONDOWN && uMsg != WM_RBUTTONUP && uMsg != WM_MBUTTONDOWN && uMsg != WM_MBUTTONUP && uMsg != WM_XBUTTONDOWN && uMsg != WM_XBUTTONUP &&
        uMsg != WM_MOUSEWHEEL)
        return NULL;

    // Prevents a simulated lctrl with Alt Gr keys
    if (IsFakeCtrl_L(uMsg, wParam, lParam))
        return NULL;

    bool bFirstHit = (lParam & 0x40000000) ? false : true;
    if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP || uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONUP || uMsg == WM_MOUSEWHEEL)
        bFirstHit = true;

    bool bExtended = (lParam & 0x1000000) ? true : false;
    bool bNumLock = (GetKeyState(VK_NUMLOCK) & 0x1) ? true : false;

    bState = (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN || uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN ||
              uMsg == WM_MOUSEWHEEL || uMsg == WM_XBUTTONDOWN);

    for (int i = 0; *g_bkKeys[i].szKey != NULL; i++)
    {
        const SBindableKey* bindable = &g_bkKeys[i];

        unsigned long ulCode = bindable->ulCode;
        eKeyData      keyData = bindable->data;

        bool bMouseWheel = false;
        if (ulCode == 0 && uMsg == WM_MOUSEWHEEL)
        {
            bool bMouseWheelUp = ((short)HIWORD(wParam) > 0);
            if ((bMouseWheelUp && bindable->iGTARelative == GTA_KEY_MSCROLLUP) || (!bMouseWheelUp && bindable->iGTARelative == GTA_KEY_MSCROLLDOWN))
            {
                bMouseWheel = true;
            }
        }

        bool bMouseXButton = false;
        if (ulCode == 0 && (uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONUP))
        {
            bool bMouseX1 = (HIWORD(wParam) == XBUTTON1);
            if ((bMouseX1 && bindable->iGTARelative == GTA_KEY_MXB1) || (!bMouseX1 && bindable->iGTARelative == GTA_KEY_MXB2))
            {
                bMouseXButton = true;
            }
        }

        if ((ulCode == wParam && (uMsg == WM_KEYDOWN || uMsg == WM_KEYUP || uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP)) ||
            (ulCode == 0x01 && (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP)) || (ulCode == 0x02 && (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP)) ||
            (ulCode == 0x04 && (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP)) || (bMouseWheel) || (bMouseXButton))
        {
            // Does it match the extended value?
            if (keyData == DATA_NONE || keyData == DATA_NUMPAD || (((keyData == DATA_EXTENDED) ? true : false) == bExtended))
            {
                // If its the first hit, or key up
                if (!bState || bFirstHit)
                {
                    // Is this a shift key
                    if (bindable->iGTARelative == GTA_KEY_LSHIFT)
                    {
                        // If our current rshift state is opposite to bState
                        if (bPreRightShift != bState)
                        {
                            // If our actual rshift state is the same as bState
                            if (((GetAsyncKeyState(VK_RSHIFT) & 0x8000) ? true : false) == bState)
                            {
                                // Return the right-shift bindable instead
                                return &g_bkKeys[10];
                            }
                        }
                    }

                    // Is num-lock off and breaking our key-codes?
                    if (!bNumLock && !bExtended && bindable->data == DATA_NUMPAD)
                    {
                        // Grab the correct key
                        if (!bindable->ucNumpadRelative)
                            return NULL;

                        // m_pCore->GetConsole ()->Printf ( "returned: %s", g_bkKeys [ bindable->ucNumpadRelative ].szKey );
                        return &g_bkKeys[bindable->ucNumpadRelative];
                    }

                    return bindable;
                }
            }
        }
    }
    return NULL;
}

bool CKeyBinds::GetKeyStateByName(const char* keyName, bool& state) const
{
    if (const SBindableKey* bindableKey = GetBindableFromKey(keyName); bindableKey != nullptr)
    {
        state = GetBindableKeyState(bindableKey);
        return true;
    }

    return false;
}

SBindableGTAControl* CKeyBinds::GetBindableFromControl(const char* szControl)
{
    for (int i = 0; *g_bcControls[i].szControl != NULL; i++)
    {
        SBindableGTAControl* temp = &g_bcControls[i];

        if (!stricmp(temp->szControl, szControl))
            return temp;
    }

    return NULL;
}

const SBindableKey* CKeyBinds::GetBindableFromGTARelative(int iGTAKey)
{
    for (int i = 0; *g_bkKeys[i].szKey != NULL; i++)
    {
        const SBindableKey* temp = &g_bkKeys[i];

        if (temp->iGTARelative == iGTAKey)
            return temp;
    }

    return NULL;
}

bool CKeyBinds::IsControl(const char* szControl)
{
    for (int i = 0; *g_bcControls[i].szControl != NULL; i++)
    {
        SBindableGTAControl* temp = &g_bcControls[i];

        if (!strcmp(temp->szControl, szControl))
            return true;
    }

    return false;
}

void CKeyBinds::SetAllControls(bool bState)
{
    SetAllFootControls(bState);
    SetAllVehicleControls(bState);
}

void CKeyBinds::SetAllFootControls(bool bState)
{
    for (int i = 0; *g_bcControls[i].szControl != NULL; i++)
    {
        SBindableGTAControl* temp = &g_bcControls[i];

        if (temp->controlType == CONTROL_FOOT)
            temp->bState = bState;
    }
}

void CKeyBinds::SetAllVehicleControls(bool bState)
{
    for (int i = 0; *g_bcControls[i].szControl != NULL; i++)
    {
        SBindableGTAControl* temp = &g_bcControls[i];

        if (temp->controlType == CONTROL_VEHICLE)
            temp->bState = bState;
    }
}

void CKeyBinds::SetAllBindStates(bool bState, KeyBindType onlyType)
{
    for (KeyBindPtr& bind : m_binds)
    {
        if (onlyType != KeyBindType::UNDEFINED && bind->type != onlyType)
            continue;

        switch (bind->type)
        {
            case KeyBindType::COMMAND:
            case KeyBindType::FUNCTION:
            {
                auto stateBind = static_cast<CKeyBindWithState*>(bind.get());
                if (stateBind->state != bState)
                {
                    if (stateBind->triggerState == bState)
                        Call(stateBind);

                    stateBind->state = bState;
                }
                break;
            }
            case KeyBindType::GTA_CONTROL:
            {
                CallGTAControlBind(static_cast<CGTAControlBind*>(bind.get()), bState);
                break;
            }
            default:
                break;
        }
    }
}

unsigned int CKeyBinds::Count(KeyBindType bindType)
{
    if (bindType == KeyBindType::UNDEFINED)
        return static_cast<unsigned int>(m_binds.size());

    const auto predicate = [bindType](const KeyBindPtr& bind) { return bind->type == bindType; };
    ptrdiff_t  count = std::count_if(m_binds.begin(), m_binds.end(), predicate);
    return static_cast<unsigned int>(count);
}

void CKeyBinds::DoPreFramePulse()
{
    CControllerState cs;
    m_pCore->GetGame()->GetPad()->GetCurrentControllerState(&cs);
    m_pCore->GetGame()->GetPad()->SetLastControllerState(&cs);

    // HACK: chatbox binds
    if (m_pChatBoxBind)
    {
        Call(m_pChatBoxBind);
        m_pChatBoxBind = NULL;
    }

    // Execute two binds from queue
    for (auto i = 0; i < 2; i++)
    {
        auto it = m_vecBindQueue.begin();
        if (it == m_vecBindQueue.end())
            break;
        Call(*it);
        m_vecBindQueue.erase(it);
    }

    // HACK: shift keys
    if (m_pCore->IsFocused())
    {
        bool bLeftShift = (GetKeyState(VK_LSHIFT) & 0x8000) != 0;
        bool bRightShift = (GetKeyState(VK_RSHIFT) & 0x8000) != 0;

        if (bLeftShift != bPreLeftShift)
        {
            if (bLeftShift)
                ProcessKeyStroke(&g_bkKeys[9], true);
            else
                ProcessKeyStroke(&g_bkKeys[9], false);

            bPreLeftShift = bLeftShift;
        }
        if (bRightShift != bPreRightShift)
        {
            if (bRightShift)
                ProcessKeyStroke(&g_bkKeys[10], true);
            else
                ProcessKeyStroke(&g_bkKeys[10], false);

            bPreRightShift = bRightShift;
        }
    }
}

bool CKeyBinds::ControlForwardsBackWards(CControllerState& cs)
{
    bool bCurrentStateForwards = g_bcControls[3].bState;
    bool bCurrentStateBackwards = g_bcControls[4].bState;
    if (bCurrentStateForwards && !bCurrentStateBackwards)
    {
        m_bLastStateForwards = true;
        m_bLastStateBackwards = false;
    }
    else if (!bCurrentStateForwards && bCurrentStateBackwards)
    {
        m_bLastStateForwards = false;
        m_bLastStateBackwards = true;
    }

    bool bBothKeysPressed = false;
    if (bCurrentStateForwards && bCurrentStateBackwards)
    {
        bBothKeysPressed = true;
        if (!m_bLastStateForwards && m_bLastStateBackwards)
        {
            m_bMoveForwards = true;
        }
        else if (m_bLastStateForwards && !m_bLastStateBackwards)
        {
            m_bMoveForwards = false;
        }

        bool bForwardsState = m_bMoveForwards;
        bool bBackwardsState = !m_bMoveForwards;
        cs.LeftStickY = (short)((bForwardsState) ? bForwardsState * -128 : bBackwardsState * 128);
    }

    return bBothKeysPressed;
}

bool CKeyBinds::ControlLeftAndRight(CControllerState& cs)
{
    bool bCurrentStateLeft = g_bcControls[5].bState;
    bool bCurrentStateRight = g_bcControls[6].bState;
    if (bCurrentStateLeft && !bCurrentStateRight)
    {
        m_bLastStateLeft = true;
        m_bLastStateRight = false;
    }
    else if (!bCurrentStateLeft && bCurrentStateRight)
    {
        m_bLastStateLeft = false;
        m_bLastStateRight = true;
    }

    bool bBothKeysPressed = false;
    if (bCurrentStateLeft && bCurrentStateRight)
    {
        bBothKeysPressed = true;
        if (!m_bLastStateLeft && m_bLastStateRight)
        {
            m_bMoveLeft = true;
        }
        else if (m_bLastStateLeft && !m_bLastStateRight)
        {
            m_bMoveLeft = false;
        }

        bool bLeftState = m_bMoveLeft;
        bool bRightState = !m_bMoveLeft;
        cs.LeftStickX = (short)((bLeftState) ? bLeftState * -128 : bRightState * 128);
    }

    return bBothKeysPressed;
}

void CKeyBinds::DoPostFramePulse()
{
    eSystemState SystemState = CCore::GetSingleton().GetGame()->GetSystemState();

    if (m_bWaitingToLoadDefaults && (SystemState == 7 || SystemState == 9))            // Are GTA controls actually initialized?
    {
        LoadDefaultBinds();
        m_bWaitingToLoadDefaults = false;
    }

    if (SystemState != 9 /* GS_PLAYING_GAME */)
        return;

    bool  bInVehicle = false, bHasDetonator = false, bIsDead = false, bAimingWeapon = false, bEnteringVehicle = false;
    CPed* pPed = m_pCore->GetGame()->GetPools()->GetPedFromRef((DWORD)1);
    // Don't set any controller states if the local player isnt alive
    if (!pPed)
        return;

    if (pPed->GetVehicle())
        bInVehicle = true;
    if (pPed->GetCurrentWeaponSlot() == 12 /*Detonator*/)
        bHasDetonator = true;

    CTaskManager* pTaskManager = pPed->GetPedIntelligence()->GetTaskManager();
    CTask*        pTask = pTaskManager->GetTask(TASK_PRIORITY_PRIMARY);
    if (pTask)
    {
        if (pTask->GetTaskType() == TASK_COMPLEX_ENTER_CAR_AS_DRIVER || pTask->GetTaskType() == TASK_COMPLEX_ENTER_CAR_AS_PASSENGER)
        {
            bEnteringVehicle = true;
        }
    }
    pTask = pTaskManager->GetTask(TASK_PRIORITY_EVENT_RESPONSE_NONTEMP);
    if (pTask && pTask->GetTaskType() == TASK_SIMPLE_DEAD)
        bIsDead = true;

    pTask = pTaskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK);
    if (pTask && pTask->GetTaskType() == TASK_SIMPLE_USE_GUN)
        bAimingWeapon = g_bcControls[39].bState;

    if (bInVehicle != m_bInVehicle)
    {
        m_bInVehicle = bInVehicle;
        if (bInVehicle)
            CallAllGTAControlBinds(CONTROL_FOOT, false);
        else
            CallAllGTAControlBinds(CONTROL_VEHICLE, false);
    }

    CControllerState cs;
    memset(&cs, 0, sizeof(CControllerState));
    if (!bIsDead)
    {
        if (!bInVehicle)
        {
            cs.ButtonCircle = (g_bcControls[0].bState && !bHasDetonator) ? 255 : 0;     // Fire
            if (bAimingWeapon)
            {
                cs.RightShoulder2 = g_bcControls[8].bState ? 255 : 0;            // Zoom Out
                cs.LeftShoulder2 = g_bcControls[7].bState ? 255 : 0;             // Zoom In
            }
            else
            {
                cs.RightShoulder2 = g_bcControls[1].bState ? 255 : 0;            // Next Weapon
                cs.LeftShoulder2 = g_bcControls[2].bState ? 255 : 0;             // Previous Weapon
            }

            if (!ControlForwardsBackWards(cs))
            {
                cs.LeftStickY = (!g_bcControls[3].bState && !g_bcControls[4].bState) ? 0 : (g_bcControls[3].bState) ? -128 : 128;
            }

            if (!ControlLeftAndRight(cs))
            {
                cs.LeftStickX = (!g_bcControls[5].bState && !g_bcControls[6].bState) ? 0 : (g_bcControls[5].bState) ? -128 : 128;
            }

            // * Enter Exit
            // * Change View
            cs.ButtonSquare = (!bEnteringVehicle && g_bcControls[11].bState) ? 255 : 0;            // Jump
            cs.ButtonCross = (g_bcControls[12].bState) ? 255 : 0;                                  // Sprint
            cs.ShockButtonR = (g_bcControls[13].bState) ? 255 : 0;                                 // Look Behind
            cs.ShockButtonL = (g_bcControls[14].bState) ? 255 : 0;                                 // Crouch
            cs.LeftShoulder1 = (g_bcControls[15].bState) ? 255 : 0;                                // Action
            cs.m_bPedWalk = (g_bcControls[16].bState) ? 255 : 0;                                   // Walk
            // * Vehicle Keys
            cs.RightShoulder1 = (g_bcControls[39].bState) ? 255 : 0;            // Aim Weapon
            cs.DPadRight = (g_bcControls[40].bState) ? 255 : 0;                 // Conversation Yes
            cs.DPadLeft = (g_bcControls[41].bState) ? 255 : 0;                  // Conversation No
            cs.DPadUp = (g_bcControls[42].bState) ? 255 : 0;                    // Group Control Forwards
            cs.DPadDown = (g_bcControls[43].bState) ? 255 : 0;                  // Group Control Backwards
        }
        else
        {
            cs.ButtonCircle = (g_bcControls[17].bState) ? 255 : 0;             // Fire
            cs.LeftShoulder1 = (g_bcControls[18].bState) ? 255 : 0;            // Secondary Fire
            cs.LeftStickX = ((g_bcControls[19].bState && g_bcControls[20].bState) || (!g_bcControls[19].bState && !g_bcControls[20].bState)) ? 0
                            : (g_bcControls[19].bState)                                                                                      ? -128
                                                                                                                                             : 128;
            cs.LeftStickY = ((g_bcControls[21].bState && g_bcControls[22].bState) || (!g_bcControls[21].bState && !g_bcControls[22].bState)) ? 0
                            : (g_bcControls[21].bState)                                                                                      ? -128
                                                                                                                                             : 128;
            cs.ButtonCross = (g_bcControls[23].bState) ? 255 : 0;                                          // Accelerate
            cs.ButtonSquare = (g_bcControls[24].bState) ? 255 : 0;                                         // Reverse
            cs.DPadUp = (g_bcControls[25].bState) ? 255 : 0;                                               // Radio Next
            cs.DPadDown = (g_bcControls[26].bState) ? 255 : 0;                                             // Radio Previous
            cs.m_bRadioTrackSkip = (g_bcControls[27].bState) ? 255 : 0;                                    // Radio Skip
            cs.ShockButtonL = (g_bcControls[28].bState) ? 255 : 0;                                         // Horn
            cs.ShockButtonR = (g_bcControls[29].bState) ? 255 : 0;                                         // Sub Mission
            cs.RightShoulder1 = (g_bcControls[30].bState) ? 255 : 0;                                       // Handbrake
            cs.LeftShoulder2 = (g_bcControls[31].bState || g_bcControls[33].bState) ? 255 : 0;             // Look Left
            cs.RightShoulder2 = (g_bcControls[32].bState || g_bcControls[33].bState) ? 255 : 0;            // Look Right
            // * Look Behind - uses both keys above simultaneously
            // Mouse Look
            cs.RightStickX = ((g_bcControls[35].bState && g_bcControls[36].bState) || (!g_bcControls[35].bState && !g_bcControls[36].bState)) ? 0
                             : (g_bcControls[35].bState)                                                                                      ? 128
                                                                                                                                              : -128;
            cs.RightStickY = ((g_bcControls[37].bState && g_bcControls[38].bState) || (!g_bcControls[37].bState && !g_bcControls[38].bState)) ? 0
                             : (g_bcControls[37].bState)                                                                                      ? 128
                                                                                                                                              : -128;
        }
        cs.ButtonTriangle = (g_bcControls[9].bState) ? 255 : 0;            // Enter Exit
        cs.Select = (g_bcControls[10].bState) ? 255 : 0;                   // Change View

        bool disableGameplayControls = m_pCore->IsCursorForcedVisible() && m_pCore->IsCursorControlsToggled();

        if (!disableGameplayControls)
        {
            GetJoystickManager()->ApplyAxes(cs, bInVehicle);
        }

        // m_pCore->GetMouseControl()->ApplyAxes ( cs );
    }

    m_pCore->GetGame()->GetPad()->SetCurrentControllerState(&cs);

    // Sirens
    m_pCore->GetGame()->GetPad()->SetHornHistoryValue((cs.ShockButtonL == 255));

    // Unset the mousewheel states if we have any bound
    if (m_bMouseWheel)
    {
        for (const KeyBindPtr& bind : m_binds)
        {
            if (bind->isBeingDeleted || !bind->boundKey)
                continue;

            switch (bind->type)
            {
                case KeyBindType::COMMAND:
                case KeyBindType::FUNCTION:
                {
                    auto stateBind = static_cast<CKeyBindWithState*>(bind.get());
                    if (stateBind->boundKey->iGTARelative == GTA_KEY_MSCROLLUP || stateBind->boundKey->iGTARelative == GTA_KEY_MSCROLLDOWN)
                        stateBind->state = false;
                    break;
                }
                case KeyBindType::GTA_CONTROL:
                {
                    auto controlBind = static_cast<CGTAControlBind*>(bind.get());
                    if (controlBind->boundKey->iGTARelative == GTA_KEY_MSCROLLUP || controlBind->boundKey->iGTARelative == GTA_KEY_MSCROLLDOWN)
                    {
                        if (controlBind->state)
                            CallGTAControlBind(controlBind, false);
                    }
                    break;
                }
                default:
                    break;
            }
        }

        bindableKeyStates[BK_MOUSE_WHEEL_UP] = false;
        bindableKeyStates[BK_MOUSE_WHEEL_DOWN] = false;

        m_bMouseWheel = false;
    }
}

bool CKeyBinds::LoadFromXML(CXMLNode* pMainNode)
{
    bool bSuccess = false;
    bool bLoadDefaults = false;
    if (pMainNode)
    {
        CXMLNode*    pNode = NULL;
        unsigned int uiCount = pMainNode->GetSubNodeCount();

        if (uiCount == 0)
            bLoadDefaults = true;

        for (unsigned int i = 0; i < uiCount; i++)
        {
            pNode = pMainNode->GetSubNode(i);

            if (pNode == NULL)
                continue;

            std::string strValue;
            strValue = pNode->GetTagName();
            if (strValue.compare("bind") == 0)
            {
                CXMLAttribute* pAttribute = pNode->GetAttributes().Find("key");

                if (pAttribute)
                {
                    std::string strKey;
                    strKey = pAttribute->GetValue();

                    if (!strKey.empty())
                    {
                        pAttribute = pNode->GetAttributes().Find("command");

                        if (pAttribute)
                        {
                            std::string strCommand;
                            strCommand = pAttribute->GetValue();

                            if (!strCommand.empty())
                            {
                                // HACK
                                std::string strState, strArguments, strResource;
                                pNode = pMainNode->GetSubNode(i);

                                pAttribute = pNode->GetAttributes().Find("state");

                                if (pAttribute)
                                {
                                    strState = pAttribute->GetValue();
                                }
                                bool bState = strState.compare("up") != 0;

                                pAttribute = pNode->GetAttributes().Find("arguments");

                                if (pAttribute)
                                {
                                    strArguments = pAttribute->GetValue();
                                }

                                pAttribute = pNode->GetAttributes().Find("resource");
                                if (pAttribute)
                                {
                                    strResource = pAttribute->GetValue();

                                    pAttribute = pNode->GetAttributes().Find("default");
                                    SString strOriginalScriptKey = pAttribute ? pAttribute->GetValue() : "";

                                    AddCommand(strKey.c_str(), strCommand.c_str(), strArguments.c_str(), bState, strResource.c_str(), false,
                                               strOriginalScriptKey);
                                    SetCommandActive(strKey.c_str(), strCommand.c_str(), bState, strArguments.c_str(), strResource.c_str(), false, true);
                                }
                                else if (!CommandExists(strKey.c_str(), strCommand.c_str(), true, bState))
                                    AddCommand(strKey.c_str(), strCommand.c_str(), strArguments.c_str(), bState);
                            }
                        }
                        else
                        {
                            pAttribute = pNode->GetAttributes().Find("control");

                            if (pAttribute)
                            {
                                std::string strControl;
                                strControl = pAttribute->GetValue();

                                if (!strControl.empty())
                                {
                                    if (!GTAControlExists(strKey.c_str(), strControl.c_str()))
                                        AddGTAControl(strKey.c_str(), strControl.c_str());
                                }
                            }
                        }
                    }
                }
            }
        }
        bSuccess = true;
    }
    else
        bLoadDefaults = true;

    eSystemState SystemState = CCore::GetSingleton().GetGame()->GetSystemState();
    if (bLoadDefaults)
    {
        if (SystemState == 7 || SystemState == 9)            // Are GTA controls actually initialized?
            LoadDefaultBinds();
        else
            m_bWaitingToLoadDefaults = true;
    }

    return bSuccess;
}

bool CKeyBinds::SaveToXML(CXMLNode* pMainNode)
{
    if (!pMainNode)
        return false;

    // Clear our current bind nodes
    pMainNode->DeleteAllSubNodes();

    bool success = true;

    for (const KeyBindPtr& bind : m_binds)
    {
        if (bind->isBeingDeleted || !bind->boundKey || (bind->type != KeyBindType::COMMAND && bind->type != KeyBindType::GTA_CONTROL))
            continue;

        CXMLNode* bindNode = pMainNode->CreateSubNode("bind");

        if (!bindNode)
        {
            success = false;
            continue;
        }

        CXMLAttributes& attributes = bindNode->GetAttributes();

        auto createAttribute = [&attributes](const char* key, const char* value) {
            if (value && value[0])
            {
                CXMLAttribute* attribute = attributes.Create(key);
                attribute->SetValue(value);
            }
        };

        if (bind->type == KeyBindType::COMMAND)
        {
            auto commandBind = static_cast<CCommandBind*>(bind.get());

            // Don't save script added binds
            if (commandBind->wasCreatedByScript && !commandBind->isReplacingScriptKey)
            {
                pMainNode->DeleteSubNode(bindNode);
                continue;
            }

            createAttribute("key", commandBind->boundKey->szKey);
            createAttribute("state", commandBind->triggerState ? "down" : "up");
            createAttribute("command", commandBind->command.c_str());
            createAttribute("arguments", commandBind->arguments.c_str());

            if (!commandBind->resource.empty())
            {
                createAttribute("resource", commandBind->resource.c_str());
                createAttribute("default", commandBind->originalScriptKey.c_str());
            }
        }
        else            // bind->type == KeyBindType::GTA_CONTROL
        {
            auto controlBind = static_cast<CGTAControlBind*>(bind.get());
            createAttribute("key", controlBind->boundKey->szKey);
            createAttribute("control", controlBind->control->szControl);
        }
    }

    return success;
}

void CKeyBinds::LoadDefaultBinds()
{
    ClearCommandsAndControls();

    LoadControlsFromGTA();
    LoadDefaultCommands(true);
}

void CKeyBinds::LoadDefaultCommands(bool bForce)
{
    for (int i = 0; *g_dcbDefaultCommands[i].szKey != NULL; i++)
    {
        const SDefaultCommandBind* temp = &g_dcbDefaultCommands[i];
        if (bForce || !CommandExists(NULL, temp->szCommand, true, temp->bState, temp->szArguments))
            AddCommand(temp->szKey, temp->szCommand, temp->szArguments, temp->bState);
    }
}

void CKeyBinds::LoadControlsFromGTA()
{
    for (int i = FIRE; i <= GROUP_CONTROL_BACK; i++)
    {
        CGame* pGame = m_pCore->GetGame();
        if (pGame)
        {
            SBindableGTAControl* pControl = GetBindableFromAction((eControllerAction)i);
            if (pControl)
            {
                // Keyboard
                int iKey = pGame->GetControllerConfigManager()->GetControllerKeyAssociatedWithAction((eControllerAction)i, KEYBOARD);
                if (iKey != NO_KEY_DEFINED)
                {
                    const SBindableKey* pKey = GetBindableFromGTARelative(iKey);
                    if (pKey)
                        AddGTAControl(pKey, pControl);
                }

                // Optional Key
                iKey = pGame->GetControllerConfigManager()->GetControllerKeyAssociatedWithAction((eControllerAction)i, OPTIONAL_EXTRA);
                if (iKey != NO_KEY_DEFINED)
                {
                    const SBindableKey* pKey = GetBindableFromGTARelative(iKey);
                    if (pKey)
                        AddGTAControl(pKey, pControl);
                }

                // Mouse
                iKey = pGame->GetControllerConfigManager()->GetControllerKeyAssociatedWithAction((eControllerAction)i, MOUSE);
                if (iKey != NO_KEY_DEFINED)
                {
                    const SBindableKey* pKey = GetBindableFromGTARelative(iKey);
                    if (pKey)
                        AddGTAControl(pKey, pControl);
                }

                // Joystick
                iKey = pGame->GetControllerConfigManager()->GetControllerKeyAssociatedWithAction((eControllerAction)i, JOYSTICK);
                if (iKey)
                {
                    const SBindableKey* pKey = GetBindableFromGTARelative(GTA_KEY_JOY(iKey));
                    if (pKey)
                        AddGTAControl(pKey, pControl);
                }
            }
        }
    }
}

void CKeyBinds::BindCommand(const char* szCmdLine)
{
    CConsoleInterface* pConsole = m_pCore->GetConsole();

    char* szError = "* Syntax: bind <defaults/key> [<up/down>] <command> [<arguments>]";
    if (szCmdLine == NULL)
    {
        pConsole->Print(szError);
        return;
    }

    // Copy the buffer
    char* szTemp = new char[strlen(szCmdLine) + 16];
    strcpy(szTemp, szCmdLine);

    // Split it up into bind key, command and arguments
    char* szKey = strtok(szTemp, " ");
    char* szCommand = strtok(NULL, " ");

    if (szKey)
    {
        if (strcmp(szKey, "defaults") == 0)
        {
            LoadDefaultBinds();
            pConsole->Print("* Bound all default commands and controls\n");
        }
        else if (szCommand)
        {
            if (IsControl(szCommand))
            {
                if (!GTAControlExists(szKey, szCommand))
                {
                    if (AddGTAControl(szKey, szCommand))
                        pConsole->Printf("* Bound key '%s' to control '%s'", szKey, szCommand);
                    else
                        pConsole->Printf("* Failed to bind '%s' to control '%s'", szKey, szCommand);
                }
                else
                    pConsole->Printf("* '%s' key already bound to control '%s'", szKey, szCommand);
            }
            else
            {
                bool bState = true;
                if (strcmp(szCommand, "up") == 0 || strcmp(szCommand, "down") == 0)
                {
                    bState = (strcmp(szCommand, "down") == 0);
                    szCommand = strtok(NULL, " ");
                }

                if (szCommand)
                {
                    char* szArguments = strtok(NULL, "\0");
                    if (szArguments)
                        szArguments = SharedUtil::Trim(szArguments);
                    if (szArguments != nullptr && szArguments[0] == '\0')
                        szArguments = nullptr;

                    SString strKeyState("%s", bState ? "down" : "up");
                    SString strCommandAndArguments("%s%s%s", szCommand, szArguments ? " " : "", szArguments ? szArguments : "");

                    if (!CommandExists(szKey, szCommand, true, bState, szArguments))
                    {
                        if (AddCommand(szKey, szCommand, szArguments, bState))
                            pConsole->Printf("* Bound key '%s' '%s' to command '%s'", szKey, *strKeyState, *strCommandAndArguments);
                        else
                            pConsole->Printf("* Failed to bind '%s' '%s' to command '%s'", szKey, *strKeyState, *strCommandAndArguments);
                    }
                    else
                        pConsole->Printf("* '%s' '%s' key already bound to command '%s'", szKey, *strKeyState, *strCommandAndArguments);
                }
                else
                    pConsole->Print(szError);
            }
        }
        else
            pConsole->Print(szError);
    }
    else
        pConsole->Print(szError);

    delete[] szTemp;
}

void CKeyBinds::UnbindCommand(const char* szCmdLine)
{
    CConsoleInterface* pConsole = m_pCore->GetConsole();

    char* szError = "* Syntax: unbind <all/key> [<up/down/both> <command>]";
    if (szCmdLine == NULL)
    {
        pConsole->Print(szError);
        return;
    }

    // Copy the buffer
    char* szTemp = new char[strlen(szCmdLine) + 16];
    strcpy(szTemp, szCmdLine);

    // Split it up into bind key and command
    char* szKey = strtok(szTemp, " ");
    char* szCommand = strtok(NULL, " ");

    if (szKey)
    {
        if (stricmp(szKey, "all") == 0)
        {
            RemoveAllCommands();
            RemoveAllGTAControls();
            pConsole->Print("* Removed all bound keys.");
        }
        else if (szCommand)
        {
            if (IsControl(szCommand))
            {
                if (RemoveGTAControl(szKey, szCommand))
                    pConsole->Printf("* Unbound key '%s' from control '%s'", szKey, szCommand);
                else
                    pConsole->Printf("* Failed to unbind '%s' from control '%s'", szKey, szCommand);
            }
            else
            {
                bool        bState = true;
                bool        both = true;
                const char* szState = "both";

                if (strcmp(szCommand, "up") == 0 || strcmp(szCommand, "down") == 0 || strcmp(szCommand, "both") == 0)
                {
                    bState = (strcmp(szCommand, "down") == 0);
                    szState = szCommand;
                    szCommand = strtok(NULL, " ");

                    if (strcmp(szState, "both") != 0)
                        both = false;
                }

                if (szCommand)
                {
                    if (RemoveCommand(szKey, szCommand, !both, bState))
                        pConsole->Printf("* Unbound key '%s' '%s' from command '%s'", szKey, szState, szCommand);
                    else
                        pConsole->Printf("* Failed to unbind '%s' '%s' from command '%s'", szKey, szState, szCommand);
                }
                else if (RemoveAllCommands(szKey, !both, bState))
                    pConsole->Printf("* Removed all binds from key '%s' '%s'", szKey, szState);
                else
                    pConsole->Printf("* Failed to remove binds from key '%s' '%s'", szKey, szState);
            }
        }
        else
        {
            bool b = RemoveAllGTAControls(szKey);
            if (RemoveAllCommands(szKey) || b)
                pConsole->Printf("* Removed all binds from key '%s'", szKey);
            else
                pConsole->Printf("* Failed to remove binds from key '%s'", szKey);
        }
    }
    else
        pConsole->Print(szError);

    delete[] szTemp;
}

void CKeyBinds::PrintBindsCommand(const char* szCmdLine)
{
    std::string_view cmdLine{szCmdLine ? szCmdLine : ""};
    std::string      key{cmdLine.substr(0, cmdLine.find(' '))};

    CConsoleInterface* console = m_pCore->GetConsole();

    auto print = [console](const KeyBindPtr& bind) {
        switch (bind->type)
        {
            case KeyBindType::COMMAND:
            {
                auto commandBind = static_cast<CCommandBind*>(bind.get());
                console->Printf("Command^%s %s: %s %s", commandBind->boundKey->szKey, (commandBind->triggerState) ? "down" : "up", commandBind->command.c_str(),
                                commandBind->arguments.c_str());
                break;
            }
            case KeyBindType::GTA_CONTROL:
            {
                auto controlBind = static_cast<CGTAControlBind*>(bind.get());
                console->Printf("Control^%s: %s", controlBind->boundKey->szKey, controlBind->control->szControl);
                break;
            }
            default:
                return false;
        }

        return true;
    };

    bool anyMatches = false;

    if (key.empty())
    {
        console->Print("* Current key binds: *");

        for (const KeyBindPtr& bind : m_binds)
        {
            if (print(bind))
                anyMatches = true;
        }
    }
    else
    {
        const SBindableKey* bindableKey = GetBindableFromKey(key.c_str());

        if (!bindableKey)
        {
            console->Printf("* Error: '%s' key not found", key.c_str());
            return;
        }

        console->Printf("* Current key binds for '%.*s': *", key.size(), key.data());

        for (const KeyBindPtr& bind : m_binds)
        {
            if (!bind->boundKey || bind->boundKey != bindableKey || !bind->boundKey->szKey)
                continue;

            if (print(bind))
                anyMatches = true;
        }
    }

    if (!anyMatches)
        console->Print("empty.");
}

bool CKeyBinds::IsFakeCtrl_L(UINT message, WPARAM wParam, LPARAM lParam)
{
    MSG  msgNext;
    LONG lTime;
    BOOL bReturn;

    /*
     * Fake Ctrl_L presses will be followed by an Alt_R keypress
     * with the same timestamp as the Ctrl_L press.
     */
    if ((message == WM_KEYDOWN || message == WM_SYSKEYDOWN) && wParam == VK_CONTROL && (HIWORD(lParam) & KF_EXTENDED) == 0)
    {
        /* Got a Ctrl_L press */

        /* Get time of current message */
        lTime = GetMessageTime();

        /* Look for fake Ctrl_L preceeding an Alt_R press. */
        bReturn = PeekMessage(&msgNext, NULL, WM_KEYDOWN, WM_SYSKEYDOWN, PM_NOREMOVE);

        /*
         * Try again if the first call fails.
         * NOTE: This usually happens when TweakUI is enabled.
         */
        if (!bReturn)
        {
            /* Voodoo to make sure that the Alt_R message has posted */
            Sleep(0);

            /* Look for fake Ctrl_L preceeding an Alt_R press. */
            bReturn = PeekMessage(&msgNext, NULL, WM_KEYDOWN, WM_SYSKEYDOWN, PM_NOREMOVE);
        }

        if (msgNext.message != WM_KEYDOWN && msgNext.message != WM_SYSKEYDOWN)
            bReturn = 0;

        /* Is next press an Alt_R with the same timestamp? */
        if (bReturn && msgNext.wParam == VK_MENU && msgNext.time == lTime && (HIWORD(msgNext.lParam) & KF_EXTENDED))
        {
            /*
             * Next key press is Alt_R with same timestamp as current
             * Ctrl_L message.  Therefore, this Ctrl_L press is a fake
             * event, so discard it.
             */
            return TRUE;
        }
    }

    /*
     * Fake Ctrl_L releases will be followed by an Alt_R release
     * with the same timestamp as the Ctrl_L release.
     */
    if ((message == WM_KEYUP || message == WM_SYSKEYUP) && wParam == VK_CONTROL && (HIWORD(lParam) & KF_EXTENDED) == 0)
    {
        /* Got a Ctrl_L release */

        /* Get time of current message */
        lTime = GetMessageTime();

        /* Look for fake Ctrl_L release preceeding an Alt_R release. */
        bReturn = PeekMessage(&msgNext, NULL, WM_KEYUP, WM_SYSKEYUP, PM_NOREMOVE);

        /*
         * Try again if the first call fails.
         * NOTE: This usually happens when TweakUI is enabled.
         */
        if (!bReturn)
        {
            /* Voodoo to make sure that the Alt_R message has posted */
            Sleep(0);

            /* Look for fake Ctrl_L release preceeding an Alt_R release. */
            bReturn = PeekMessage(&msgNext, NULL, WM_KEYUP, WM_SYSKEYUP, PM_NOREMOVE);
        }

        if (msgNext.message != WM_KEYUP && msgNext.message != WM_SYSKEYUP)
            bReturn = 0;

        /* Is next press an Alt_R with the same timestamp? */
        if (bReturn && (msgNext.message == WM_KEYUP || msgNext.message == WM_SYSKEYUP) && msgNext.wParam == VK_MENU && msgNext.time == lTime &&
            (HIWORD(msgNext.lParam) & KF_EXTENDED))
        {
            /*
             * Next key release is Alt_R with same timestamp as current
             * Ctrl_L message. Therefore, this Ctrl_L release is a fake
             * event, so discard it.
             */
            return TRUE;
        }
    }

    /* Not a fake control left press/release */
    return FALSE;
}

bool CKeyBinds::TriggerKeyStrokeHandler(const SString& strKey, bool bState, bool bIsConsoleInputKey)
{
    // Call the key-stroke handler if we have one
    if (m_KeyStrokeHandler)
    {
        return m_KeyStrokeHandler(strKey, bState, bIsConsoleInputKey);
    }
    return true;
}
