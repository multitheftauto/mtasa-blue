/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/lua/CLuaFunctionParseHelpers.h
 *
 *****************************************************************************/

#pragma once

// Forward declare enum reflection stuff
#include <gui/CGUIEnumDefs.h>
#include <game/CAudioEngine.h>
#include <game/CRenderWare.h>
#include <game/CHud.h>
#include <game/CStreaming.h>
#include <type_traits>

enum eLuaType
{
};

DECLARE_ENUM(eLuaType);
DECLARE_ENUM(CGUIVerticalAlign);
DECLARE_ENUM(CGUIHorizontalAlign);
DECLARE_ENUM(eInputMode);
DECLARE_ENUM(eAccessType);
DECLARE_ENUM(TrafficLight::EColor);
DECLARE_ENUM(TrafficLight::EState);
DECLARE_ENUM(CEasingCurve::eType);
DECLARE_ENUM(eAmbientSoundType)
DECLARE_ENUM(eCGUIType);
DECLARE_ENUM(eDxTestMode)
DECLARE_ENUM(eWeaponType)
DECLARE_ENUM(eWeaponProperty)
DECLARE_ENUM(eWeaponSkill)
DECLARE_ENUM(ERenderFormat);
DECLARE_ENUM(ETextureType);
DECLARE_ENUM(ETextureAddress);
DECLARE_ENUM(EPixelsFormatType);
DECLARE_ENUM(EBlendModeType)
DECLARE_ENUM(EEntityTypeMask);
DECLARE_ENUM(eWeaponState);
DECLARE_ENUM(eWeaponFlags);
DECLARE_ENUM(eVehicleComponent);
DECLARE_ENUM(eObjectProperty);
DECLARE_ENUM(eObjectGroup::Modifiable);
DECLARE_ENUM(eObjectGroup::DamageEffect);
DECLARE_ENUM(eObjectGroup::CollisionResponse);
DECLARE_ENUM(eObjectGroup::FxType);
DECLARE_ENUM(eObjectGroup::BreakMode);
DECLARE_ENUM(eFontType);
DECLARE_ENUM(eFontQuality);
DECLARE_ENUM(eAudioLookupIndex);
DECLARE_ENUM(eAspectRatio);
DECLARE_ENUM(eRadioStreamIndex);
DECLARE_ENUM(EComponentBase::EComponentBaseType);
DECLARE_ENUM(eWebBrowserMouseButton);
DECLARE_ENUM(eTrayIconType)
DECLARE_ENUM(eCursorType)
DECLARE_ENUM(eWheelPosition)
DECLARE_ENUM(D3DPRIMITIVETYPE);
DECLARE_ENUM(eVehicleDummies);
DECLARE_ENUM_CLASS(eGrainMultiplierType);
DECLARE_ENUM_CLASS(eResizableVehicleWheelGroup);
DECLARE_ENUM(eSurfaceProperties);
DECLARE_ENUM(eSurfaceAudio);
DECLARE_ENUM(eSurfaceBulletEffect);
DECLARE_ENUM(eSurfaceWheelEffect);
DECLARE_ENUM(eSurfaceSkidMarkType);
DECLARE_ENUM(eSurfaceAdhesionGroup);
DECLARE_ENUM_CLASS(eClientModelType);
DECLARE_ENUM(eSoundEffectType);
DECLARE_ENUM_CLASS(eSoundEffectParams::Chorus);
DECLARE_ENUM_CLASS(eSoundEffectParams::Compressor);
DECLARE_ENUM_CLASS(eSoundEffectParams::Distortion);
DECLARE_ENUM_CLASS(eSoundEffectParams::Echo);
DECLARE_ENUM_CLASS(eSoundEffectParams::Flanger);
DECLARE_ENUM_CLASS(eSoundEffectParams::Gargle);
DECLARE_ENUM_CLASS(eSoundEffectParams::I3DL2Reverb);
DECLARE_ENUM_CLASS(eSoundEffectParams::ParamEq);
DECLARE_ENUM_CLASS(eSoundEffectParams::Reverb);
DECLARE_ENUM_CLASS(eModelIdeFlag);
DECLARE_ENUM_CLASS(_D3DFORMAT);
DECLARE_ENUM_CLASS(eRenderStage);
DECLARE_ENUM_CLASS(eFxParticleSystems);
DECLARE_ENUM(ePools);
DECLARE_ENUM(eWorldProperty);
DECLARE_ENUM_CLASS(eModelLoadState);
DECLARE_ENUM_CLASS(PreloadAreaOption);
DECLARE_ENUM_CLASS(taskType);
DECLARE_ENUM(eEntityType);


class CRemoteCall;

enum eDXHorizontalAlign
{
    DX_ALIGN_LEFT = DT_LEFT,
    DX_ALIGN_HCENTER = DT_CENTER,
    DX_ALIGN_RIGHT = DT_RIGHT,
};
DECLARE_ENUM(eDXHorizontalAlign);

enum eDXVerticalAlign
{
    DX_ALIGN_TOP = DT_TOP,
    DX_ALIGN_VCENTER = DT_VCENTER,
    DX_ALIGN_BOTTOM = DT_BOTTOM,
};
DECLARE_ENUM(eDXVerticalAlign);

DECLARE_ENUM(eHudComponent);
DECLARE_ENUM_CLASS(eHudComponentProperty);
DECLARE_ENUM_CLASS(eFontStyle);
DECLARE_ENUM_CLASS(eFontAlignment);

enum eFieldOfViewMode
{
    FOV_MODE_PLAYER,
    FOV_MODE_VEHICLE,
    FOV_MODE_VEHICLE_MAX,
    FOV_MODE_AIMING
};
DECLARE_ENUM(eFieldOfViewMode);

#include "json.h"
// Prettify toJSON (see mantis #9210)
enum eJSONPrettyType
{
    JSONPRETTY_SPACES = JSON_C_TO_STRING_PRETTY,
    JSONPRETTY_NONE = -1,
    JSONPRETTY_TABS = JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_PRETTY_TAB
};
DECLARE_ENUM(eJSONPrettyType);

// class -> class type
inline eCGUIType GetClassType(CGUIButton*)
{
    return CGUI_BUTTON;
}
inline eCGUIType GetClassType(CGUICheckBox*)
{
    return CGUI_CHECKBOX;
}
inline eCGUIType GetClassType(CGUIEdit*)
{
    return CGUI_EDIT;
}
inline eCGUIType GetClassType(CGUIGridList*)
{
    return CGUI_GRIDLIST;
}
inline eCGUIType GetClassType(CGUILabel*)
{
    return CGUI_LABEL;
}
inline eCGUIType GetClassType(CGUIMemo*)
{
    return CGUI_MEMO;
}
inline eCGUIType GetClassType(CGUIProgressBar*)
{
    return CGUI_PROGRESSBAR;
}
inline eCGUIType GetClassType(CGUIRadioButton*)
{
    return CGUI_RADIOBUTTON;
}
inline eCGUIType GetClassType(CGUIStaticImage*)
{
    return CGUI_STATICIMAGE;
}
inline eCGUIType GetClassType(CGUITab*)
{
    return CGUI_TAB;
}
inline eCGUIType GetClassType(CGUITabPanel*)
{
    return CGUI_TABPANEL;
}
inline eCGUIType GetClassType(CGUIWindow*)
{
    return CGUI_WINDOW;
}
inline eCGUIType GetClassType(CGUIScrollPane*)
{
    return CGUI_SCROLLPANE;
}
inline eCGUIType GetClassType(CGUIScrollBar*)
{
    return CGUI_SCROLLBAR;
}
inline eCGUIType GetClassType(CGUIComboBox*)
{
    return CGUI_COMBOBOX;
}
inline eCGUIType GetClassType(CGUIWebBrowser*)
{
    return CGUI_WEBBROWSER;
}

// class -> class name
inline SString GetClassTypeName(CClientEntity*)
{
    return "element";
}
inline SString GetClassTypeName(CClientCamera*)
{
    return "camera";
}
inline SString GetClassTypeName(CClientPlayer*)
{
    return "player";
}
inline SString GetClassTypeName(CClientVehicle*)
{
    return "vehicle";
}
inline SString GetClassTypeName(CClientRadarMarker*)
{
    return "blip";
}
inline SString GetClassTypeName(CClientObject*)
{
    return "object";
}
inline SString GetClassTypeName(CClientPickup*)
{
    return "pickup";
}
inline SString GetClassTypeName(CClientRadarArea*)
{
    return "radararea";
}
inline SString GetClassTypeName(CClientMarker*)
{
    return "marker";
}
inline SString GetClassTypeName(CClientTeam*)
{
    return "team";
}
inline SString GetClassTypeName(CClientPed*)
{
    return "ped";
}
inline SString GetClassTypeName(CRemoteCall*)
{
    return "request";
}
inline SString GetClassTypeName(CClientProjectile*)
{
    return "projectile";
}
inline SString GetClassTypeName(CClientGUIElement*)
{
    return "gui-element";
}
inline SString GetClassTypeName(CClientColShape*)
{
    return "colshape";
}
inline SString GetClassTypeName(CClientDummy*)
{
    return "dummy";
}
inline SString GetClassTypeName(CScriptFile*)
{
    return "scriptfile";
}
inline SString GetClassTypeName(CClientDFF*)
{
    return "dff";
}
inline SString GetClassTypeName(CClientColModel*)
{
    return "col-model";
}
inline SString GetClassTypeName(CClientTXD*)
{
    return "txd";
}
inline SString GetClassTypeName(CClientIMG*)
{
    return "img";
}
inline SString GetClassTypeName(CClientBuilding*)
{
    return "building";
}
inline SString GetClassTypeName(CClientSound*)
{
    return "sound";
}
inline SString GetClassTypeName(CClientWater*)
{
    return "water";
}
inline SString GetClassTypeName(CClientDxFont*)
{
    return "dx-font";
}
inline SString GetClassTypeName(CClientGuiFont*)
{
    return "gui-font";
}
inline SString GetClassTypeName(CClientMaterial*)
{
    return "material";
}
inline SString GetClassTypeName(CClientRenderTarget*)
{
    return "render-target-texture";
}
inline SString GetClassTypeName(CClientScreenSource*)
{
    return "screen-source-texture";
}
inline SString GetClassTypeName(CClientTexture*)
{
    return "texture";
}
inline SString GetClassTypeName(CClientWebBrowser*)
{
    return "browser";
}
inline SString GetClassTypeName(CClientWeapon*)
{
    return "weapon";
}
inline SString GetClassTypeName(CClientEffect*)
{
    return "effect";
}
inline SString GetClassTypeName(CClientPointLights*)
{
    return "light";
}

inline SString GetClassTypeName(CGUIButton*)
{
    return "gui-button";
}
inline SString GetClassTypeName(CGUICheckBox*)
{
    return "gui-checkbox";
}
inline SString GetClassTypeName(CGUIEdit*)
{
    return "gui-edit";
}
inline SString GetClassTypeName(CGUIGridList*)
{
    return "gui-gridlist";
}
inline SString GetClassTypeName(CGUILabel*)
{
    return "gui-label";
}
inline SString GetClassTypeName(CGUIMemo*)
{
    return "gui-memo";
}
inline SString GetClassTypeName(CGUIProgressBar*)
{
    return "gui-progressbar";
}
inline SString GetClassTypeName(CGUIRadioButton*)
{
    return "gui-radiobutton";
}
inline SString GetClassTypeName(CGUIStaticImage*)
{
    return "gui-staticimage";
}
inline SString GetClassTypeName(CGUITab*)
{
    return "gui-tab";
}
inline SString GetClassTypeName(CGUITabPanel*)
{
    return "gui-tabpanel";
}
inline SString GetClassTypeName(CGUIWindow*)
{
    return "gui-window";
}
inline SString GetClassTypeName(CGUIScrollPane*)
{
    return "gui-scrollpane";
}
inline SString GetClassTypeName(CGUIScrollBar*)
{
    return "gui-scrollbar";
}
inline SString GetClassTypeName(CGUIComboBox*)
{
    return "gui-combobox";
}
inline SString GetClassTypeName(CGUIWebBrowser*)
{
    return "gui-browser";
}

inline SString GetClassTypeName(CResource*)
{
    return "resource-data";
}
inline SString GetClassTypeName(CXMLNode*)
{
    return "xml-node";
}
inline SString GetClassTypeName(CLuaTimer*)
{
    return "lua-timer";
}
inline SString GetClassTypeName(CEntity*)
{
    return "entity";
}
inline SString GetClassTypeName(CLuaVector2D*)
{
    return "vector2";
}
inline SString GetClassTypeName(CLuaVector3D*)
{
    return "vector3";
}
inline SString GetClassTypeName(CLuaVector4D*)
{
    return "vector4";
}
inline SString GetClassTypeName(CLuaMatrix*)
{
    return "matrix";
}
inline SString GetClassTypeName(D3DPRIMITIVETYPE*)
{
    return "primitive-type";
}
inline SString GetClassTypeName(eVehicleDummies*)
{
    return "vehicle-dummy";
}
inline SString GetClassTypeName(eSurfaceProperties*)
{
    return "surface-property-type";
}
inline SString GetClassTypeName(eSurfaceAudio*)
{
    return "surface-audio-type";
}
inline SString GetClassTypeName(eSurfaceBulletEffect*)
{
    return "surface-bullet-effect";
}
inline SString GetClassTypeName(eSurfaceWheelEffect*)
{
    return "surface-wheel-effect";
}
inline SString GetClassTypeName(eSurfaceSkidMarkType*)
{
    return "surface-skidmark-type";
}
inline SString GetClassTypeName(eSurfaceAdhesionGroup*)
{
    return "surface-adhesion-group";
}
inline SString GetClassTypeName(eSoundEffectType*)
{
    return "soundeffect-type";
}
inline SString GetClassTypeName(eSoundEffectParams::Chorus*)
{
    return "soundeffect-params-chorus";
}
inline SString GetClassTypeName(eSoundEffectParams::Compressor*)
{
    return "soundeffect-params-compressor";
}
inline SString GetClassTypeName(eSoundEffectParams::Distortion*)
{
    return "soundeffect-params-distortion";
}
inline SString GetClassTypeName(eSoundEffectParams::Echo*)
{
    return "soundeffect-params-echo";
}
inline SString GetClassTypeName(eSoundEffectParams::Flanger*)
{
    return "soundeffect-params-flanger";
}
inline SString GetClassTypeName(eSoundEffectParams::Gargle*)
{
    return "soundeffect-params-gargle";
}
inline SString GetClassTypeName(eSoundEffectParams::I3DL2Reverb*)
{
    return "soundeffect-params-i3dl2reverb";
}
inline SString GetClassTypeName(eSoundEffectParams::ParamEq*)
{
    return "soundeffect-params-parameq";
}
inline SString GetClassTypeName(eSoundEffectParams::Reverb*)
{
    return "soundeffect-params-reverb";
}
inline SString GetClassTypeName(eWorldProperty*)
{
    return "world-property";
}

inline SString GetClassTypeName(CClientVectorGraphic*)
{
    return "svg";
}

//
// CResource from userdata
//
CResource* UserDataCast(CResource* ptr, lua_State* luaState);

//
// CXMLNode from userdata
//
CXMLNode* UserDataCast(CXMLNode* ptr, lua_State* luaState);

//
// CLuaTimer from userdata
//
CLuaTimer* UserDataCast(CLuaTimer* ptr, lua_State* luaState);

//
// CLuaVector2D from userdata
//
CLuaVector2D* UserDataCast(CLuaVector2D* ptr, lua_State* luaState);

//
// CLuaVector3D from userdata
//
CLuaVector3D* UserDataCast(CLuaVector3D* ptr, lua_State* luaState);

//
// CLuaVector4D from userdata
//
CLuaVector4D* UserDataCast(CLuaVector4D* ptr, lua_State* luaState);

//
// CLuaMatrix from userdata
//
CLuaMatrix* UserDataCast(CLuaMatrix* ptr, lua_State* luaState);

//
// CClientEntity from userdata
//
CClientEntity* UserDataToElementCast(CClientEntity* ptr, SharedUtil::ClassId classId, lua_State* luaState);

template <typename T, typename = std::enable_if_t<std::is_base_of_v<CClientEntity, T>>>
T* UserDataCast(T* ptr, lua_State* luaState)
{
    return reinterpret_cast<T*>(UserDataToElementCast(ptr, T::GetClassId(), luaState));
}

//
// CRemoteCall from userdata
//
CRemoteCall* UserDataCast(CRemoteCall* ptr, lua_State* luaState);

//
// CClientGUIElement ( CGUIElement )
//
// Returns true if T is the same class as the one wrapped by pGuiElement
template <class T>
bool CheckWrappedUserDataType(CClientGUIElement*& pGuiElement, SString& strErrorExpectedType)
{
    if (pGuiElement->GetCGUIElement()->GetType() == GetClassType((T*)0))
        return true;
    strErrorExpectedType = GetClassTypeName((T*)0);
    return false;
}

SString GetUserDataClassName(void* ptr, lua_State* luaVM, bool bFindElementType = true);

//
// Reading mixed types
//
class CScriptArgReader;
void MixedReadDxFontString(CScriptArgReader& argStream, eFontType& outFontType, eFontType defaultFontType, CClientDxFont*& poutDxFontElement);
void MixedReadGuiFontString(CScriptArgReader& argStream, SString& strFontName, const char* szDefaultFontName, CClientGuiFont*& poutGuiFontElement);
void MixedReadMaterialString(CScriptArgReader& argStream, CClientMaterial*& pMaterialElement);
bool IsValidMatrixLuaTable(lua_State* luaVM, std::uint32_t argIndex) noexcept;
bool ReadMatrix(lua_State* luaVM, uint uiArgIndex, CMatrix& outMatrix);
void MinClientReqCheck(lua_State* luaVM, const char* szVersionReq, const char* szReason);
bool MinClientReqCheck(CScriptArgReader& argStream, const char* szVersionReq, const char* szReason = nullptr);
void ReadPregFlags(CScriptArgReader& argStream, pcrecpp::RE_Options& pOptions);

//
// Resource access helpers
//
void CheckCanModifyOtherResource(CScriptArgReader& argStream, CResource* pThisResource, CResource* pOtherResource);
void CheckCanModifyOtherResources(CScriptArgReader& argStream, CResource* pThisResource, std::initializer_list<CResource*> resourceList);
void CheckCanAccessOtherResourceFile(CScriptArgReader& argStream, CResource* pThisResource, CResource* pOtherResource, const SString& strAbsPath,
                                     bool* pbReadOnly = nullptr);

//
// Other misc helpers
//
bool IsWeaponPropertyFlag(eWeaponProperty weaponProperty);
uint GetWeaponPropertyFlagBit(eWeaponProperty weaponProperty);
