/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/gui/CGUI.h
*  PURPOSE:     Graphical User Interface module interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CGUI;

#ifndef __CGUI_H
#define __CGUI_H

#include "CGUIElement.h"

#include "CGUIButton.h"
#include "CGUICallback.h"
#include "CGUICheckBox.h"
#include "CGUIEdit.h"
#include "CGUIEvent.h"
#include "CGUIFont.h"
#include "CGUIGridList.h"
#include "CGUILabel.h"
#include "CGUIMemo.h"
#include "CGUIMessageBox.h"
#include "CGUIProgressBar.h"
#include "CGUIRadioButton.h"
#include "CGUIStaticImage.h"
#include "CGUIScrollBar.h"
#include "CGUIScrollPane.h"
#include "CGUITexture.h"
#include "CGUIWindow.h"
#include "CGUITabPanel.h"
#include "CGUIComboBox.h"
#include "CGUITypes.h"

// Path defines for CGUI
#define	CGUI_ICON_MESSAGEBOX_INFO		"cgui\\images\\info.png"
#define	CGUI_ICON_MESSAGEBOX_QUESTION	"cgui\\images\\question.png"
#define	CGUI_ICON_MESSAGEBOX_WARNING	"cgui\\images\\warning.png"
#define	CGUI_ICON_MESSAGEBOX_ERROR		"cgui\\images\\error.png"
#define CGUI_ICON_SERVER_PASSWORD		"cgui\\images\\locked.png"
#define CGUI_IMAGE_BACKGROUND			"cgui\\images\\background.png"

// Message box types
enum CMessageBoxFlag
{
    MB_BUTTON_NONE = 0,
    MB_BUTTON_OK = 1,
    MB_BUTTON_CANCEL = 2,
    MB_BUTTON_YES = 4,
    MB_BUTTON_NO = 8,
    MB_ICON_INFO = 16,
    MB_ICON_QUESTION = 32,
    MB_ICON_WARNING = 64,
    MB_ICON_ERROR = 128,
};

class CGUI
{
public:

    virtual void                Draw                    ( void ) = 0;
    virtual void                Invalidate              ( void ) = 0;
    virtual void                Restore                 ( void ) = 0;

    virtual void                DrawMouseCursor         ( void ) = 0;

    virtual void                ProcessMouseInput       ( CGUIMouseInput eMouseInput, unsigned long ulX = 0, unsigned long ulY = 0, CGUIMouseButton eMouseButton = NoButton ) = 0;
    virtual void                ProcessKeyboardInput    ( unsigned long ulKey, bool bIsDown ) = 0;
    virtual void                ProcessCharacter        ( unsigned long ulCharacter ) = 0;

	//
	virtual void				SetGUIInputEnabled		( bool bEnabled ) = 0;
	virtual bool				GetGUIInputEnabled		( void ) = 0;

	//
	virtual CGUIMessageBox*		CreateMessageBox		( const char* szTitle, const char* szMessage, unsigned int uiFlags ) = 0;
    virtual CGUIButton*         CreateButton            ( CGUIElement* pParent = NULL, const char* szCaption = "" ) = 0;
    virtual CGUIButton*         CreateButton            ( CGUITab* pParent = NULL, const char* szCaption = "" ) = 0;

    virtual CGUICheckBox*       CreateCheckBox          ( CGUIElement* pParent = NULL, const char* szCaption = "", bool bChecked = false ) = 0;
    virtual CGUICheckBox*       CreateCheckBox          ( CGUITab* pParent = NULL, const char* szCaption = "", bool bChecked = false ) = 0;

    virtual CGUIRadioButton*    CreateRadioButton       ( CGUIElement* pParent = NULL, const char* szCaption = "" ) = 0;
    virtual CGUIRadioButton*    CreateRadioButton       ( CGUITab* pParent = NULL, const char* szCaption = "" ) = 0;

	virtual CGUIEdit*           CreateEdit              ( CGUIElement* pParent = NULL, const char* szText = "" ) = 0;
	virtual CGUIEdit*           CreateEdit              ( CGUITab* pParent = NULL, const char* szText = "" ) = 0;

    virtual CGUIGridList*       CreateGridList          ( CGUIElement* pParent = NULL, bool bFrame = true ) = 0;
    virtual CGUIGridList*       CreateGridList          ( CGUITab* pParent = NULL, bool bFrame = true ) = 0;

    virtual CGUILabel*          CreateLabel             ( CGUIElement* pParent, const char* szCaption = "" ) = 0;
    virtual CGUILabel*          CreateLabel             ( CGUITab* pParent, const char* szCaption = "" ) = 0;
    virtual CGUILabel*          CreateLabel             ( const char* szCaption = "" ) = 0;

    virtual CGUIProgressBar*    CreateProgressBar       ( CGUIElement* pParent ) = 0;
    virtual CGUIProgressBar*    CreateProgressBar       ( CGUITab* pParent ) = 0;

	virtual CGUIMemo*           CreateMemo              ( CGUIElement* pParent = NULL, const char* szText = "" ) = 0;
    virtual CGUIMemo*           CreateMemo              ( CGUITab* pParent = NULL, const char* szText = "" ) = 0;

	virtual CGUIStaticImage*    CreateStaticImage       ( CGUIElement* pParent ) = 0;
    virtual CGUIStaticImage*    CreateStaticImage       ( CGUITab* pParent ) = 0;
    virtual CGUIStaticImage*    CreateStaticImage       ( CGUIGridList* pParent ) = 0;
	virtual CGUIStaticImage*    CreateStaticImage       ( void ) = 0;

	virtual CGUITabPanel*       CreateTabPanel          ( CGUIElement* pParent = NULL ) = 0;
	virtual CGUITabPanel*       CreateTabPanel          ( CGUITab* pParent = NULL ) = 0;

	virtual CGUIScrollPane*     CreateScrollPane        ( CGUIElement* pParent = NULL ) = 0;
	virtual CGUIScrollPane*     CreateScrollPane        ( CGUITab* pParent = NULL ) = 0;

	virtual CGUIScrollBar*      CreateScrollBar         ( bool bHorizontal, CGUIElement* pParent = NULL ) = 0;
	virtual CGUIScrollBar*      CreateScrollBar         ( bool bHorizontal, CGUITab* pParent = NULL ) = 0;

    virtual CGUIComboBox*       CreateComboBox          ( CGUIElement* pParent = NULL, const char* szCaption = "" ) = 0;
    virtual CGUIComboBox*       CreateComboBox          ( CGUIComboBox* pParent = NULL, const char* szCaption = "" ) = 0;
    //

	virtual CGUIWindow*         CreateWnd               ( CGUIElement* pParent = NULL, const char* szCaption = "" ) = 0;
	virtual CGUIFont*           CreateFnt               ( const char* szFontName, const char* szFontFile, unsigned int uSize = 8, unsigned int uFlags = 0, unsigned int uExtraGlyphs[] = 0, bool bAutoScale = false ) = 0;
    virtual CGUITexture*        CreateTexture           ( void ) = 0;

	virtual void                SetCursorEnabled        ( bool bEnabled ) = 0;
    virtual bool                IsCursorEnabled         ( void ) = 0;

	virtual CVector2D			GetResolution			( void ) = 0;
	virtual void				SetResolution			( float fWidth, float fHeight ) = 0;

	virtual CGUIFont*			GetDefaultFont			( void ) = 0;
	virtual CGUIFont*			GetSmallFont			( void ) = 0;
	virtual CGUIFont*			GetBoldFont				( void ) = 0;
	virtual CGUIFont*			GetClearFont			( void ) = 0;
	virtual CGUIFont*			GetSansFont				( void ) = 0;

	virtual void				SetWorkingDirectory		( const char * szDir ) = 0;

	virtual void				SetCharacterKeyHandler		( void ) = 0;
	virtual void				SetKeyDownHandler			( void ) = 0;
	virtual void				SetMouseClickHandler		( void ) = 0;
	virtual void				SetMouseDoubleClickHandler	( void ) = 0;
	virtual void				SetMouseMoveHandler			( void ) = 0;
	virtual void				SetMouseEnterHandler		( void ) = 0;
	virtual void				SetMouseLeaveHandler		( void ) = 0;
	virtual void				SetMouseWheelHandler		( void ) = 0;
	virtual void				SetMovedHandler				( void ) = 0;
	virtual void				SetSizedHandler				( void ) = 0;

	virtual void				SetCharacterKeyHandler		( const GUI_CALLBACK_KEY & Callback ) = 0;
	virtual void				SetKeyDownHandler			( const GUI_CALLBACK_KEY & Callback ) = 0;
	virtual void				SetMouseClickHandler		( const GUI_CALLBACK_MOUSE & Callback ) = 0;
	virtual void				SetMouseDoubleClickHandler	( const GUI_CALLBACK_MOUSE & Callback ) = 0;
	virtual void				SetMouseMoveHandler			( const GUI_CALLBACK_MOUSE & Callback ) = 0;
	virtual void				SetMouseEnterHandler		( const GUI_CALLBACK_MOUSE & Callback ) = 0;
	virtual void				SetMouseLeaveHandler		( const GUI_CALLBACK_MOUSE & Callback ) = 0;
	virtual void				SetMouseWheelHandler		( const GUI_CALLBACK_MOUSE & Callback ) = 0;
	virtual void				SetMovedHandler				( const GUI_CALLBACK & Callback ) = 0;
	virtual void				SetSizedHandler				( const GUI_CALLBACK & Callback ) = 0;

    virtual bool                IsTransferBoxVisible        ( void ) = 0;
    virtual void                SetTransferBoxVisible       ( bool bVisible ) = 0;
};

#endif
