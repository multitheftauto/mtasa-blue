/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/LuaCommon.cpp
*  PURPOSE:     Lua common functions
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

#define TO_ELEMENTID(x) ((ElementID) reinterpret_cast < unsigned long > (x) )

// Lua push/pop macros for our datatypes
CElement* lua_toelement ( lua_State* luaVM, int iArgument )
{
    if ( lua_type ( luaVM, iArgument ) == LUA_TLIGHTUSERDATA )
    {
        ElementID ID = TO_ELEMENTID ( lua_touserdata ( luaVM, iArgument ) );
        CElement* pElement = CElementIDs::GetElement ( ID );
        if ( !pElement || pElement->IsBeingDeleted () )
            return NULL;
        return pElement;
    }

    return NULL;
}


CAccount* lua_toaccount ( lua_State* luaVM, int iArgument )
{
    CAccount* pAccount = reinterpret_cast < CAccount* > ( lua_touserdata ( luaVM, iArgument ) );
    if ( g_pGame->GetAccountManager ()->Exists ( pAccount ) )
        return pAccount;

    return NULL;
}


CAccessControlList* lua_toacl ( lua_State* luaVM, int iArgument )
{
    CAccessControlList* pACL = reinterpret_cast < CAccessControlList* > ( lua_touserdata ( luaVM, iArgument ) );
    if ( g_pGame->GetACLManager ()->VerifyACL ( pACL ) )
        return pACL;

    return NULL;
}


CAccessControlListGroup* lua_toaclgroup ( lua_State* luaVM, int iArgument )
{
    CAccessControlListGroup* pACLGroup = reinterpret_cast < CAccessControlListGroup* > ( lua_touserdata ( luaVM, iArgument ) );
    if ( g_pGame->GetACLManager ()->VerifyGroup ( pACLGroup ) )
        return pACLGroup;

    return NULL;
}


CBlip* lua_toblip ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_BLIP ( pElement ) )
        return static_cast < CBlip* > ( pElement );
    else
        return NULL;
}


CColShape* lua_tocolshape ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_COLSHAPE ( pElement ) )
        return static_cast < CColShape* > ( pElement );
    else
        return NULL;
}


CScriptFile* lua_tofile ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_FILE ( pElement ) )
        return static_cast < CScriptFile* > ( pElement );
    else
        return NULL;
}


CMarker* lua_tomarker ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_MARKER ( pElement ) )
        return static_cast < CMarker* > ( pElement );
    else
        return NULL;
}


CObject* lua_toobject ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_OBJECT ( pElement ) )
        return static_cast < CObject* > ( pElement );
    else
        return NULL;
}


CPed* lua_toped ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_PED ( pElement ) )
        return static_cast < CPed* > ( pElement );
    else
        return NULL;
}


CPickup* lua_topickup ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_PICKUP ( pElement ) )
        return static_cast < CPickup* > ( pElement );
    else
        return NULL;
}


CPlayer* lua_toplayer ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_PLAYER ( pElement ) )
        return static_cast < CPlayer* > ( pElement );
    else
        return NULL;
}


CResource* lua_toresource ( lua_State* luaVM, int iArgument )
{
    CResource* pResource = reinterpret_cast < CResource* > ( lua_touserdata ( luaVM, iArgument ) );
    if ( g_pGame->GetResourceManager ()->Exists ( pResource ) )
        return pResource;

    return NULL;
}


CRadarArea* lua_toradararea ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_RADAR_AREA ( pElement ) )
        return static_cast < CRadarArea* > ( pElement );
    else
        return NULL;
}


CTeam* lua_toteam ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_TEAM ( pElement ) )
        return static_cast < CTeam* > ( pElement );
    else
        return NULL;
}


CTextDisplay* lua_totextdisplay ( lua_State* luaVM, int iArgument )
{
    CLuaMain* pLuaVM = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( pLuaVM )
    {
        CTextDisplay* pDisplay = reinterpret_cast < CTextDisplay* > ( lua_touserdata ( luaVM, iArgument ) );
        if ( pLuaVM->TextDisplayExists ( pDisplay ) )
        {
            return pDisplay;
        }
    }

    return NULL;
}


CTextItem* lua_totextitem ( lua_State* luaVM, int iArgument )
{
    CLuaMain* pLuaVM = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( pLuaVM )
    {
        CTextItem* pItem = reinterpret_cast < CTextItem* > ( lua_touserdata ( luaVM, iArgument ) );
        if ( pLuaVM->TextItemExists ( pItem ) )
        {
            return pItem;
        }
    }

    return NULL;
}


CLuaTimer* lua_totimer ( lua_State* luaVM, int iArgument )
{
    CLuaMain* pLuaVM = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( pLuaVM )
    {
        CLuaTimer* pTimer = reinterpret_cast < CLuaTimer* > ( lua_touserdata ( luaVM, iArgument ) );
        if ( pLuaVM->GetTimerManager ()->Exists ( pTimer ) )
        {
            return pTimer;
        }
    }

    return NULL;
}


CVehicle* lua_tovehicle ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_VEHICLE ( pElement ) )
        return static_cast < CVehicle* > ( pElement );
    else
        return NULL;
}


CXMLNode* lua_toxmlnode ( lua_State* luaVM, int iArgument )
{
    return g_pServerInterface->GetXML ()->GetNodeFromID ( reinterpret_cast < unsigned long > ( lua_touserdata ( luaVM, iArgument ) ) );
}


CBan* lua_toban ( lua_State* luaVM, int iArgument )
{
    CBan* pBan = reinterpret_cast < CBan* > ( lua_touserdata ( luaVM, iArgument ) );
    if ( g_pGame->GetBanManager ()->Exists ( pBan ) )
        return pBan;

    return NULL;
}


CWater* lua_towater ( lua_State* luaVM, int iArgument )
{
    CElement* pElement = lua_toelement ( luaVM, iArgument );
    if ( pElement && IS_WATER ( pElement ) )
        return static_cast < CWater* > ( pElement );
    else
        return NULL;
}


void lua_pushelement ( lua_State* luaVM, CElement* pElement )
{
    if ( pElement )
    {
        ElementID ID = pElement->GetID ();
        if ( ID != INVALID_ELEMENT_ID )
        {
            lua_pushlightuserdata ( luaVM, (void*) ID );
            return;
        }
    }

    lua_pushnil ( luaVM );
}


void lua_pushacl ( lua_State* luaVM, CAccessControlList* pACL )
{
    lua_pushlightuserdata ( luaVM, pACL );
}


void lua_pushaclgroup ( lua_State* luaVM, CAccessControlListGroup* pACL )
{
    lua_pushlightuserdata ( luaVM, pACL );
}


void lua_pushaccount ( lua_State* luaVM, CAccount* pAccount )
{
    lua_pushlightuserdata ( luaVM, pAccount );
}


void lua_pushresource ( lua_State* luaVM, CResource* pElement )
{
    lua_pushlightuserdata ( luaVM, pElement );
}


void lua_pushtextdisplay ( lua_State* luaVM, CTextDisplay* pDisplay )
{
    lua_pushlightuserdata ( luaVM, pDisplay );
}


void lua_pushtextitem ( lua_State* luaVM, CTextItem* pItem )
{
    lua_pushlightuserdata ( luaVM, pItem );
}


void lua_pushtimer ( lua_State* luaVM, CLuaTimer* pElement )
{
    lua_pushlightuserdata ( luaVM, pElement );
}


void lua_pushxmlnode ( lua_State* luaVM, CXMLNode* pElement )
{
    lua_pushlightuserdata ( luaVM, reinterpret_cast < void* > ( pElement->GetID () ) );
}


void lua_pushban ( lua_State* luaVM, CBan* pBan )
{
    lua_pushlightuserdata ( luaVM, pBan );
}
