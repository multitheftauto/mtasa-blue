/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTask.h
*  PURPOSE:     Task class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CCLIENTTASK_H
#define __CCLIENTTASK_H

#include <list>
#include "lua/LuaCommon.h"
#include "CClientEntity.h"

class CClientEntity;
class CClientManager;
class CClientPed;
class CElementArray;
class CLuaArgument;
class CPed;
class CTask;
class CVehicle;

class CClientTask
{
public:
                                CClientTask                 ( CClientManager* pManager );

    bool                        Read                        ( lua_State* luaVM, int iTableIndex, bool bClear );
    bool                        ReadElements                ( lua_State* luaVM, int iTableIndex, bool bClear );
    bool                        ReadParameters              ( lua_State* luaVM, int iTableIndex, bool bClear );
    bool                        Write                       ( lua_State* luaVM, int iTableIndex );
    bool                        WriteElements               ( lua_State* luaVM, int iTableIndex );
    bool                        WriteParameters             ( lua_State* luaVM, int iTableIndex );

    inline const char*          GetTaskName                 ( void )                { return m_strTaskName.c_str (); };
    inline void                 SetTaskName                 ( const char* szName )  { m_strTaskName = szName ? szName : ""; };

    static unsigned long        GenerateUniqueIdentifier    ( void );
    inline unsigned long        GetUniqueIdentifier         ( void )                { return m_ulUniqueIdentifier; };
    inline void                 SetUniqueIdentifier         ( unsigned long ulID )  { m_ulUniqueIdentifier = ulID; };

    void                        AddElement                  ( CClientEntity* pElement );
    void                        RemoveElement               ( CClientEntity* pElement );
    void                        ClearElements               ( void );

    bool                        IsElementIn                 ( CClientEntity* pElement );
    inline const std::list < ElementID >&   GetElements     ( void )    { return m_Elements; };

    inline const std::list <std::string>&   GetKeys         ( void )    { return m_Keys; };
    inline const std::list <CLuaArgument>&  GetValues       ( void )    { return m_Values; };
    void                        SetParameter                ( const char* szKey, const CLuaArgument& Value );
    void                        SetParameters               ( const std::list <std::string>& Keys, const std::list <CLuaArgument>& Values );
    void                        ClearParameters             ( void );

    CLuaArgument*               GetParameter                ( const char* szKey );
    bool                        GetParameterBool            ( const char* szKey, bool& Bool );
    bool                        GetParameterNumber          ( const char* szKey, float& Number );
    CPed*                       GetParameterPed             ( const char* szKey );
    const char*                 GetParameterString          ( const char* szKey );
    CVehicle*                   GetParameterVehicle         ( const char* szKey );

    bool                        ApplyTask                   ( CClientPed& Ped );

private:
    CTask*                      CreateTask                  ( bool& bTaskPrimary, int& iTaskPriority );

    CClientManager*             m_pManager;

    std::string                 m_strTaskName;
    unsigned long               m_ulUniqueIdentifier;

    std::list < ElementID >     m_Elements;
    std::list < std::string >   m_Keys;
    std::list < CLuaArgument >  m_Values;

    static unsigned long        m_ulLastUniqueIdentifier;
};

#endif
