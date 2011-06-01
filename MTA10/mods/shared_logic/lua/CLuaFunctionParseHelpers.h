/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        MTA10/mods/shared_logic/lua/CLuaFunctionParseHelpers.h
*  PURPOSE:
*  DEVELOPERS:  Nobody knows
*
*****************************************************************************/

// Forward declare enum reflection stuff
#include <gui/CGUI.h>
enum eLuaType { };
DECLARE_ENUM( eLuaType );
DECLARE_ENUM( CGUIVerticalAlign );
DECLARE_ENUM( CGUIHorizontalAlign );
DECLARE_ENUM( eInputMode );


// class -> class type
inline eClientEntityType GetClassType ( CClientCamera* )        { return CCLIENTCAMERA; }
inline eClientEntityType GetClassType ( CClientPlayer* )        { return CCLIENTPLAYER; }
inline eClientEntityType GetClassType ( CClientVehicle* )       { return CCLIENTVEHICLE; }
inline eClientEntityType GetClassType ( CClientRadarMarker* )   { return CCLIENTRADARMARKER; }
inline eClientEntityType GetClassType ( CClientObject* )        { return CCLIENTOBJECT; }
inline eClientEntityType GetClassType ( CClientCivilian* )      { return CCLIENTCIVILIAN; }
inline eClientEntityType GetClassType ( CClientPickup* )        { return CCLIENTPICKUP; }
inline eClientEntityType GetClassType ( CClientRadarArea* )     { return CCLIENTRADARAREA; }
inline eClientEntityType GetClassType ( CClientMarker* )        { return CCLIENTMARKER; }
inline eClientEntityType GetClassType ( CClientTeam* )          { return CCLIENTTEAM; }
inline eClientEntityType GetClassType ( CClientPed* )           { return CCLIENTPED; }
inline eClientEntityType GetClassType ( CClientProjectile* )    { return CCLIENTPROJECTILE; }
inline eClientEntityType GetClassType ( CClientGUIElement* )    { return CCLIENTGUI; }
inline eClientEntityType GetClassType ( CClientColShape* )      { return CCLIENTCOLSHAPE; }
inline eClientEntityType GetClassType ( CClientDummy* )         { return CCLIENTDUMMY; }
inline eClientEntityType GetClassType ( CScriptFile* )          { return SCRIPTFILE; }
inline eClientEntityType GetClassType ( CClientDFF* )           { return CCLIENTDFF; }
inline eClientEntityType GetClassType ( CClientColModel* )      { return CCLIENTCOL; }
inline eClientEntityType GetClassType ( CClientTXD* )           { return CCLIENTTXD; }
inline eClientEntityType GetClassType ( CClientSound* )         { return CCLIENTSOUND; }
inline eClientEntityType GetClassType ( CClientWater* )         { return CCLIENTWATER; }

inline eCGUIType GetClassType ( CGUIButton* )      { return CGUI_BUTTON; }
inline eCGUIType GetClassType ( CGUICheckBox* )    { return CGUI_CHECKBOX; }
inline eCGUIType GetClassType ( CGUIEdit* )        { return CGUI_EDIT; }
inline eCGUIType GetClassType ( CGUIGridList* )    { return CGUI_GRIDLIST; }
inline eCGUIType GetClassType ( CGUILabel* )       { return CGUI_LABEL; }
inline eCGUIType GetClassType ( CGUIMemo* )        { return CGUI_MEMO; }
inline eCGUIType GetClassType ( CGUIProgressBar* ) { return CGUI_PROGRESSBAR; }
inline eCGUIType GetClassType ( CGUIRadioButton* ) { return CGUI_RADIOBUTTON; }
inline eCGUIType GetClassType ( CGUIStaticImage* ) { return CGUI_STATICIMAGE; }
inline eCGUIType GetClassType ( CGUITab* )         { return CGUI_TAB; }
inline eCGUIType GetClassType ( CGUITabPanel* )    { return CGUI_TABPANEL; }
inline eCGUIType GetClassType ( CGUIWindow* )      { return CGUI_WINDOW; }
inline eCGUIType GetClassType ( CGUIScrollPane* )  { return CGUI_SCROLLPANE; }
inline eCGUIType GetClassType ( CGUIScrollBar* )   { return CGUI_SCROLLBAR; }
inline eCGUIType GetClassType ( CGUIComboBox* )    { return CGUI_COMBOBOX; }


// class -> class name
inline SString GetClassTypeName ( CClientCamera* )          { return "camera"; }
inline SString GetClassTypeName ( CClientPlayer* )          { return "player"; }
inline SString GetClassTypeName ( CClientVehicle* )         { return "vehicle"; }
inline SString GetClassTypeName ( CClientRadarMarker* )     { return "blip"; }
inline SString GetClassTypeName ( CClientObject* )          { return "object"; }
inline SString GetClassTypeName ( CClientCivilian* )        { return "civilian"; }
inline SString GetClassTypeName ( CClientPickup* )          { return "pickup"; }
inline SString GetClassTypeName ( CClientRadarArea* )       { return "radararea"; }
inline SString GetClassTypeName ( CClientMarker* )          { return "marker"; }
inline SString GetClassTypeName ( CClientTeam* )            { return "team"; }
inline SString GetClassTypeName ( CClientPed* )             { return "ped"; }
inline SString GetClassTypeName ( CClientProjectile* )      { return "projectile"; }
inline SString GetClassTypeName ( CClientGUIElement* )      { return "gui-element"; }
inline SString GetClassTypeName ( CClientColShape* )        { return "colshape"; }
inline SString GetClassTypeName ( CClientDummy* )           { return "dummy"; }
inline SString GetClassTypeName ( CScriptFile* )            { return "scriptfile"; }
inline SString GetClassTypeName ( CClientDFF* )             { return "dff"; }
inline SString GetClassTypeName ( CClientColModel* )        { return "col-model"; }
inline SString GetClassTypeName ( CClientTXD* )             { return "txd"; }
inline SString GetClassTypeName ( CClientSound* )           { return "sound"; }
inline SString GetClassTypeName ( CClientWater* )           { return "water"; }

inline SString GetClassTypeName ( CGUIButton* )      { return "gui-button"; }
inline SString GetClassTypeName ( CGUICheckBox* )    { return "gui-checkbox"; }
inline SString GetClassTypeName ( CGUIEdit* )        { return "gui-edit"; }
inline SString GetClassTypeName ( CGUIGridList* )    { return "gui-gridlist"; }
inline SString GetClassTypeName ( CGUILabel* )       { return "gui-label"; }
inline SString GetClassTypeName ( CGUIMemo* )        { return "gui-memo"; }
inline SString GetClassTypeName ( CGUIProgressBar* ) { return "gui-progressbar"; }
inline SString GetClassTypeName ( CGUIRadioButton* ) { return "gui-radiobutton"; }
inline SString GetClassTypeName ( CGUIStaticImage* ) { return "gui-staticimage"; }
inline SString GetClassTypeName ( CGUITab* )         { return "gui-tab"; }
inline SString GetClassTypeName ( CGUITabPanel* )    { return "gui-tabpanel"; }
inline SString GetClassTypeName ( CGUIWindow* )      { return "gui-window"; }
inline SString GetClassTypeName ( CGUIScrollPane* )  { return "gui-scrollpane"; }
inline SString GetClassTypeName ( CGUIScrollBar* )   { return "gui-scrollbar"; }
inline SString GetClassTypeName ( CGUIComboBox* )    { return "gui-combobox"; }

inline SString GetClassTypeName ( CXMLNode* )               { return "xml-node"; }


//
// CXMLNode from userdata
//
template < class T >
CXMLNode* UserDataCast ( CXMLNode*, void* ptr )
{
    return g_pCore->GetXML ()->GetNodeFromID ( reinterpret_cast < unsigned long > ( ptr ) );
}


//
// CClientEntity from userdata
//
template < class T >
CClientEntity* UserDataCast ( CClientEntity*, void* ptr )
{
    ElementID ID = TO_ELEMENTID ( ptr );
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );
    if ( !pEntity || pEntity->IsBeingDeleted () || pEntity->GetType () != GetClassType ( (T*)0 ) )
        return NULL;
    return pEntity;
}


//
// CClientGUIElement ( CGUIElement )
//
// Returns true if T is the same class as the one wrapped by pGuiElement
template < class T >
bool CheckWrappedUserDataType ( CClientGUIElement*& pGuiElement, SString& strErrorExpectedType )
{
    if ( pGuiElement->GetCGUIElement ()->GetType () == GetClassType ( (T*)0 ) )
        return true;
    strErrorExpectedType = GetClassTypeName ( (T*)0 );
    return false;
}
