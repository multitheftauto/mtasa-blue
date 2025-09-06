/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CKeyBindsInterface.h
 *  PURPOSE:     Keybind manager interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

// Beware: this class is poorly written and depends on game while it shouldn't
#include <game/CControllerConfigManager.h>

#include <list>
#include <ksignals/ksignals.h>

class CKeyFunctionBind;
class CControlFunctionBind;
struct SBindableKey;

typedef void (*KeyFunctionBindHandler)(CKeyFunctionBind* pBind);
typedef void (*ControlFunctionBindHandler)(CControlFunctionBind* pBind);
typedef bool (*KeyStrokeHandler)(const SString& strKey, bool bState, bool bIsConsoleInputKey);
typedef bool (*CharacterKeyHandler)(WPARAM wChar);

enum eKeyData
{
    DATA_NONE = 0,
    DATA_EXTENDED,
    DATA_NOT_EXTENDED,
    DATA_NUMPAD,
};

struct SBindableKey
{
    const char    szKey[20];
    unsigned long ulCode;
    int           iGTARelative;
    eKeyData      data;
    unsigned char ucNumpadRelative;
    bool          bIgnoredByGUI;
};

enum eControlType
{
    CONTROL_FOOT,
    CONTROL_VEHICLE,
    CONTROL_BOTH
};

struct SBindableGTAControl
{
    const char        szControl[25];
    eControllerAction action;
    eControlType      controlType;
    bool              bState;
    bool              bEnabled;
    const char*       szDescription;
};

enum class KeyBindType
{
    UNDEFINED,
    COMMAND,                     // bind key function args
    GTA_CONTROL,                 // bind key gta_control
    FUNCTION,                    // script bind key to function
    CONTROL_FUNCTION,            // script bind gta_control to function (pressing control, calls function)
};

class CKeyBind
{
public:
    explicit CKeyBind(KeyBindType type_ = KeyBindType::UNDEFINED) : type(type_) {}

    virtual ~CKeyBind() = default;

public:
    const KeyBindType   type;
    const SBindableKey* boundKey{nullptr};
    bool                isBeingDeleted{false};
    bool                isActive{true};
};

class CKeyBindWithState : public CKeyBind
{
public:
    using CKeyBind::CKeyBind;

public:
    bool state{false};
    bool triggerState{false};            // true == "down", false == "up"
};

class CCommandBind : public CKeyBindWithState
{
public:
    CCommandBind() : CKeyBindWithState(KeyBindType::COMMAND) {}

public:
    std::string command;
    std::string arguments;
    std::string resource;
    std::string originalScriptKey;            // Original key set by script
    bool        wasCreatedByScript{false};
    bool        isReplacingScriptKey{false};            // true if script set key is not being used
};

class CKeyFunctionBind : public CKeyBindWithState
{
public:
    CKeyFunctionBind() : CKeyBindWithState(KeyBindType::FUNCTION) {}

public:
    KeyFunctionBindHandler handler{nullptr};
    bool                   ignoresGUI{false};
};

class CControlFunctionBind : public CKeyBindWithState
{
public:
    CControlFunctionBind() : CKeyBindWithState(KeyBindType::CONTROL_FUNCTION) {}

public:
    SBindableGTAControl*       control{nullptr};
    ControlFunctionBindHandler handler{nullptr};
};

class CGTAControlBind : public CKeyBind
{
public:
    CGTAControlBind() : CKeyBind(KeyBindType::GTA_CONTROL) {}

public:
    SBindableGTAControl* control{nullptr};
    bool                 state{false};
    bool                 isEnabled{false};
};

class CKeyBindsInterface
{
public:
    using KeyBindPtr = std::unique_ptr<CKeyBind>;
    using KeyBindContainer = std::list<KeyBindPtr>;

    virtual ~CKeyBindsInterface() = default;

    virtual bool ProcessMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
    virtual void OnLoseFocus() = 0;

    virtual void Remove(CKeyBind* keyBind) = 0;

    virtual KeyBindContainer::iterator       begin() = 0;
    virtual KeyBindContainer::const_iterator begin() const = 0;
    virtual KeyBindContainer::iterator       end() = 0;
    virtual KeyBindContainer::const_iterator end() const = 0;

    // Command-bind funcs
    virtual bool AddCommand(const char* szKey, const char* szCommand, const char* szArguments, bool bState, const char* szResource = NULL,
                            bool bScriptCreated = false, const char* szOriginalScriptKey = NULL) = 0;
    virtual bool AddCommand(const SBindableKey* pKey, const char* szCommand, const char* szArguments = NULL, bool bState = true) = 0;
    virtual bool CommandExists(const char* szKey, const char* szCommand, bool bCheckState = false, bool bState = true, const char* szArguments = NULL,
                               const char* szResource = NULL, bool bCheckScriptCreated = false, bool bScriptCreated = false) = 0;
    virtual bool SetCommandActive(const char* szKey, const char* szCommand, bool bState, const char* szArguments, const char* szResource, bool bActive,
                                  bool checkHitState, bool bConsiderDefaultKey = false) = 0;
    virtual void SetAllCommandsActive(const char* szResource, bool bActive, const char* szCommand = NULL, bool bState = true, const char* szArguments = NULL,
                                      bool checkHitState = false, const char* szOnlyWithDefaultKey = NULL) = 0;
    virtual CCommandBind* GetBindFromCommand(const char* szCommand, const char* szArguments = NULL, bool bMatchCase = true, const char* szKey = NULL,
                                             bool bCheckHitState = false, bool bState = NULL) = 0;
    virtual bool          GetBoundCommands(const char* szCommand, std::list<CCommandBind*>& commandsList) = 0;
    virtual void          UserChangeCommandBoundKey(CCommandBind* pBind, const SBindableKey* pNewBoundKey) = 0;
    virtual void          UserRemoveCommandBoundKey(CCommandBind* pBind) = 0;
    virtual CCommandBind* FindMatchingUpBind(CCommandBind* pBind) = 0;

    // Control-bind funcs
    virtual bool AddGTAControl(const char* szKey, const char* szControl) = 0;
    virtual bool AddGTAControl(const SBindableKey* pKey, SBindableGTAControl* pControl) = 0;
    virtual bool RemoveGTAControl(const char* szKey, const char* szControl) = 0;
    virtual bool RemoveAllGTAControls(const char* szKey) = 0;
    virtual bool RemoveAllGTAControls() = 0;
    virtual bool GTAControlExists(const char* szKey, const char* szControl) = 0;
    virtual bool GTAControlExists(const SBindableKey* pKey, SBindableGTAControl* pControl) = 0;
    virtual void CallGTAControlBind(CGTAControlBind* pBind, bool bState) = 0;
    virtual void CallAllGTAControlBinds(eControlType controlType, bool bState) = 0;
    virtual bool GetBoundControls(SBindableGTAControl* pControl, std::list<CGTAControlBind*>& controlsList) = 0;

    virtual bool GetMultiGTAControlState(CGTAControlBind* pBind) = 0;
    virtual bool IsControlEnabled(const char* szControl) = 0;
    virtual bool SetControlEnabled(const char* szControl, bool bEnabled) = 0;
    virtual void SetAllControlsEnabled(bool bGameControls, bool bMTAControls, bool bEnabled) = 0;

    // Function-bind funcs
    virtual bool AddFunction(const char* szKey, KeyFunctionBindHandler Handler, bool bState = true, bool bIgnoreGUI = false) = 0;
    virtual bool AddFunction(const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bState = true, bool bIgnoreGUI = false) = 0;
    virtual bool RemoveFunction(const char* szKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true) = 0;
    virtual bool RemoveFunction(const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true) = 0;
    virtual bool RemoveAllFunctions(KeyFunctionBindHandler Handler) = 0;
    virtual bool RemoveAllFunctions() = 0;
    virtual bool FunctionExists(const char* szKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true) = 0;
    virtual bool FunctionExists(const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true) = 0;

    // Function-control-bind funcs
    virtual bool AddControlFunction(const char* szControl, ControlFunctionBindHandler Handler, bool bState = true) = 0;
    virtual bool AddControlFunction(SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bState = true) = 0;
    virtual bool RemoveControlFunction(const char* szControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true) = 0;
    virtual bool RemoveControlFunction(SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true) = 0;
    virtual bool RemoveAllControlFunctions(ControlFunctionBindHandler Handler) = 0;
    virtual bool RemoveAllControlFunctions() = 0;
    virtual bool ControlFunctionExists(const char* szControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true) = 0;
    virtual bool ControlFunctionExists(SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true) = 0;

    // Key/code funcs
    virtual const SBindableKey* GetBindableFromKey(const char* szKey) = 0;
    virtual const SBindableKey* GetBindableFromKey(const char* szKey) const = 0;
    virtual const SBindableKey* GetBindableFromGTARelative(int iGTAKey) = 0;
    virtual bool                IsKey(const char* szKey) = 0;
    virtual const SBindableKey* GetBindableFromMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bState) = 0;
    virtual void                SetKeyStrokeHandler(KeyStrokeHandler Handler) = 0;
    virtual void                SetCharacterKeyHandler(CharacterKeyHandler Handler) = 0;
    virtual bool                GetKeyStateByName(const char* keyName, bool& state) const = 0;

    // Control/action funcs
    virtual SBindableGTAControl* GetBindableFromControl(const char* szControl) = 0;
    virtual SBindableGTAControl* GetBindableFromAction(eControllerAction action) = 0;
    virtual bool                 IsControl(const char* szControl) = 0;

    virtual void SetAllFootControls(bool bState) = 0;
    virtual void SetAllVehicleControls(bool bState) = 0;

    virtual void         SetAllBindStates(bool bState, KeyBindType onlyType = KeyBindType::UNDEFINED) = 0;
    virtual unsigned int Count(KeyBindType bindType = KeyBindType::UNDEFINED) = 0;

    virtual void DoPreFramePulse() = 0;
    virtual void DoPostFramePulse() = 0;

    virtual bool LoadFromXML(class CXMLNode* pMainNode) = 0;
    virtual bool SaveToXML(class CXMLNode* pMainNode) = 0;
    virtual void LoadDefaultBinds() = 0;
    // virtual void                    LoadDefaultControls         ( void ) = 0;
    virtual void LoadDefaultCommands(bool bForce) = 0;
    virtual void LoadControlsFromGTA() = 0;

    virtual void BindCommand(const char* szCmdLine) = 0;
    virtual void UnbindCommand(const char* szCmdLine) = 0;
    virtual void PrintBindsCommand(const char* szCmdLine) = 0;
    virtual bool TriggerKeyStrokeHandler(const SString& strKey, bool bActive, bool bIsConsoleInputKey) = 0;

    // Events
    ksignals::Event<void(const SString&)> OnPaste;
};
