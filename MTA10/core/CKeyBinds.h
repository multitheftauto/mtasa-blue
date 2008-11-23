/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CKeyBinds.h
*  PURPOSE:     Header file for core keybind manager class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CKeyBinds;

#ifndef __CKEYBINDS_H
#define __CKEYBINDS_H

#include <windows.h>
#include <string.h>
#include <list>
using namespace std;

#include <core/CCoreInterface.h>
#include <core/CCommandsInterface.h>

class CKeyBinds: public CKeyBindsInterface
{
public:
                            CKeyBinds                   ( class CCore* pCore );
                            ~CKeyBinds                  ( void );

    bool                    ProcessMessage              ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
    bool                    ProcessKeyStroke            ( const SBindableKey * pKey, bool bState );
public:

    // Basic funcs
    void                    Add                         ( CKeyBind* pKeyBind );
    void                    Remove                      ( CKeyBind* pKeyBind );
    void                    Clear                       ( void );
    void                    RemoveDeletedBinds          ( void );
    void                    ClearCommandsAndControls    ( void );
    bool                    Call                        ( CKeyBind* pKeyBind );
    
    list < CKeyBind* > ::const_iterator IterBegin       ( void )    { return m_pList->begin (); }
    list < CKeyBind* > ::const_iterator IterEnd         ( void )    { return m_pList->end (); }


    // Command-bind funcs
    bool                    AddCommand                  ( const char* szKey, const char* szCommand, const char* szArguments = NULL, bool bState = true );
    bool                    AddCommand                  ( const SBindableKey* pKey, const char* szCommand, const char* szArguments = NULL, bool bState = true );
    bool                    RemoveCommand               ( const char* szKey, const char* szCommand, bool bCheckState = false, bool bState = true );
    bool                    RemoveAllCommands           ( const char* szKey, bool bCheckState = false, bool bState = true );
    bool                    RemoveAllCommands           ( void );
    bool                    CommandExists               ( const char* szKey, const char* szCommand, bool bCheckState = false, bool bState = true );
    CCommandBind*           GetBindFromCommand          ( const char* szCommand, const char* szArguments = NULL, bool bMatchCase = true );
    bool                    GetBoundCommands            ( const char* szCommand, list < CCommandBind * > & commandsList );
    
    // Control-bind funcs
    bool                    AddGTAControl               ( const char* szKey, const char* szControl );
    bool                    AddGTAControl               ( const char* szKey, eControllerAction action );
    bool                    AddGTAControl               ( const SBindableKey* pKey, SBindableGTAControl* pControl );
    bool                    RemoveGTAControl            ( const char* szKey, const char* szControl );
	void					RemoveGTAControls		    ( const char* szControl, bool bDestroy = true );
    bool                    RemoveAllGTAControls        ( const char* szKey );
    bool                    RemoveAllGTAControls        ( void );
    bool                    GTAControlExists            ( const char* szKey, const char* szControl );
    bool                    GTAControlExists            ( const SBindableKey* pKey, SBindableGTAControl* pControl );
    unsigned int            GTAControlsCount            ( void );
    void                    CallGTAControlBind          ( CGTAControlBind* pBind, bool bState );
    void                    CallAllGTAControlBinds      ( eControlType controlType, bool bState );
    bool                    GetBoundControls            ( SBindableGTAControl * pControl, list < CGTAControlBind * > & controlsList );

    // Control-bind extra funcs
    bool                    GetMultiGTAControlState     ( CGTAControlBind* pBind );
    bool                    IsControlEnabled            ( const char* szControl );
    bool                    SetControlEnabled           ( const char* szControl, bool bEnabled );
    void                    SetAllControlsEnabled       ( bool bGameControls, bool bMTAControls, bool bEnabled );
    void                    ResetGTAControlState        ( SBindableGTAControl * pControl );
    void                    ResetAllGTAControlStates    ( void );    

    // Function-bind funcs
    bool                    AddFunction                 ( const char* szKey, KeyFunctionBindHandler Handler, bool bState = true, bool bIgnoreGUI = false );
    bool                    AddFunction                 ( const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bState = true, bool bIgnoreGUI = false );
    bool                    RemoveFunction              ( const char* szKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true );
    bool                    RemoveFunction              ( const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true );
    bool                    RemoveAllFunctions          ( KeyFunctionBindHandler Handler );
    bool                    RemoveAllFunctions          ( void );
    bool                    FunctionExists              ( const char* szKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true );
    bool                    FunctionExists              ( const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true );

    // Function-control-bind funcs
    bool                    AddControlFunction          ( const char* szControl, ControlFunctionBindHandler Handler, bool bState = true );
    bool                    AddControlFunction          ( SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bState = true );
    bool                    RemoveControlFunction       ( const char* szControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true );
    bool                    RemoveControlFunction       ( SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true );
    bool                    RemoveAllControlFunctions   ( ControlFunctionBindHandler Handler );
    bool                    RemoveAllControlFunctions   ( void );
    bool                    ControlFunctionExists       ( const char* szControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true );
    bool                    ControlFunctionExists       ( SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true );

    // Key/code funcs
    char*					GetKeyFromCode              ( unsigned long ulCode );
    bool					GetCodeFromKey              ( const char* szKey, unsigned long& ucCode );
    const SBindableKey*     GetBindableFromKey          ( const char* szKey );
    const SBindableKey*     GetBindableFromGTARelative  ( int iGTAKey );
    bool                    IsKey                       ( const char* szKey );
    char*                   GetKeyFromGTARelative       ( int iGTAKey );
    const SBindableKey*     GetBindableFromMessage      ( UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bState );

    // Control/action funcs
    char*                   GetControlFromAction        ( eControllerAction action );
    bool                    GetActionFromControl        ( const char* szControl, eControllerAction& action );
    SBindableGTAControl*    GetBindableFromControl      ( const char* szControl );
    SBindableGTAControl*    GetBindableFromAction       ( eControllerAction action );
    bool                    IsControl                   ( const char* szControl );
    void                    SetAllControls              ( bool bState );
    void                    SetAllFootControls          ( bool bState );
    void                    SetAllVehicleControls       ( bool bState );

    void                    SetAllBindStates            ( bool bState, eKeyBindType onlyType = KEY_BIND_UNDEFINED );

    unsigned int            Count                       ( eKeyBindType bindType = KEY_BIND_UNDEFINED );

    void                    DoPreFramePulse             ( void );
    void                    DoPostFramePulse            ( void );

    bool                    LoadFromXML                 ( CXMLNode* pMainNode );
    bool                    SaveToXML                   ( CXMLNode* pMainNode );
    void                    LoadDefaultBinds            ( void );
    void                    LoadDefaultControls         ( void );
    void                    LoadDefaultCommands         ( void );
    void                    LoadControlsFromGTA         ( void );

    void                    BindCommand                 ( const char* szCmdLine );
    void                    UnbindCommand               ( const char* szCmdLine );
    void                    PrintBindsCommand           ( const char* szCmdLine );

    static bool             IsFakeCtrl_L                ( UINT message, WPARAM wParam, LPARAM lParam );

    void                    AddSection                  ( char* szSection );
    void                    RemoveSection               ( char* szSection );
    void                    RemoveAllSections           ( void );

private:    
    CCore*                  m_pCore;

    list < CKeyBind* >*     m_pList;
    char*                   m_szFileName;
    bool                    m_bMouseWheel;
    bool                    m_bInVehicle;
    CCommandBind*           m_pChatBoxBind;
    bool                    m_bProcessingKeyStroke;
};

#endif
