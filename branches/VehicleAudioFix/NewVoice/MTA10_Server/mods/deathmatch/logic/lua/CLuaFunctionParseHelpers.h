/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        MTA10_Server/mods/deathmatch/logic/lua/CLuaFunctionParseHelpers.h
*  PURPOSE:
*  DEVELOPERS:  Nobody knows
*
*****************************************************************************/

#include "CElementIDs.h"

// Forward declare enum reflection stuff
enum eLuaType { };
DECLARE_ENUM( eLuaType );
DECLARE_ENUM( TrafficLight::EColor );
DECLARE_ENUM( TrafficLight::EState );
DECLARE_ENUM( CEasingCurve::eType );

enum eHudComponent
{
    HUD_AMMO,
    HUD_WEAPON,
    HUD_HEALTH,
    HUD_BREATH,
    HUD_ARMOUR,
    HUD_MONEY,
    HUD_VEHICLE_NAME,
    HUD_AREA_NAME,
    HUD_RADAR,
    HUD_CLOCK,
    HUD_RADIO,
    HUD_WANTED,
    HUD_CROSSHAIR,
    HUD_ALL,
};
DECLARE_ENUM( eHudComponent );


// class -> class type
typedef int eEntityType;
inline eEntityType GetClassType ( CElement* )       { return -1; }
inline eEntityType GetClassType ( CPlayer* )        { return CElement::PLAYER; }
inline eEntityType GetClassType ( CVehicle* )       { return CElement::VEHICLE; }
inline eEntityType GetClassType ( CBlip* )          { return CElement::BLIP; }
inline eEntityType GetClassType ( CObject* )        { return CElement::OBJECT; }
inline eEntityType GetClassType ( CPickup* )        { return CElement::PICKUP; }
inline eEntityType GetClassType ( CRadarArea* )     { return CElement::RADAR_AREA; }
inline eEntityType GetClassType ( CMarker* )        { return CElement::MARKER; }
inline eEntityType GetClassType ( CTeam* )          { return CElement::TEAM; }
inline eEntityType GetClassType ( CPed* )           { return CElement::PED; }
inline eEntityType GetClassType ( CColShape* )      { return CElement::COLSHAPE; }
inline eEntityType GetClassType ( CDummy* )         { return CElement::DUMMY; }
inline eEntityType GetClassType ( CScriptFile* )    { return CElement::SCRIPTFILE; }
inline eEntityType GetClassType ( CWater* )         { return CElement::WATER; }


// class -> class name
inline SString GetClassTypeName ( CElement* )       { return "element"; }
inline SString GetClassTypeName ( CPlayer* )        { return "player"; }
inline SString GetClassTypeName ( CVehicle* )       { return "vehicle"; }
inline SString GetClassTypeName ( CBlip* )          { return "blip"; }
inline SString GetClassTypeName ( CObject* )        { return "object"; }
inline SString GetClassTypeName ( CPickup* )        { return "pickup"; }
inline SString GetClassTypeName ( CRadarArea* )     { return "radararea"; }
inline SString GetClassTypeName ( CMarker* )        { return "marker"; }
inline SString GetClassTypeName ( CTeam* )          { return "team"; }
inline SString GetClassTypeName ( CPed* )           { return "ped"; }
inline SString GetClassTypeName ( CColShape* )      { return "colshape"; }
inline SString GetClassTypeName ( CDummy* )         { return "dummy"; }
inline SString GetClassTypeName ( CScriptFile* )    { return "scriptfile"; }
inline SString GetClassTypeName ( CWater* )         { return "water"; }

inline SString GetClassTypeName ( CXMLNode* )       { return "xml-node"; }
inline SString GetClassTypeName ( CLuaTimer* )      { return "lua-timer"; }
inline SString GetClassTypeName ( CAccount* )       { return "account"; }


//
// CXMLNode from userdata
//
template < class T >
CXMLNode* UserDataCast ( CXMLNode*, void* ptr, lua_State* )
{
    return g_pServerInterface->GetXML ()->GetNodeFromID ( reinterpret_cast < unsigned long > ( ptr ) );
}


//
// CLuaTimer from userdata
//
template < class T >
CLuaTimer* UserDataCast ( CLuaTimer*, void* ptr, lua_State* luaVM )
{
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        CLuaTimer* pLuaTimer = reinterpret_cast < CLuaTimer* > ( ptr );
        if ( pLuaMain->GetTimerManager ()->Exists ( pLuaTimer ) )
            return pLuaTimer;
    }
    return NULL;
}


//
// CAccount from userdata
//
template < class T >
CAccount* UserDataCast ( CAccount*, void* ptr, lua_State* luaVM )
{
    CAccount* pAccount = reinterpret_cast < CAccount* > ( ptr );
    if ( g_pGame->GetAccountManager ()->Exists ( pAccount ) )
        return pAccount;
    return NULL;
}


//
// CElement from userdata
//
template < class T >
CElement* UserDataCast ( CElement*, void* ptr, lua_State* )
{
    ElementID ID = TO_ELEMENTID ( ptr );
    CElement* pElement = CElementIDs::GetElement ( ID );
    if ( !pElement || pElement->IsBeingDeleted () || ( pElement->GetType () != GetClassType ( (T*)0 ) && GetClassType ( (T*)0 ) != -1 ) )
        return NULL;
    return pElement;
}


//
// CElement ( something )
//
// Returns true if T is the same class as the one wrapped by pElement
template < class T >
bool CheckWrappedUserDataType ( CElement*& pElement, SString& strErrorExpectedType )
{
    // Not used server side (yet)
    strErrorExpectedType = GetClassTypeName ( (T*)0 );
    return false;
}
