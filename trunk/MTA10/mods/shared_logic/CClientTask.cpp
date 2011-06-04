/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTask.cpp
*  PURPOSE:     Task class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

unsigned long CClientTask::m_ulLastUniqueIdentifier = 0;

// Temporary until we start using ids
#define VERIFY_PLAYER(player) (m_pManager->GetPlayerManager ()->Exists(player)&&!player->IsBeingDeleted())
#define VERIFY_VEHICLE(vehicle) (m_pManager->GetVehicleManager ()->Exists(vehicle)&&!vehicle->IsBeingDeleted())

CClientTask::CClientTask ( CClientManager* pManager )
{
    m_ulUniqueIdentifier = 0;
    m_pManager = pManager;
}


bool CClientTask::Read ( lua_State* luaVM, int iTableIndex, bool bClear )
{
    // Grab the stack position for the table after we've pushed data to it.
    // This is table index - 1 if the index is negative, otherwize it stays
    // the same.
    int iNewTableIndex;
    if ( iTableIndex < 0 )
        iNewTableIndex = iTableIndex - 1;
    else
        iNewTableIndex = iTableIndex;

    // Not a table? Bad
    if ( !lua_istable ( luaVM, iTableIndex ) )
    {
        return false;
    }

    // Grab the elements
    lua_pushstring ( luaVM, "elements" );
    lua_gettable ( luaVM, iNewTableIndex );

    // Read them out. Abort if failed
    if ( !ReadElements ( luaVM, -1, bClear ) )
    {
        return false;
    }

    // Grab the taskname
    lua_pushstring ( luaVM, "taskname" );
    lua_gettable ( luaVM, iNewTableIndex );
    const char* szTaskName = lua_tostring ( luaVM, -1 );

    // Check that it exists, if it does set it
    if ( !szTaskName )
        return false;
    m_strTaskName = szTaskName;

    // Grab the identifier
    lua_pushstring ( luaVM, "identifier" );
    lua_gettable ( luaVM, iNewTableIndex );
    m_ulUniqueIdentifier = static_cast < unsigned long > ( lua_tonumber ( luaVM, -1 ) );

    // Grab the parameter table
    lua_pushstring ( luaVM, "parameters" );
    lua_gettable ( luaVM, iNewTableIndex );

    // Read out the table that is now at the top of the stack
    return ReadParameters ( luaVM, -1, bClear );
}


bool CClientTask::ReadElements ( lua_State* luaVM, int iTableIndex, bool bClear )
{
    // Clear the old parameters first
    if ( bClear )
    {
        m_Elements.clear ();
    }

    // Grab the stack position for the table after we've pushed data to it.
    // This is table index - 1 if the index is negative, otherwize it stays
    // the same.
    int iNewTableIndex;
    if ( iTableIndex < 0 )
        iNewTableIndex = iTableIndex - 1;
    else
        iNewTableIndex = iTableIndex;

    // Not a table? Bad
    if ( !lua_istable ( luaVM, iTableIndex ) )
    {
        return false;
    }

    // Loop through our table, beginning at the first key
    lua_pushnil ( luaVM );
    while ( lua_next ( luaVM, iNewTableIndex ) != 0 )
    {
        // Get the index and the element ID
        unsigned int uiIndex = static_cast < unsigned int > ( lua_tonumber ( luaVM, -2 ) );
        ElementID ID = static_cast < ElementID > ( lua_tonumber ( luaVM, -1 ) );

        // Grab the element and check he's a player/ped
        CClientEntity* pEntity = CElementIDs::GetElement ( ID );
        if ( pEntity && pEntity->GetType () == CCLIENTPLAYER )
        {
            // If we cleared, just add it
            if ( bClear )
            {
                // Store it
                m_Elements.push_back ( ID );;
            }
            else
            {
                // Otherwize merge it in
                AddElement ( pEntity );
            }
        }

        // Remove the value and keep the key for the next iteration
        lua_pop ( luaVM, 1 );
    }

    // Success
    return true;
}


bool CClientTask::ReadParameters ( lua_State* luaVM, int iTableIndex, bool bClear )
{
    // Clear the old parameters first
    if ( bClear )
    {
        m_Keys.clear ();
        m_Values.clear ();
    }

    // Grab the stack position for the table after we've pushed data to it.
    // This is table index - 1 if the index is negative, otherwize it stays
    // the same.
    int iNewTableIndex;
    if ( iTableIndex < 0 )
        iNewTableIndex = iTableIndex - 1;
    else
        iNewTableIndex = iTableIndex;

    // Not a table? Bad
    if ( !lua_istable ( luaVM, iTableIndex ) )
    {
        return false;
    }

    // Loop through our table, beginning at the first key
    lua_pushnil ( luaVM );
    while ( lua_next ( luaVM, iNewTableIndex ) != 0 )
    {
        // Get the key and value
        const char* szKey = lua_tostring ( luaVM, -2 );
        CLuaArgument Value;
        Value.Read ( luaVM, -1 );

        // Got both a key and a value?
        if ( szKey )
        {
            // If we cleared, just add them
            if ( bClear )
            {
                // Store it
                m_Keys.push_back ( szKey );
                m_Values.push_back ( Value );
            }
            else
            {
                // Otherwize merge it in
                SetParameter ( szKey, Value );
            }

            // Remove the value and keep the key for the next iteration
            lua_pop ( luaVM, 1 );
        }
        else
        {
            // Remove the value and keep the key for the next iteration
            lua_pop ( luaVM, 1 );
            return false;
        }
    }

    // Success
    return true;
}


bool CClientTask::Write ( lua_State* luaVM, int iTableIndex )
{
    // Grab the stack position for the table after we've pushed data to it.
    // This is table index - 2 if the index is negative, otherwize it stays
    // the same.
    int iNewTableIndex;
    if ( iTableIndex < 0 )
        iNewTableIndex = iTableIndex - 2;
    else
        iNewTableIndex = iTableIndex;

    // Not a table? Bad
    if ( !lua_istable ( luaVM, iTableIndex ) )
    {
        return false;
    }

    // Make a table with elements
    lua_pushstring ( luaVM, "elements" );
    lua_newtable ( luaVM );

    // Element entry. Represents the peds and the players this task belongs to.
    if ( !WriteElements ( luaVM, -1 ) )
    {
        return false;
    }

    lua_settable ( luaVM, iNewTableIndex );

    // Store the task name in the userdata
    lua_pushstring ( luaVM, "taskname" );
    lua_pushstring ( luaVM, m_strTaskName.c_str () );
    lua_settable ( luaVM, iNewTableIndex );

    // Store the identifier in the userdata
    lua_pushstring ( luaVM, "identifier" );
    lua_pushnumber ( luaVM, m_ulUniqueIdentifier );
    lua_settable ( luaVM, iNewTableIndex );

    // Make a table with entries
    lua_pushstring ( luaVM, "parameters" );
    lua_newtable ( luaVM );

    // Write the parameters to it
    if ( !WriteParameters ( luaVM, -1 ) )
    {
        return false;
    }

    // Add the table to the first table
    lua_settable ( luaVM, iNewTableIndex );
    return true;
}


bool CClientTask::WriteElements ( lua_State* luaVM, int iTableIndex )
{
    // Grab the stack position for the table after we've pushed data to it.
    // This is table index - 2 if the index is negative, otherwize it stays
    // the same.
    int iNewTableIndex;
    if ( iTableIndex < 0 )
        iNewTableIndex = iTableIndex - 2;
    else
        iNewTableIndex = iTableIndex;

    // Not a table? Bad
    if ( !lua_istable ( luaVM, iTableIndex ) )
    {
        return false;
    }

    // Push the values to it.
    unsigned int uiIndex = 1;
    list < ElementID > ::iterator iterElements = m_Elements.begin ();
    while ( iterElements != m_Elements.end () )
    {
        // Add the entries to it
        lua_pushnumber ( luaVM, uiIndex );
        lua_pushnumber ( luaVM, (*iterElements).Value() );
        lua_settable ( luaVM, iNewTableIndex );

        // Increment index
        ++uiIndex;
    }

    // Success
    return true;
}


bool CClientTask::WriteParameters ( lua_State* luaVM, int iTableIndex )
{
    // Grab the stack position for the table after we've pushed data to it.
    // This is table index - 2 if the index is negative, otherwize it stays
    // the same.
    int iNewTableIndex;
    if ( iTableIndex < 0 )
        iNewTableIndex = iTableIndex - 2;
    else
        iNewTableIndex = iTableIndex;

    // Not a table? Bad
    if ( !lua_istable ( luaVM, iTableIndex ) )
    {
        return false;
    }

    // Push the values to it.
    list < std::string > ::iterator iterKey = m_Keys.begin ();
    list < CLuaArgument > ::iterator iterValue = m_Values.begin ();
    while ( iterKey != m_Keys.end () )
    {
        // Add the entries to it
        lua_pushstring ( luaVM, iterKey->c_str () );
        iterValue->Push ( luaVM );
        lua_settable ( luaVM, iNewTableIndex );

        // Increment iterators
        iterKey++;
        iterValue++;
    }

    // Success
    return true;
}


unsigned long CClientTask::GenerateUniqueIdentifier ( void )
{
    return ++m_ulLastUniqueIdentifier;
}


void CClientTask::AddElement ( CClientEntity* pElement )
{
    // Not already in the list? Add it
    if ( !IsElementIn ( pElement ) )
    {
        m_Elements.push_back ( pElement->GetID () );
    }
}


void CClientTask::RemoveElement ( CClientEntity* pElement )
{
    // Remove his ID from the list
    m_Elements.remove ( pElement->GetID () );
}


void CClientTask::ClearElements ( void )
{
    // Clear the list
    m_Elements.clear ();
}


bool CClientTask::IsElementIn ( CClientEntity* pElement )
{
    // Grab the element id
    ElementID ID = pElement->GetID ();

    // Loop through all the elements
    std::list < ElementID > ::iterator iter = m_Elements.begin ();
    for ( ; iter != m_Elements.end (); iter++ )
    {
        // Matching ids? Exists
        if ( ID == *iter )
        {
            return true;
        }
    }

    // Not
    return false;
}


void CClientTask::SetParameter ( const char* szKey, const CLuaArgument& Value )
{
    // Grab the value
    CLuaArgument* pValue = GetParameter ( szKey );

    // Does it exist? Change it
    if ( pValue )
    {
        *pValue = Value;
    }
    else
    {
        // If not, add it
        m_Keys.push_back ( szKey );
        m_Values.push_back ( Value );
    }
}


void CClientTask::SetParameters ( const std::list <std::string>& Keys,
                                  const std::list <CLuaArgument>& Values )
{
    // Replace the lists
    m_Keys = Keys;
    m_Values = Values;

    // They must have identical size
    assert ( m_Keys.size () == m_Values.size () );
}


void CClientTask::ClearParameters ( void )
{
    // Clear all the parameters
    m_Keys.clear ();
    m_Values.clear ();
}


CLuaArgument* CClientTask::GetParameter ( const char* szKey )
{
    // Loop through all the keys
    std::list < std::string > ::iterator iterKeys = m_Keys.begin ();
    std::list < CLuaArgument > ::iterator iterValues = m_Values.begin ();
    while ( iterKeys != m_Keys.end () )
    {
        // Matching key? Return it.
        if ( *iterKeys == szKey )
        {
            return &*iterValues;
        }

        // Next
        iterKeys++;
        iterValues++;
    }

    // Nope
    return NULL;
}


bool CClientTask::GetParameterBool ( const char* szKey, bool& Bool )
{
    // Grab the parameter and check its type
    CLuaArgument* pArgument = GetParameter ( szKey );
    if ( pArgument && pArgument->GetType () == LUA_TBOOLEAN )
    {
        // Return the bool
        Bool = pArgument->GetBoolean ();
        return true;
    }

    // Non-existing
    return false;
}


bool CClientTask::GetParameterNumber ( const char* szKey, float& Number )
{
    // Grab the parameter and check its type
    CLuaArgument* pArgument = GetParameter ( szKey );
    if ( pArgument && pArgument->GetType () == LUA_TNUMBER )
    {
        // Return the number
        Number = static_cast < float > ( pArgument->GetNumber () );
        return true;
    }

    // Non-existing
    return false;
}


CPed* CClientTask::GetParameterPed ( const char* szKey )
{
    // Grab the parameter, is it userdata?
    CLuaArgument* pArgument = GetParameter ( szKey );
    if ( pArgument && pArgument->GetType () == LUA_TLIGHTUSERDATA )
    {
        // Grab the player and verify it
        CClientPlayer* pPlayer = reinterpret_cast < CClientPlayer* > ( pArgument->GetLightUserData () );
        if ( VERIFY_PLAYER ( pPlayer ) )
        {
            // Return his game player
            return pPlayer->GetGamePlayer ();
        }
    }

    // Non-existing
    return NULL;
}


const char* CClientTask::GetParameterString ( const char* szKey )
{
    // Grab the parameter and check its type
    CLuaArgument* pArgument = GetParameter ( szKey );
    if ( pArgument && pArgument->GetType () == LUA_TSTRING )
    {
        // Return the string
        return pArgument->GetString ();
    }

    // Non-existing
    return NULL;
}


CVehicle* CClientTask::GetParameterVehicle ( const char* szKey )
{
    // Grab the parameter, is it userdata?
    CLuaArgument* pArgument = GetParameter ( szKey );
    if ( pArgument && pArgument->GetType () == LUA_TLIGHTUSERDATA )
    {
        // Grab the player and verify it
        CClientVehicle* pVehicle = reinterpret_cast < CClientVehicle* > ( pArgument->GetLightUserData () );
        if ( VERIFY_VEHICLE ( pVehicle ) )
        {
            // Return the game vehicle
            return pVehicle->GetGameVehicle ();
        }
    }

    // Non-existing
    return NULL;
}


bool CClientTask::ApplyTask ( CClientPed& Ped )
{
    // Create the task we have
    bool bTaskPrimary;
    int iTaskPriority;
    CTask* pTask = CreateTask ( bTaskPrimary, iTaskPriority );
    if ( pTask )
    {
        // Assign it to the player
        if ( bTaskPrimary )
        {
            return Ped.SetTask ( pTask, iTaskPriority );
        }
        else
        {
            return Ped.SetTaskSecondary ( pTask, iTaskPriority );
        }
    }

    return false;
}


CTask* CClientTask::CreateTask ( bool& bTaskPrimary, int& iTaskPriority )
{
    // Run named anim task?
    if ( m_strTaskName == "TASK_SIMPLE_NAMED_ANIM" )
    {
        // Grab the parameters for this task
        float fFlags = 0;
        float fBlendDelta = 0;
        float fTime = -1;
        bool bDontInterrupt = false;
        bool bRunInSequence = false;
        bool bOffsetPed = false;
        bool bHoldLastFrame = false;
        const char* szAnim = GetParameterString ( "anim" );
        const char* szAnimGroup = GetParameterString ( "animGroup" );
        GetParameterNumber ( "flags", fFlags );
        GetParameterNumber ( "blendDelta", fBlendDelta );
        GetParameterNumber ( "time", fTime );
        GetParameterBool ( "dontInterrupt", bDontInterrupt );
        GetParameterBool ( "runInSequence", bRunInSequence );
        GetParameterBool ( "offsetPed", bOffsetPed );
        GetParameterBool ( "holdLastFrame", bHoldLastFrame );

        // Default slots
        //bTaskPrimary = true;
        //iTaskPriority = TASK_PRIORITY_PRIMARY;
        bTaskPrimary = false;
        iTaskPriority = TASK_SECONDARY_PARTIAL_ANIM;

        // Create the task
        return g_pGame->GetTasks ()->CreateTaskSimpleRunNamedAnim ( szAnim,
                                                                    szAnimGroup,
                                                                    static_cast < int > ( fFlags ),
                                                                    fBlendDelta,
                                                                    static_cast < int > ( fTime ),
                                                                    bDontInterrupt,
                                                                    bRunInSequence,
                                                                    bOffsetPed,
                                                                    bHoldLastFrame );
    }

    // Run anim task?
    if ( m_strTaskName == "TASK_SIMPLE_ANIM" )
    {
        // Grab the parameters for this task
        float fAnimGroup = 0;
        float fAnimID = 0;
        float fBlendDelta = 0;
        float fTaskType = 0;
        bool bHoldLastFrame = false;
        GetParameterNumber ( "animGroup", fAnimGroup );
        GetParameterNumber ( "animID", fAnimID );
        GetParameterNumber ( "blendDelta", fBlendDelta );
        GetParameterNumber ( "taskType", fTaskType );
        const char* szTaskName = GetParameterString ( "taskName" );
        GetParameterBool ( "holdLastFrame", bHoldLastFrame );

        // Default slots
        bTaskPrimary = true;
        iTaskPriority = TASK_PRIORITY_PRIMARY;

        // Create the task
        return g_pGame->GetTasks ()->CreateTaskSimpleRunAnim (  static_cast < int > ( fAnimGroup ),
                                                                static_cast < int > ( fAnimID ),
                                                                fBlendDelta,
                                                                static_cast < int > ( fTaskType ),
                                                                szTaskName,
                                                                bHoldLastFrame );
    }

    // Climbing task?
    if ( m_strTaskName == "TASK_SIMPLE_CLIMB" )
    {
        // Grab the parameters for this task
        CPed* pAttacker = GetParameterPed ( "attacker" );
        bool bIsTeargas = true;
        GetParameterBool ( "teargas", bIsTeargas );

        CVector vecTarget;
        float fHeading;
        float fSurfaceType;
        float fClimbStage;
        bool bForceClimb;
        GetParameterNumber ( "targetX", vecTarget.fX );
        GetParameterNumber ( "targetY", vecTarget.fY );
        GetParameterNumber ( "targetZ", vecTarget.fZ );
        GetParameterNumber ( "heading", fHeading );
        GetParameterNumber ( "surfaceType", fSurfaceType );
        GetParameterNumber ( "climbStage", fClimbStage );
        GetParameterBool ( "forceClimb", bForceClimb );

        // Default slots
        bTaskPrimary = true;
        iTaskPriority = TASK_PRIORITY_PRIMARY;

        // Create the task
        return g_pGame->GetTasks ()->CreateTaskSimpleClimb ( NULL,
                                                             vecTarget,
                                                             fHeading,
                                                             static_cast < unsigned char > ( fSurfaceType ),
                                                             static_cast < char > ( fClimbStage ),
                                                             bForceClimb );
    }

    // Choking task?
    if ( m_strTaskName == "TASK_SIMPLE_CHOKING" )
    {
        // Grab the parameters for this task
        CPed* pAttacker = GetParameterPed ( "attacker" );
        bool bIsTeargas = true;
        GetParameterBool ( "teargas", bIsTeargas );

        // Default slots
        bTaskPrimary = true;
        iTaskPriority = TASK_PRIORITY_PHYSICAL_RESPONSE;

        // Create the task
        return g_pGame->GetTasks ()->CreateTaskSimpleChoking ( pAttacker, bIsTeargas );
    }

    // Use mobile phone task?
    if ( m_strTaskName == "TASK_COMPLEX_USE_MOBILE_PHONE" )
    {
        // Grab the parameters for this task
        float fDuration;
        if ( !GetParameterNumber ( "duration", fDuration ) )
            fDuration = -1;

        // Default slots
        bTaskPrimary = true;
        iTaskPriority = TASK_PRIORITY_PRIMARY;

        // Create the task
        return g_pGame->GetTasks ()->CreateTaskComplexUseMobilePhone ( static_cast < int > ( fDuration ) );
    }

    // Use wander task?
    if ( m_strTaskName == "TASK_COMPLEX_WANDER_STANDARD" )
    {
        // Grab the parameters for this task
        float fMoveState;
        float fDir;
        bool bWanderSensibly = true;
        GetParameterNumber ( "moveState", fMoveState );
        GetParameterNumber ( "dir", fDir );
        GetParameterBool ( "wanderSensibly", bWanderSensibly );

        // Default slots
        bTaskPrimary = true;
        iTaskPriority = TASK_PRIORITY_PRIMARY;

        // Create the task
        return g_pGame->GetTasks ()->CreateTaskComplexWanderStandard ( static_cast < int > ( fMoveState ),
                                                                       static_cast < char > ( fDir ),
                                                                       bWanderSensibly );
    }

    // No such task
    return NULL;
}
