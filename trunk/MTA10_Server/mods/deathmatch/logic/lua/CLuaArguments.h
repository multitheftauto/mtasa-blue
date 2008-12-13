/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaArguments.h
*  PURPOSE:     Lua argument manager class
*  DEVELOPERS:  Ed Lyons <>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUAARGUMENTS_H
#define __CLUAARGUMENTS_H

extern "C"
{
    #include "lua.h"
}

#include "CLuaArgument.h"
#include <vector>
#include "../common/CBitStream.h"
#include "json.h"

class CAccessControlList;
class CAccessControlListGroup;
class CAccount;
class CElement;
class CLuaTimer;
class CResource;
class CTextDisplay;
class CTextItem;

class CLuaArguments;

class CLuaArguments
{
public:
                                                        CLuaArguments       ( void )                { }
                                                        CLuaArguments       ( const CLuaArguments& Arguments, std::map < CLuaArguments*, CLuaArguments* > * pKnownTables = NULL );
														CLuaArguments		( NetServerBitStreamInterface& bitStream, std::vector < CLuaArguments* > * pKnownTables = NULL );
                                                        ~CLuaArguments      ( void )                { DeleteArguments (); };

    void                                                CopyRecursive       ( const CLuaArguments& Arguments, std::map < CLuaArguments*, CLuaArguments* > * pKnownTables = NULL );

    const CLuaArguments&                                operator =          ( const CLuaArguments& Arguments );
	CLuaArgument*										operator []			( const unsigned int uiPosition ) const;

    void                                                ReadArgument        ( lua_State* luaVM, signed int uiIndex );
    void                                                ReadArguments       ( lua_State* luaVM, signed int uiIndexBegin = 1 );
    void                                                PushArguments       ( lua_State* luaVM ) const;
    void                                                PushArguments       ( CLuaArguments& Arguments );
    bool                                                Call                ( class CLuaMain* pLuaMain, int iLuaFunction, CLuaArguments * returnValues = NULL ) const;
	bool                                                CallGlobal          ( class CLuaMain* pLuaMain, const char* szFunction, CLuaArguments * returnValues = NULL ) const;

    void                                                ReadTable           ( lua_State* luaVM, int iIndexBegin, std::map < const void*, CLuaArguments* > * pKnownTables = NULL );
    void                                                PushAsTable         ( lua_State* luaVM, std::map < CLuaArguments*, int > * pKnownTables = NULL );

    CLuaArgument*                                       PushNil             ( void );
    CLuaArgument*                                       PushBoolean         ( bool bBool );
    CLuaArgument*                                       PushNumber          ( double dNumber );
    CLuaArgument*                                       PushString          ( const char* szString );
    CLuaArgument*                                       PushUserData        ( void* pUserData );
    CLuaArgument*                                       PushElement         ( CElement* pElement );
    CLuaArgument*                                       PushACL             ( CAccessControlList* pACL );
    CLuaArgument*                                       PushACLGroup        ( CAccessControlListGroup* pACLGroup );
    CLuaArgument*                                       PushAccount         ( CAccount* pAccount );
    CLuaArgument*                                       PushResource        ( CResource* pResource );
    CLuaArgument*                                       PushTextDisplay     ( CTextDisplay* pTextDisplay );
    CLuaArgument*                                       PushTextItem        ( CTextItem* pTextItem );
    CLuaArgument*                                       PushTimer           ( CLuaTimer* pLuaTimer );

    CLuaArgument*                                       PushArgument        ( CLuaArgument& argument );
    CLuaArgument*                                       PushTable           ( CLuaArguments * table );

    void                                                DeleteArguments     ( void );

    bool                                                ReadFromBitStream   ( NetServerBitStreamInterface& bitStream, std::vector < CLuaArguments* > * pKnownTables = NULL );
    bool                                                ReadFromJSONString  ( const char* szJSON );
    bool                                                WriteToBitStream    ( NetServerBitStreamInterface& bitStream, std::map < CLuaArguments*, unsigned long > * pKnownTables = NULL ) const;
    std::vector < char * > *                            WriteToCharVector   ( std::vector < char * > * values );
    bool                                                WriteToJSONString   ( std::string& strJSON, bool bSerialize = false );
    json_object *                                       WriteTableToJSONObject ( bool bSerialize = false, std::map < CLuaArguments*, unsigned long > * pKnownTables = NULL );
    json_object *                                       WriteToJSONArray    ( bool bSerialize );
    bool                                                ReadFromJSONObject  ( json_object * object, std::vector < CLuaArguments* > * pKnownTables = NULL );
    bool                                                ReadFromJSONArray   ( json_object * object, std::vector < CLuaArguments* > * pKnownTables = NULL );

    unsigned int                                        Count               ( void ) const          { return static_cast < unsigned int > ( m_Arguments.size () ); };
    std::vector < CLuaArgument* > ::const_iterator      IterBegin           ( void )                { return m_Arguments.begin (); };
    std::vector < CLuaArgument* > ::const_iterator      IterEnd             ( void )                { return m_Arguments.end (); };

private:
    std::vector < CLuaArgument* >                       m_Arguments;
};

#endif
