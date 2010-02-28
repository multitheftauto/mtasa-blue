/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/core/CKeyBindsInterface.h
*  PURPOSE:		Keybind manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CKEYBINDSINTERFACE_H
#define __CKEYBINDSINTERFACE_H

// Beware: this class is poorly written and depends on game while it shouldn't
#include <game/CControllerConfigManager.h>

#include <list>

class CKeyFunctionBind;
class CControlFunctionBind;

typedef void ( *KeyFunctionBindHandler ) ( CKeyFunctionBind* pBind );
typedef void ( *ControlFunctionBindHandler ) ( CControlFunctionBind* pBind );

enum eKeyData
{
    DATA_NONE = 0,
    DATA_EXTENDED,
    DATA_NOT_EXTENDED,
    DATA_NUMPAD,
};

struct SBindableKey
{
    char szKey [20];
    unsigned long ulCode;
    int iGTARelative;
    eKeyData data;
    unsigned char ucNumpadRelative;
};

enum eControlType
{
    CONTROL_FOOT,
    CONTROL_VEHICLE,
    CONTROL_BOTH
};

struct SBindableGTAControl
{
    char szControl [25];
    eControllerAction action;
    eControlType controlType;
    bool bState;
    bool bEnabled;
    char* szDescription;
};

enum eKeyBindType
{
    KEY_BIND_COMMAND, // bind key function args
    KEY_BIND_GTA_CONTROL, // bind key gta_control 
    KEY_BIND_FUNCTION, // script bind key to function 
    KEY_BIND_CONTROL_FUNCTION, // script bind gta_control to function (pressing control, calls function)
    KEY_BIND_UNDEFINED,
};

class CKeyBind
{
public:
    inline                  CKeyBind ( void ) : boundKey ( NULL ), beingDeleted ( false ) { bActive = true; }
    const SBindableKey*		boundKey;
    bool                    beingDeleted;
    bool                    bActive;
    inline bool             IsBeingDeleted ( void ) { return beingDeleted; }
    virtual eKeyBindType	GetType    ( void ) = 0;
};

class CKeyBindWithState: public CKeyBind
{
public:
    inline          CKeyBindWithState ( void ) { bState = false; }
    bool            bState;
    bool            bHitState;
};


class CCommandBind: public CKeyBindWithState
{
public:
    inline          CCommandBind    ( void ) { szCommand = NULL; szArguments = NULL; szResource = NULL; }
    inline          ~CCommandBind   ( void ) { delete [] szCommand; if ( szArguments ) delete [] szArguments; if ( szResource ) delete [] szResource; }
    eKeyBindType    GetType         ( void ) { return KEY_BIND_COMMAND; }
    char*           szCommand;
    char*           szArguments;
    char*           szResource;
    char*           szDefaultKey;
};

class CKeyFunctionBind: public CKeyBindWithState
{
public:
    eKeyBindType            GetType         ( void ) { return KEY_BIND_FUNCTION; }
    KeyFunctionBindHandler  Handler;
    bool                    bIgnoreGUI;
};

class CControlFunctionBind: public CKeyBindWithState
{
public:
    eKeyBindType               GetType         ( void ) { return KEY_BIND_CONTROL_FUNCTION; }
    SBindableGTAControl*       control;
    ControlFunctionBindHandler Handler;
};

class CGTAControlBind: public CKeyBind
{
public:
    eKeyBindType    GetType       ( void ) { return KEY_BIND_GTA_CONTROL; }
    SBindableGTAControl* control;
    bool            bState;
    bool            bEnabled;
};

class CKeyBindsInterface
{
public:
    virtual bool                    ProcessMessage              ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) = 0;

    // Basic funcs
    virtual void                    Add                         ( CKeyBind* pKeyBind ) = 0;
    virtual void                    Remove                      ( CKeyBind* pKeyBind ) = 0;
    virtual void                    Clear                       ( void ) = 0;
    virtual bool                    Call                        ( CKeyBind* pKeyBind ) = 0;

    virtual std::list < CKeyBind* > ::const_iterator IterBegin  ( void ) = 0;
    virtual std::list < CKeyBind* > ::const_iterator IterEnd    ( void ) = 0;

    // Command-bind funcs
    virtual bool                    AddCommand                  ( const char* szKey, const char* szCommand, const char* szArguments = NULL, bool bState = true, const char* szResource = NULL ) = 0;
    virtual bool                    AddCommand                  ( const SBindableKey* pKey, const char* szCommand, const char* szArguments = NULL, bool bState = true ) = 0;
    virtual bool                    RemoveCommand               ( const char* szKey, const char* szCommand, bool bCheckState = false, bool bState = true, const char* szResource = NULL ) = 0;
    virtual bool                    RemoveAllCommands           ( const char* szKey, bool bCheckState = false, bool bState = true ) = 0;
    virtual bool                    RemoveAllCommands           ( void ) = 0;
    virtual bool                    CommandExists               ( const char* szKey, const char* szCommand, bool bCheckState = false, bool bState = true, const char* szArguments = NULL ) = 0;
    virtual bool                    SetCommandActive            ( const char* szKey, const char* szCommand, bool bState, const char* szArguments, const char* szResource, bool bActive, bool checkHitState ) = 0;
    virtual void                    SetAllCommandsActive        ( const char* szResource, bool bActive ) = 0;
    virtual CCommandBind*           GetBindFromCommand          ( const char* szCommand, const char* szArguments = NULL, bool bMatchCase = true, const char* szKey = NULL, bool bCheckHitState = false, bool bState = NULL ) = 0;
    virtual bool                    GetBoundCommands            ( const char* szCommand, std::list < CCommandBind * > & commandsList ) = 0;

    // Control-bind funcs
    virtual bool                    AddGTAControl               ( const char* szKey, const char* szControl ) = 0;
    virtual bool                    AddGTAControl               ( const char* szKey, eControllerAction action ) = 0;
    virtual bool                    AddGTAControl               ( const SBindableKey* pKey, SBindableGTAControl* pControl ) = 0;
    virtual bool                    RemoveGTAControl            ( const char* szKey, const char* szControl ) = 0;
	virtual void					RemoveGTAControls		    ( const char* szControl, bool bDestroy = true ) = 0;
    virtual bool                    RemoveAllGTAControls        ( const char* szKey ) = 0;
    virtual bool                    RemoveAllGTAControls        ( void ) = 0;
    virtual bool                    GTAControlExists            ( const char* szKey, const char* szControl ) = 0;
    virtual bool                    GTAControlExists            ( const SBindableKey* pKey, SBindableGTAControl* pControl ) = 0;
    virtual unsigned int            GTAControlsCount            ( void ) = 0;
    virtual void                    CallGTAControlBind          ( CGTAControlBind* pBind, bool bState ) = 0;
    virtual void                    CallAllGTAControlBinds      ( eControlType controlType, bool bState ) = 0;
    virtual bool                    GetBoundControls            ( SBindableGTAControl * pControl, std::list < CGTAControlBind * > & controlsList ) = 0;

    virtual bool                    GetMultiGTAControlState     ( CGTAControlBind* pBind ) = 0;
    virtual bool                    IsControlEnabled            ( const char* szControl ) = 0;
    virtual bool                    SetControlEnabled           ( const char* szControl, bool bEnabled ) = 0;
    virtual void                    SetAllControlsEnabled       ( bool bGameControls, bool bMTAControls, bool bEnabled ) = 0;
    
    // Function-bind funcs
    virtual bool                    AddFunction                 ( const char* szKey, KeyFunctionBindHandler Handler, bool bState = true, bool bIgnoreGUI = false ) = 0;
    virtual bool                    AddFunction                 ( const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bState = true, bool bIgnoreGUI = false ) = 0;
    virtual bool                    RemoveFunction              ( const char* szKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true ) = 0;
    virtual bool                    RemoveFunction              ( const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true ) = 0;
    virtual bool                    RemoveAllFunctions          ( KeyFunctionBindHandler Handler ) = 0;
    virtual bool                    RemoveAllFunctions          ( void ) = 0;
    virtual bool                    FunctionExists              ( const char* szKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true ) = 0;
    virtual bool                    FunctionExists              ( const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true ) = 0;

    // Function-control-bind funcs
    virtual bool                    AddControlFunction          ( const char* szControl, ControlFunctionBindHandler Handler, bool bState = true ) = 0;
    virtual bool                    AddControlFunction          ( SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bState = true ) = 0;
    virtual bool                    RemoveControlFunction       ( const char* szControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true ) = 0;
    virtual bool                    RemoveControlFunction       ( SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true ) = 0;
    virtual bool                    RemoveAllControlFunctions   ( ControlFunctionBindHandler Handler ) = 0;
    virtual bool                    RemoveAllControlFunctions   ( void ) = 0;
    virtual bool                    ControlFunctionExists       ( const char* szControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true ) = 0;
    virtual bool                    ControlFunctionExists       ( SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true ) = 0;

    // Key/code funcs
    virtual char*                   GetKeyFromCode              ( unsigned long ulCode ) = 0;
    virtual bool                    GetCodeFromKey              ( const char* szKey, unsigned long& ucCode ) = 0;
    virtual const SBindableKey*     GetBindableFromKey          ( const char* szKey ) = 0;
    virtual const SBindableKey*     GetBindableFromGTARelative  ( int iGTAKey ) = 0;
    virtual bool                    IsKey                       ( const char* szKey ) = 0;
    virtual char*                   GetKeyFromGTARelative       ( int iGTAKey ) = 0;
    virtual const SBindableKey*     GetBindableFromMessage      ( UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bState ) = 0;

    // Control/action funcs
    virtual char*                   GetControlFromAction        ( eControllerAction action ) = 0;
    virtual bool                    GetActionFromControl        ( const char* szControl, eControllerAction& action ) = 0;
    virtual SBindableGTAControl*    GetBindableFromControl      ( const char* szControl ) = 0;
    virtual SBindableGTAControl*    GetBindableFromAction       ( eControllerAction action ) = 0;
    virtual bool                    IsControl                   ( const char* szControl ) = 0;

    virtual void                    SetAllFootControls          ( bool bState ) = 0;
    virtual void                    SetAllVehicleControls       ( bool bState ) = 0;

    virtual void                    SetAllBindStates            ( bool bState, eKeyBindType onlyType = KEY_BIND_UNDEFINED ) = 0;
    virtual unsigned int            Count                       ( eKeyBindType bindType ) = 0;

    virtual void                    DoPreFramePulse             ( void ) = 0;
    virtual void                    DoPostFramePulse            ( void ) = 0;

    virtual bool                    LoadFromXML                 ( class CXMLNode* pMainNode ) = 0;
    virtual bool                    SaveToXML                   ( class CXMLNode* pMainNode ) = 0;
    virtual void                    LoadDefaultBinds            ( void ) = 0;
    virtual void                    LoadDefaultControls         ( void ) = 0;
    virtual void                    LoadDefaultCommands         ( bool bForce ) = 0;
    virtual void                    LoadControlsFromGTA         ( void ) = 0;

    virtual void                    BindCommand                 ( const char* szCmdLine ) = 0;
    virtual void                    UnbindCommand               ( const char* szCmdLine ) = 0;
    virtual void                    PrintBindsCommand           ( const char* szCmdLine ) = 0;
};

#endif
