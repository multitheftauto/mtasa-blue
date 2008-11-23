/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CMainConfig.h
*  PURPOSE:     Header file for configuration class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CMAINCONFIG_H
#define __CMAINCONFIG_H

#include "CXMLConfig.h"
#include <core/CMainConfigInterface.h>

#define CONFIG_FAVOURITE_LIST_TAG   "favourite_server"
#define CONFIG_RECENT_LIST_TAG      "recently_played_server"

void _SetString ( char** szString, char* szValue );

class CMainConfig: public CMainConfigInterface, public CXMLConfig
{
public:
                                    CMainConfig                 ( char* szFileName );
                                    ~CMainConfig                ( void );

    bool                            Load                        ( char* szFileName = NULL );
    bool                            Save                        ( char* szFileName = NULL );

	inline bool						IsLoaded					( void ) { return m_bLoaded; };

    bool                            LoadServerList              ( CXMLNode* pNode, std::string strTagName, CServerList *pList );
    bool                            SaveServerList              ( CXMLNode* pNode, std::string strTagName, CServerList *pList );

    bool                            LoadSettings                ( CXMLNode* pNode );
    bool                            SaveSettings                ( CXMLNode* pNode );

    void                            GetCommunityUsername        ( std::string & strUsername );
    void                            GetCommunityPassword        ( std::string & strPassword );

    inline void                     GetChatColor                ( unsigned char& R, unsigned char& G, unsigned char& B, unsigned char& A )  { R = ucChatColorR; G = ucChatColorG; B = ucChatColorB; A = ucChatColorA; }
    inline void                     SetChatColor                ( unsigned char R, unsigned char G, unsigned char B, unsigned char A )      { ucChatColorR = R; ucChatColorG = G; ucChatColorB = B; ucChatColorA = A; }
    inline void                     GetChatInputColor           ( unsigned char& R, unsigned char& G, unsigned char& B, unsigned char& A )  { R = ucChatInputColorR; G = ucChatInputColorG; B = ucChatInputColorB; A = ucChatInputColorA; }
    inline void                     SetChatInputColor           ( unsigned char R, unsigned char G, unsigned char B, unsigned char A )      { ucChatInputColorR = R; ucChatInputColorG = G; ucChatInputColorB = B; ucChatInputColorA = A; }
    inline void                     GetChatInputPrefixColor     ( unsigned char& R, unsigned char& G, unsigned char& B, unsigned char& A )  { R = ucChatInputPrefixColorR; G = ucChatInputPrefixColorG; B = ucChatInputPrefixColorB; A = ucChatInputPrefixColorA; }
    inline void                     SetChatInputPrefixColor     ( unsigned char R, unsigned char G, unsigned char B, unsigned char A )      { ucChatInputPrefixColorR = R; ucChatInputPrefixColorG = G; ucChatInputPrefixColorB = B; ucChatInputPrefixColorA = A; }
    inline void                     GetChatInputTextColor       ( unsigned char& R, unsigned char& G, unsigned char& B, unsigned char& A )  { R = ucChatInputTextColorR; G = ucChatInputTextColorG; B = ucChatInputTextColorB; A = ucChatInputTextColorA; }
    inline void                     SetChatInputTextColor       ( unsigned char R, unsigned char G, unsigned char B, unsigned char A )      { ucChatInputTextColorR = R; ucChatInputTextColorG = G; ucChatInputTextColorB = B; ucChatInputTextColorA = A; }
  
    // Interface
    inline float                    GetTextScale                ( void )                                                                    { return fTextScale; }
    inline void                     SetTextScale                ( float fScale )                                                            { fTextScale = fScale; }

	unsigned short					GetFPSLimit					( void )																	{ return usFPSLimit; }
	inline void						SetFPSLimit					( unsigned short usLimit )													{ usFPSLimit = usLimit; }
    
    // Connection
    std::string                     strNick;

    // last successful connection
    std::string                     strHost;
    unsigned short                  usPort;
    std::string                     strPassword;

    // quick connect info
    std::string                     qc_strHost;
    unsigned short                  qc_usPort;
    std::string                     qc_strPassword;

    // Misc
    std::string                     strDebugFileName;
	unsigned short					usFPSLimit;
    unsigned short					usMTUSize;

    // Console
    CVector2D                       vecConsolePosition;
    CVector2D                       vecConsoleSize;

    // Chat
    unsigned char                   ucChatFont;
    unsigned char                   ucChatLines;
    unsigned char                   ucChatColorR, ucChatColorG, ucChatColorB, ucChatColorA;
    unsigned char                   ucChatInputColorR, ucChatInputColorG, ucChatInputColorB, ucChatInputColorA;
    unsigned char                   ucChatInputPrefixColorR, ucChatInputPrefixColorG, ucChatInputPrefixColorB, ucChatInputPrefixColorA;
    unsigned char                   ucChatInputTextColorR, ucChatInputTextColorG, ucChatInputTextColorB, ucChatInputTextColorA;
    CVector2D                       vecChatScale;
    float                           fChatWidth;
    bool                            bChatCssStyleText;
    bool                            bChatCssStyleBackground;
    unsigned long                   ulChatLineLife;
    unsigned long                   ulChatLineFadeOut;
    bool                            bChatUseCEGUI;	

    // Server browser
//    CSavedServerManager             FavouriteServers;
//    CSavedServerManager             RecentlyPlayedServers;

    // Menu
	int								iMenuOptions;

    // Display
    float                           fTextScale;

    // Controls
    bool                            bInvertMouse;
    bool                            bSteerWithMouse;
    bool                            bFlyWithMouse;

	// Community
	std::string						strCommunityUsername;
	std::string						strCommunityPassword;

private:
	bool							m_bLoaded;		// Indicate that the Load function has been called, so it's safe to read out the configuration
};

#endif
