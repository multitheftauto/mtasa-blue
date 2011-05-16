/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CScriptFontLoader.cpp
*  PURPOSE:     Font loader for scripts
*  DEVELOPERS:  Talidan <>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "utils/XFont.h"

CScriptFontLoader::CScriptFontLoader ( void )
{
    m_pGUI = g_pCore->GetGUI ();
}


CScriptFontLoader::~CScriptFontLoader ( void )
{
}


bool CScriptFontLoader::LoadFont ( std::string strFullFilePath, bool bBold, unsigned int uiSize, std::string strMetaPath, CResource* pResource )
{
    uiSize = ( uiSize < 5 ) ? 5 : (( uiSize > 150 ) ? 150 : uiSize );
    std::string strFontName = SString("%s/%s", pResource->GetName(), strMetaPath.c_str());
    ReplaceOccurrencesInString ( strFontName, "\\", "/" );
    if ( !FileExists(strFullFilePath.c_str()) )
        return false;

    if ( m_pGUI->IsFontPresent (strFontName.c_str()) )
        return false;

    // Let's grab the font name
    FONT_PROPERTIES* sFontProps = new FONT_PROPERTIES;
    if (!GetFontProperties(LPCTSTR(strFullFilePath.c_str()), sFontProps))
        return false;

    std::string strFaceName = sFontProps->csName;

    delete sFontProps;
    ID3DXFont *pFntNormal = NULL,*pFntBig = NULL;
    if ( !g_pCore->GetGraphics()->LoadFont ( strFullFilePath, strFaceName, uiSize, bBold, &pFntNormal, &pFntBig ))
        return false;

    // Load our font to GUI
    CGUIFont* fntCEGUI = m_pGUI->CreateFnt ( SString("%s/%s", pResource->GetName(), strMetaPath.c_str()), strFullFilePath.c_str(), uiSize/2 );

    //Compile our font data
    sScriptFont sNewFont;
    sNewFont.fntNormal = pFntNormal;
    sNewFont.fntBig = pFntBig;
    sNewFont.fntCEGUI = fntCEGUI;
    sNewFont.strInternalName = strFontName;
    sNewFont.strPath = strFullFilePath;
    sNewFont.pResource = pResource;

    m_List.push_back(sNewFont);

    return true;
}


bool CScriptFontLoader::UnloadFont ( std::string strFullFilePath, std::string strMetaPath, CResource* pResource )
{
    std::string strFontName = SString("%s/%s", pResource->GetName(), strMetaPath.c_str()).c_str();
    ReplaceOccurrencesInString ( strFontName, "\\", "/" );

    std::list < sScriptFont > ::iterator it = m_List.begin ();
    for ( ; it !=m_List.end(); it++ )
    {
        if ( strFontName == (*it).strInternalName )
        {
            // Destroy the font from DX
            g_pCore->GetGraphics()->DestroyFont ( strFullFilePath );
            // Destroy the font from CEGUI
            delete (*it).fntCEGUI;
            // Delete our struct
            m_List.erase(it);
            return true;
        }
    }
    return false;
}

void CScriptFontLoader::UnloadFonts ( CResource* pResource )
{
    std::list < sScriptFont > ::iterator it = m_List.begin ();
    while (it != m_List.end())
    {
        if ( pResource == (*it).pResource )
        {
            // Destroy the font from DX
            g_pCore->GetGraphics()->DestroyFont ( (*it).strPath );
            // Destroy the font from CEGUI
            delete (*it).fntCEGUI;
            // Delete our struct
            it = m_List.erase(it);
        }
        else
            it++;
    }
}


bool CScriptFontLoader::GetDXFont ( ID3DXFont** pFont, std::string strFullFilePath, std::string strMetaPath, CResource* pResource, float fScaleX, float fScaleY )
{
    std::string strFontName = SString("%s/%s", pResource->GetName(), strMetaPath.c_str()).c_str();
    ReplaceOccurrencesInString ( strFontName, "\\", "/" );

    if ( !m_List.empty() )
    {
        std::list < sScriptFont > ::iterator it = m_List.begin ();
        for ( ; it != m_List.end(); it++ )
        {
            if ( strFontName == (*it).strInternalName )
            {
                *pFont = ( fScaleX > 1.1f || fScaleY > 1.1f ) ? (*it).fntBig : (*it).fntNormal;
                return true;
            }
        }
    }       

    return false;
}
