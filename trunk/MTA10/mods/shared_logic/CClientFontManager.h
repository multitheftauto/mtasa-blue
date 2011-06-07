/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientFontManager.h
*  PURPOSE:     Custom font bucket bucket
*  DEVELOPERS:  qwerty
*
*****************************************************************************/

class CClientFontManager;

#include "CClientFont.h"


struct SGUIFontInfo
{
    CGUIFont*   pFntCEGUI;
    SString     strCEGUIFontName;
    std::set < CClientFont* > fontElementUsers;
};


struct SDXFontInfo
{
    SString     strFullFilePath;
    ID3DXFont*  pFntNormal;
    ID3DXFont*  pFntBig;
    std::set < CClientFont* > fontElementUsers;
};


class CClientFontManager
{
public:
                            CClientFontManager          ( CClientManager* pClientManager );
                            ~CClientFontManager         ( void );

    CClientFont*            CreateFont                  ( const SString& strFullFilePath, uint uiSize, bool bBold, const SString& strMetaPath, class CResource* pResource );
    void                    Remove                      ( CClientFont* pFont );

    uint                    GetGUIFontCount             ( void )                { return m_GUIFontInfoMap.size (); }
    uint                    GetDXFontCount              ( void )                { return m_DXFontInfoMap.size (); }

protected:
    SGUIFontInfo*           GetGUIFontInfo              ( const SString& strFontName, const SString& strFullFilePath, uint uiSize );
    SDXFontInfo*            GetDXFontInfo               ( const SString& strFontName, const SString& strFullFilePath, uint uiSize, bool bBold );

    CClientManager*                     m_pClientManager;
    std::map < SString, SGUIFontInfo >  m_GUIFontInfoMap;
    std::map < SString, SDXFontInfo >   m_DXFontInfoMap;
};
