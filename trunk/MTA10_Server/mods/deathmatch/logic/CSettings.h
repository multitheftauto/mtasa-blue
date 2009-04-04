/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CSettings.h
*  PURPOSE:     XML-based variable settings class
*  DEVELOPERS:  Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CSettings;

#ifndef __CSETTINGS_H
#define __CSETTINGS_H

#include "CClient.h"
#include "CXMLConfig.h"
#include "CLogger.h"

class CResourceManager;

class CSettings
{
public:
	enum AccessType {
		Private,
		Protected,
		Public
	};
									CSettings				( CResourceManager* pResourceManager );
									~CSettings				( void );

	CXMLNode*						Get						( const char *szLocalResource, const char *szSetting, bool& bDeleteNode );
	bool							Set						( const char *szLocalResource, const char *szSetting, const char *szContent );

private:
	enum SettingStatus {
		NoAccess,
		NotFound,
		Found
	};

	CXMLNode*						Get						( CXMLNode *pSource, CXMLNode *pStorage, const char *szSourceResource, const char *szLocalResource, const char *szSetting, bool& bDeleteNode, SettingStatus& eSetting, CXMLNode* pMultiresultParentNode = NULL );

	CXMLNode*						CreateSetting			( CXMLNode *pDst, const char *szSetting, const char *szContent );
	inline CSettings::AccessType	GetAccessType			( char cCharacter );
	inline bool						HasPrefix				( char cCharacter );
	inline const char*				GetResourceName			( const char *szSetting, char *szBuffer, unsigned int uiLength );
	inline bool						HasResourceName			( const char *szSetting );
	inline const char*				GetName					( const char *szSetting, unsigned int uiResourceLength = -1 );

	char					m_szPath[MAX_PATH];
	CXMLFile*				m_pFile;
	CXMLNode*				m_pNodeGlobalSettings;

    CResourceManager*       m_pResourceManager;
};

#endif
