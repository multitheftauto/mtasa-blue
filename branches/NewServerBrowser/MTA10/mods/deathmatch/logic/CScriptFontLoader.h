/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CScriptFontLoader.h
*  PURPOSE:     Font loader for scripts
*  DEVELOPERS:  Talidan <>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


#ifndef __CScriptFontLoader_H
#define __CScriptFontLoader_H

struct sScriptFont
{
    ID3DXFont       *fntNormal;
    ID3DXFont       *fntBig;
    CGUIFont*       fntCEGUI;
    std::string     strInternalName;
    std::string     strPath;
    CResource*      pResource;
};

class CScriptFontLoader
{
public:
                                CScriptFontLoader         ( void );
                                ~CScriptFontLoader        ( void );

    // Basic funcs
    bool                        LoadFont                  ( std::string strFullFilePath, bool bBold, unsigned int uiSize, std::string strMetaPath, CResource* pResource );
    bool                        UnloadFont                ( std::string strFullFilePath, std::string strMetaPath, CResource* pResource );

    void                        UnloadFonts               ( CResource* pResource );

    bool                        GetDXFont                 ( ID3DXFont** pFont, std::string strFullFilePath, std::string strMetaPath, CResource* pResource, float fScaleX, float fScaleY );


    std::list < sScriptFont >   m_List;
    CGUI*                       m_pGUI;

};

#endif
