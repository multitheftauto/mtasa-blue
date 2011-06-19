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


//
// CXMLNode from userdata
//
template < class T >
CXMLNode* UserDataCast ( CXMLNode*, void* ptr )
{
    return g_pServerInterface->GetXML ()->GetNodeFromID ( reinterpret_cast < unsigned long > ( ptr ) );
}


//
// CElement from userdata
//
template < class T >
CElement* UserDataCast ( CElement*, void* ptr )
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
