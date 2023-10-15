/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientTask.h
 *  PURPOSE:     Task class header
 *
 *****************************************************************************/

#pragma once

#include <list>
#include "lua/LuaCommon.h"
#include "CClientEntity.h"
#include <game/CTaskManager.h>

class CClientEntity;
class CClientManager;
class CClientPed;
class CElementArray;
class CLuaArgument;
class CPed;
class CTask;
class CVehicle;

struct STaskState
{
    bool                              bUseZone;
    std::string                       strState;
    std::optional<eTaskType>          eSubTask = {};
    std::optional<eTaskType>          eSecondaryTask = {};
    std::optional<eSecondaryTaskType> eSecondaryType = {};
};

static const std::multimap<eTaskType, STaskState> g_playerTaskStates{
    {TASK_COMPLEX_JUMP, {true, "Climbing around in", TASK_SIMPLE_CLIMB}},
    {TASK_SIMPLE_GANG_DRIVEBY, {true, "Doing a drive-by in"}},
    {TASK_SIMPLE_DRIVEBY_SHOOT, {true, "Doing a drive-by in"}},
    {TASK_SIMPLE_DIE, {false, "Blub blub...", TASK_SIMPLE_DROWN}},
    {TASK_SIMPLE_DIE, {false, "Breathing water", TASK_SIMPLE_DROWN}},
    {TASK_SIMPLE_DIE, {true, "Drowning in", TASK_SIMPLE_DROWN}},
    {TASK_SIMPLE_PLAYER_ON_FOOT, {true, "Ducking for cover in", {}, TASK_SIMPLE_DUCK, TASK_SECONDARY_DUCK}},
    {TASK_SIMPLE_PLAYER_ON_FOOT, {true, "Fighting in", {}, TASK_SIMPLE_FIGHT, TASK_SECONDARY_ATTACK}},
    {TASK_SIMPLE_PLAYER_ON_FOOT, {true, "Throwing fists in", {}, TASK_SIMPLE_FIGHT, TASK_SECONDARY_ATTACK}},
    {TASK_SIMPLE_PLAYER_ON_FOOT, {true, "Blastin' fools in", {}, TASK_SIMPLE_USE_GUN, TASK_SECONDARY_ATTACK}},
    {TASK_SIMPLE_PLAYER_ON_FOOT, {true, "Shooting up", {}, TASK_SIMPLE_USE_GUN, TASK_SECONDARY_ATTACK}},
    {TASK_SIMPLE_JETPACK, {true, "Jetpacking in"}},
    {TASK_SIMPLE_PLAYER_ON_FOOT, {true, "Literally on fire in", {}, TASK_SIMPLE_PLAYER_ON_FIRE, TASK_SECONDARY_PARTIAL_ANIM}},
    {TASK_SIMPLE_PLAYER_ON_FOOT, {true, "Burning up in", {}, TASK_SIMPLE_PLAYER_ON_FIRE, TASK_SECONDARY_PARTIAL_ANIM}},
    {TASK_COMPLEX_IN_WATER, {true, "Swimming in", TASK_SIMPLE_SWIM}},
    {TASK_COMPLEX_IN_WATER, {true, "Floating around in", TASK_SIMPLE_SWIM}},
    {TASK_COMPLEX_IN_WATER, {false, "Being chased by a shark", TASK_SIMPLE_SWIM}},
    {TASK_SIMPLE_CHOKING, {true, "Choking to death in"}},
};

class CClientTask
{
public:
    CClientTask(CClientManager* pManager);

    bool Read(lua_State* luaVM, int iTableIndex, bool bClear);
    bool ReadElements(lua_State* luaVM, int iTableIndex, bool bClear);
    bool ReadParameters(lua_State* luaVM, int iTableIndex, bool bClear);
    bool Write(lua_State* luaVM, int iTableIndex);
    bool WriteElements(lua_State* luaVM, int iTableIndex);
    bool WriteParameters(lua_State* luaVM, int iTableIndex);

    const char* GetTaskName() { return m_strTaskName.c_str(); };
    void        SetTaskName(const char* szName) { m_strTaskName = szName ? szName : ""; };

    static unsigned long GenerateUniqueIdentifier();
    unsigned long        GetUniqueIdentifier() { return m_ulUniqueIdentifier; };
    void                 SetUniqueIdentifier(unsigned long ulID) { m_ulUniqueIdentifier = ulID; };

    void AddElement(CClientEntity* pElement);
    void RemoveElement(CClientEntity* pElement);
    void ClearElements();

    bool                        IsElementIn(CClientEntity* pElement);
    const std::list<ElementID>& GetElements() { return m_Elements; };

    const std::list<std::string>&  GetKeys() { return m_Keys; };
    const std::list<CLuaArgument>& GetValues() { return m_Values; };
    void                           SetParameter(const char* szKey, const CLuaArgument& Value);
    void                           SetParameters(const std::list<std::string>& Keys, const std::list<CLuaArgument>& Values);
    void                           ClearParameters();

    CLuaArgument* GetParameter(const char* szKey);
    bool          GetParameterBool(const char* szKey, bool& Bool);
    bool          GetParameterNumber(const char* szKey, float& Number);
    CPed*         GetParameterPed(const char* szKey);
    const char*   GetParameterString(const char* szKey);
    CVehicle*     GetParameterVehicle(const char* szKey);

    bool ApplyTask(CClientPed& Ped);

private:
    CTask* CreateTask(bool& bTaskPrimary, int& iTaskPriority);

    CClientManager* m_pManager;

    std::string   m_strTaskName;
    unsigned long m_ulUniqueIdentifier;

    std::list<ElementID>    m_Elements;
    std::list<std::string>  m_Keys;
    std::list<CLuaArgument> m_Values;

    static unsigned long m_ulLastUniqueIdentifier;
};
