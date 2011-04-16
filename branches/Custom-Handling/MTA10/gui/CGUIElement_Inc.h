/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIElement_Impl.cpp
*  PURPOSE:     Element derived class inclusion header
*               (fixes C++-language issues regarding inheritance)
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

void SetProperty ( const char *szProperty, const char *szValue ) { CGUIElement_Impl::SetProperty ( szProperty, szValue ); };
std::string GetProperty ( const char *szProperty )          { return CGUIElement_Impl::GetProperty ( szProperty ); };

CGUIPropertyIter GetPropertiesBegin ( void )                { return CGUIElement_Impl::GetPropertiesBegin (); };
CGUIPropertyIter GetPropertiesEnd ( void )                  { return CGUIElement_Impl::GetPropertiesEnd (); };

bool SetFont ( const char *szFontName )                     { return CGUIElement_Impl::SetFont ( szFontName ); };
std::string GetFont ( void )                                { return CGUIElement_Impl::GetFont (); };

#ifndef SETVISIBLE_HACK
void SetVisible ( bool bVisible )                           { CGUIElement_Impl::SetVisible ( bVisible ); };
#endif
bool IsVisible ( void )                                     { return CGUIElement_Impl::IsVisible (); };

#ifndef SETENABLED_HACK
void SetEnabled ( bool bEnabled )                           { CGUIElement_Impl::SetEnabled ( bEnabled ); };
#endif
bool IsEnabled ( void )                                     { return CGUIElement_Impl::IsEnabled (); };
void SetZOrderingEnabled ( bool bZOrderingEnabled )         { CGUIElement_Impl::SetZOrderingEnabled ( bZOrderingEnabled ); };
bool IsZOrderingEnabled ( void )                            { return CGUIElement_Impl::IsZOrderingEnabled (); };

void BringToFront ( void )                                  { CGUIElement_Impl::BringToFront (); };
void MoveToBack ( void )                                    { CGUIElement_Impl::MoveToBack (); };

void SetPosition ( const CVector2D& vecPosition, bool bRelative = false )   { CGUIElement_Impl::SetPosition ( vecPosition, bRelative ); };
CVector2D GetPosition ( bool bRelative = false )                            { return CGUIElement_Impl::GetPosition ( bRelative ); };
void GetPosition ( CVector2D& vecPosition, bool bRelative = false )         { CGUIElement_Impl::GetPosition ( vecPosition, bRelative ); };

void SetWidth ( float fX, bool bRelative = false )                          { CGUIElement_Impl::SetWidth ( fX, bRelative ); };
void SetHeight ( float fY, bool bRelative = false )                         { CGUIElement_Impl::SetHeight ( fY, bRelative ); };

void SetSize ( const CVector2D& vecSize, bool bRelative = false )           { CGUIElement_Impl::SetSize ( vecSize, bRelative ); };
CVector2D GetSize ( bool bRelative = false )                                { return CGUIElement_Impl::GetSize ( bRelative ); };
void GetSize ( CVector2D& vecSize, bool bRelative = false )                 { CGUIElement_Impl::GetSize ( vecSize, bRelative ); };

void SetMinimumSize ( const CVector2D& vecSize )            { CGUIElement_Impl::SetMinimumSize ( vecSize ); };
CVector2D GetMinimumSize ( void )                           { return CGUIElement_Impl::GetMinimumSize (); };
void GetMinimumSize ( CVector2D& vecSize )                  { CGUIElement_Impl::GetMinimumSize ( vecSize ); };

void SetMaximumSize ( const CVector2D& vecSize )            { CGUIElement_Impl::SetMaximumSize ( vecSize ); };
CVector2D GetMaximumSize ( void )                           { return CGUIElement_Impl::GetMaximumSize (); };
void GetMaximumSize ( CVector2D& vecSize )                  { CGUIElement_Impl::GetMaximumSize ( vecSize ); };

#ifndef EXCLUDE_SET_TEXT
void SetText ( const char*Text )                            { CGUIElement_Impl::SetText ( Text ); };
#endif

std::string GetText ( void )                                { return CGUIElement_Impl::GetText (); };

void SetAlpha ( float fAlpha )                              { CGUIElement_Impl::SetAlpha ( fAlpha ); };
float GetAlpha ( void )                                     { return CGUIElement_Impl::GetAlpha (); };
void SetInheritsAlpha ( bool bInheritsAlpha )               { CGUIElement_Impl::SetInheritsAlpha ( bInheritsAlpha ); };
bool GetInheritsAlpha ( void )                              { return CGUIElement_Impl::GetInheritsAlpha (); };

void Activate ( void )                                      { CGUIElement_Impl::Activate (); };
void Deactivate ( void )                                    { CGUIElement_Impl::Deactivate (); };
bool IsActive ( void )                                      { return CGUIElement_Impl::IsActive (); };

void ForceRedraw ( void )                                   { CGUIElement_Impl::ForceRedraw (); };
void SetAlwaysOnTop ( bool bAlwaysOnTop )                   { CGUIElement_Impl::SetAlwaysOnTop ( bAlwaysOnTop ); };
bool IsAlwaysOnTop ( void )                                 { return CGUIElement_Impl::IsAlwaysOnTop (); };

CRect2D AbsoluteToRelative ( const CRect2D& Rect )        { return CGUIElement_Impl::AbsoluteToRelative ( Rect ); };
CVector2D AbsoluteToRelative ( const CVector2D& Vector )    { return CGUIElement_Impl::AbsoluteToRelative ( Vector ); };

CRect2D RelativeToAbsolute ( const CRect2D& Rect )        { return CGUIElement_Impl::RelativeToAbsolute ( Rect ); };
CVector2D RelativeToAbsolute ( const CVector2D& Vector )    { return CGUIElement_Impl::RelativeToAbsolute ( Vector ); };

void SetParent ( CGUIElement* pParent )                     { CGUIElement_Impl::SetParent ( pParent ); };
CGUIElement* GetParent ( void )                             { return CGUIElement_Impl::GetParent (); };

CEGUI::Window* GetWindow ( void )                           { return CGUIElement_Impl::GetWindow (); };

void SetUserData ( void* pData )                            { CGUIElement_Impl::SetUserData ( pData ); };
void* GetUserData ( void )                                  { return CGUIElement_Impl::GetUserData (); };

void SetClickHandler ( GUI_CALLBACK Callback )              { CGUIElement_Impl::SetClickHandler ( Callback ); };
void SetMovedHandler ( GUI_CALLBACK Callback )              { CGUIElement_Impl::SetMovedHandler ( Callback ); };
void SetSizedHandler ( GUI_CALLBACK Callback )              { CGUIElement_Impl::SetSizedHandler ( Callback ); };
void SetMouseEnterHandler ( GUI_CALLBACK Callback )         { CGUIElement_Impl::SetMouseEnterHandler ( Callback ); };
void SetMouseLeaveHandler ( GUI_CALLBACK Callback )         { CGUIElement_Impl::SetMouseLeaveHandler ( Callback ); };
void SetMouseButtonDownHandler ( GUI_CALLBACK Callback )    { CGUIElement_Impl::SetMouseButtonDownHandler ( Callback ); };
void SetActivateHandler ( GUI_CALLBACK Callback )           { CGUIElement_Impl::SetActivateHandler ( Callback ); };
void SetDeactivateHandler ( GUI_CALLBACK Callback )         { CGUIElement_Impl::SetDeactivateHandler ( Callback ); };