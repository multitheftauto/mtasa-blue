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

// Forward declare enum reflection stuff
DECLARE_ENUM( enum eLuaType );


// class -> class type
typedef int eEntityType;
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


// Dynamic cast - Returns NULL if it can't be done
template < class T >
T* ElementCast ( CElement* pElement )
{
    if ( pElement && pElement->GetType () == GetClassType ( (T*)0 ) )
        return static_cast < T* > ( pElement );
    return NULL;
}
