/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CSettings.cpp
*  PURPOSE:     XML-based variable settings class
*  DEVELOPERS:  Cecill Etheredge <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define ERROR_BUFFER            32

CSettings::CSettings ( CResourceManager* pResourceManager )
{
    m_pResourceManager = pResourceManager;
    m_pNodeGlobalSettings = NULL;

    // Get the absolute path to the global settings xml file
    m_strPath = g_pServerInterface->GetModManager ()->GetAbsolutePath ( FILENAME_SETTINGS );

    // Open the XML file and check for validity
    if ( !( m_pFile = g_pServerInterface->GetXML ()->CreateXML ( m_strPath ) ) ) {
        CLogger::ErrorPrintf ( "Could not create XML instance for settings registry! Registry disabled." );
        return;
    }

    // Parse the file
    if ( !m_pFile->Parse () || (m_pNodeGlobalSettings = m_pFile->GetRootNode ()) == NULL ) {
        // Read the error into the error buffer
        std::string strError;
        m_pFile->GetLastError ( strError );

        // Print the error buffer
        CLogger::LogPrintf ( "File '%s' not present, creating new node.\n", FILENAME_SETTINGS );

        // Create a new root node
        m_pNodeGlobalSettings = m_pFile->CreateRootNode ( ROOTNODE_SETTINGS );

        if ( !m_pFile->Write () )
            CLogger::ErrorPrintf ( "Error saving '%s'\n", FILENAME_SETTINGS );
    }
}

CSettings::~CSettings ( void )
{
    if ( m_pNodeGlobalSettings )
        delete m_pNodeGlobalSettings;

    if ( m_pFile )
        delete m_pFile;
}


// Get ( [xml node], [xml storage dummy node], [xml node owner resource name], [querying resource name], [setting identifier] )
//
// The XML storage dummy node is used to output all nodes to which we have access (instead of returning the entire root node with
// all nodes in it), and is only needed when you want to return tables with multiple entries.
// Be sure to ALWAYS remove the storage node after calling Get!
//
// Returns the XML node in pNode
//
// Status values: NotFound (none found), NoAccess (no access/error) or Found (found)
CXMLNode* CSettings::Get ( CXMLNode *pSource, CXMLNode *pStorage, const char *szSourceResource, const char *szLocalResource, const char *szSetting, bool &bDeleteNode, SettingStatus &eStatus, CXMLNode* pMultiresultParentNode )
{
    CXMLNode *pNode = NULL;
    unsigned int uiCurrentIndex = 0, uiResourceNameLength = 0;
    char szQueryResource[MAX_RESOURCE_LENGTH] = {0}, szResource[MAX_RESOURCE_LENGTH] = {0};
    const char *szName, *szQueryName = NULL;
    eStatus = NoAccess;
    bDeleteNode = false;

    // Return if there was no source
    if ( pSource == NULL )
        return NULL;

    // Get the resource name from the specified setting
    if ( !GetResourceName ( szSetting, szQueryResource, MAX_RESOURCE_LENGTH - 1 ) ) {   // (something): No resource specified, so use the local resource name
        strncpy ( szQueryResource, szLocalResource, MAX_RESOURCE_LENGTH - 1 );
    } else {
        // Save GetName the hassle of calling GetResourceName again by calculating the resource name length ourselves
        uiResourceNameLength = strlen ( szQueryResource ) + 1;
    }

    // Extract attribute name if setting to be gotten has three parts i.e. resname.settingname.attributename
    SString strSetting = szSetting;
    SString strAttribute = "value";
    vector < SString > Result;
    strSetting.Split ( ".", Result );
    if ( Result.size () == 3 && Result[2].length () )
    {
        strSetting = Result[0] + "." + Result[1];
        strAttribute = Result[2];
    }

    // Get the actual name from the specified setting
    if ( !(szQueryName = GetName ( strSetting, uiResourceNameLength ) ) ) {
        // No name specified, so make sure we eventually return the entire node
        bDeleteNode = true;
    }

    // Iterate through all the setting subnodes
    while ( ( pNode = pSource->FindSubNode ( "setting", uiCurrentIndex++ ) ) ) {
        std::string strContent;
        unsigned int uiResourceNameLength = 0;

        CXMLAttribute* pName    = pNode->GetAttributes ().Find ( "name" );
        CXMLAttribute* pValue   = pNode->GetAttributes ().Find ( strAttribute );

        // Check if both attibutes exist (otherwise ignore the entry)
        if ( pName && pValue ) {
            // Read the settings name and see if its valid
            strContent = pName->GetValue ();
            if ( strContent.empty () ) continue;

            // Parse the settings name and store the resource name in szResource
            if ( !GetResourceName ( strContent.c_str (), szResource, MAX_RESOURCE_LENGTH - 1 ) ) {
                // If there was no resource name, copy the owner's name
                strncpy ( szResource, szSourceResource, MAX_RESOURCE_LENGTH - 1 );
            } else {
                // Save GetName the hassle of calling GetResourceName again by calculating the resource name length ourselves
                uiResourceNameLength = strlen ( szResource ) + 1;
            }

            // Get the access type
            AccessType eAccess = GetAccessType ( strContent.at(0) );

            // Parse the settings name and store the split off name in szName (skip the prefix, if any)
            szName = GetName ( strContent.c_str (), uiResourceNameLength );

            // Compare the results
            if ( bDeleteNode ) {
                // If we're walking through all resources (no resource nor setting name was specified) - ...
                // Or if we're walking through a specific resource - ...
                if ( ( uiResourceNameLength == 0 && ( stricmp ( szResource, szLocalResource ) == 0 || eAccess != CSettings::Private ) )
                    || ( uiResourceNameLength > 0 && ( ( stricmp ( szResource, szQueryResource ) == 0 && ( eAccess != CSettings::Private || stricmp ( szResource, szLocalResource ) == 0 ) )
                    ) ) ) {
                    if ( pMultiresultParentNode == NULL ) {
                        // Create a new temporary node (in which we can put all nodes we have access to), and add it as temporary subnode of pSource
                        // The node itself will be deleted
                        pMultiresultParentNode = pStorage->CreateSubNode ( "setting" );
                    }

                    // We are meant to return an entire node. Since we are allowed to read this node, copy it and add it to our storage node
                    eStatus = Found;
                    CreateSetting ( pMultiresultParentNode, strContent.c_str (), pValue->GetValue ().c_str () );
                }
            } else if ( stricmp ( szName, szQueryName ) == 0 &&
                        stricmp ( szResource, szQueryResource ) == 0 ) {            // If the query name/resource and found node name/resource combinations are equal
                eStatus = (stricmp ( szResource, szLocalResource ) == 0 || eAccess != CSettings::Private) ? Found : NoAccess;
                return pNode;
            }
        }
    }
    // If we have multiple entries, return the storage node
    if ( bDeleteNode )
        return pMultiresultParentNode;

    // Otherwise, return NULL
    eStatus = NotFound;
    return NULL;
}

// Get ( [resource requesting the query], [setting name], [return buffer], [return buffer length], [delete node] )
//
// bDeleteNode contains a boolean to indicate whether the table contains one entry or multiple entries. If multiple
// entries are used, the resource's storage node (which only gets deleted on resource stop) is used to store any
// matching nodes that you have access to. The boolean is meant to let the caller know that the XML node that is
// returned should be deleted after usage, for the sake of memory usage.
//
// Returns the XML node.
//
// If ( bDeleteNode == TRUE ), always remove the XML node after you're done with it!
CXMLNode* CSettings::Get ( const char *szLocalResource, const char *szSetting, bool& bDeleteNode )
{
    CResource *pResource;
    CXMLNode *pNode = NULL;
    char szQueryResource[MAX_RESOURCE_LENGTH] = {0};
    SettingStatus eStatus = NotFound;
    bDeleteNode = false;

    // Get the temporary storage node associated with this resource
    CXMLNode *pStorage = m_pResourceManager->GetResource ( szLocalResource )->GetStorageNode ();

    // Get the actual resource name from the specified setting, and get the resource class
    if ( !GetResourceName ( szSetting, szQueryResource, MAX_RESOURCE_LENGTH - 1 ) ) {
        // No name was specified, so use the local resource
        pResource = m_pResourceManager->GetResource ( szLocalResource );
    } else
        pResource = m_pResourceManager->GetResource ( szQueryResource );

    // If we have a valid resource
    if ( pResource ) {
        CXMLNode *pSource = pResource->GetSettingsNode ();

        // Try to get the value for the appropriate setting from the settings registry
        if ( pStorage )
        {
            pNode = Get ( m_pNodeGlobalSettings, pStorage, "", szLocalResource, szSetting, bDeleteNode, eStatus );
            // If we're getting all of the resource's settings, throw in those from the meta as well
            if ( bDeleteNode )
            {
                SettingStatus eMetaStatus = NotFound;
                CXMLNode* pMetaNode = Get ( pSource, pStorage, pResource->GetName ().c_str (), szLocalResource, szSetting, bDeleteNode, eMetaStatus, pNode );
                if ( eMetaStatus == Found )
                {
                    eStatus = eMetaStatus;
                    pNode = pMetaNode;
                }
            }
        }

        // See if we found a matching setting
        if ( eStatus == Found )
            return pNode;   // Found
        else if ( eStatus == NotFound ) {       // Not found, continue searching
            // Try to get the value for the appropriate setting from the resource's meta XML file
            if ( pSource )
                pNode = Get ( pSource, pStorage, pResource->GetName ().c_str (), szLocalResource, szSetting, bDeleteNode, eStatus );
            if ( eStatus == Found )
                return pNode;
        }
    } else {
        // Try to get the value for the appropriate setting from the settings registry
        pNode = Get ( m_pNodeGlobalSettings, pStorage, "", szLocalResource, szSetting, bDeleteNode, eStatus );
        if ( eStatus == Found )
            return pNode;
    }

    return NULL;                            // No access or no settings found
}

// Creates a new setting and adds it to the destination node
CXMLNode* CSettings::CreateSetting ( CXMLNode *pDst, const char *szName, const char *szContent )
{
    // Create the node
    CXMLNode *pNode = pDst->CreateSubNode ( "setting" );
    CXMLAttributes *pAttributes = &(pNode->GetAttributes ());

    // Add the attributes with the corresponding values
    pAttributes->Create ( "name" )->SetValue ( szName );
    pAttributes->Create ( "value" )->SetValue ( szContent );

    return pNode;
}

// Set ( resource requesting the query, setting name, content )
bool CSettings::Set ( const char *szLocalResource, const char *szSetting, const char *szContent )
{
    CXMLNode *pNode;
    CResource *pResource;
    CXMLAttributes *pAttributes;
    char szBuffer[MAX_SETTINGS_LENGTH] = {0};
    char szQueryResource[MAX_RESOURCE_LENGTH] = {0};
    SettingStatus eStatus;
    bool bDeleteNode, bExists;
    SString strOldValue;

    // Check for empty strings
    if ( strlen ( szSetting ) < 1 ) return false;

    // Get the actual resource name from the specified setting, and get the resource class
    if ( !GetResourceName ( szSetting, szQueryResource, MAX_RESOURCE_LENGTH - 1 ) ) {
        // No name was specified, so use the local resource
        pResource = m_pResourceManager->GetResource ( szLocalResource );
    } else
        pResource = m_pResourceManager->GetResource ( szQueryResource );

    // If we have a valid resource
    if ( pResource ) {
        CXMLNode *pSource = pResource->GetSettingsNode ();

        // Check whether the setting exists in the settings registry
        pNode = Get ( m_pNodeGlobalSettings, NULL, "", szLocalResource, szSetting, bDeleteNode, eStatus );
        bExists = true;         // Default value

        // Try to get the value for the appropriate setting from the resource's meta XML file
        if ( eStatus == NotFound && pSource ) {
            pNode = Get ( pSource, NULL, pResource->GetName ().c_str (), szLocalResource, szSetting, bDeleteNode, eStatus );
            bExists = false;    // There's no node in the settings registry, so we create one
        }

        // See if we have access
        if ( eStatus != NoAccess ) {
            // See if we have a prefix
            bool bPrefix = HasPrefix ( szSetting[0] );

            // If no resource name was specified, use the local resource name
            if ( !HasResourceName ( szSetting ) ) {
                // If we have a prefix, move it from szSetting and put it at the beginning
                if ( bPrefix )
                    snprintf ( szBuffer, MAX_SETTINGS_LENGTH - 1, "%c%s.%s", szSetting[0], szLocalResource, szSetting + 1 );
                else
                    snprintf ( szBuffer, MAX_SETTINGS_LENGTH - 1, "%s.%s", szLocalResource, szSetting );

            } else {
                // If we have a prefix, move it from szSetting and put it at the beginning
                if ( bPrefix )
                    snprintf ( szBuffer, MAX_SETTINGS_LENGTH - 1, "%c%s", szSetting[0], szSetting + 1 );
                else
                    strncpy ( szBuffer, szSetting, MAX_SETTINGS_LENGTH - 1 );
            }

            if ( !bExists || !pNode ) {             // No existing settings registry entry, so create a new setting
                CreateSetting ( m_pNodeGlobalSettings, szBuffer, szContent );
            } else {                        // Existing settings registry entry
                // Get the attributes
                pAttributes = &(pNode->GetAttributes ());

                // Abort if this value isnt public (but protected or private), and if the local resource
                // (doing the query) doesn't equal the setting's resource name
                if ( GetAccessType ( pAttributes->Find ( "name" )->GetValue()[0] ) != CSettings::Public &&
                     stricmp ( pResource->GetName ().c_str (), szLocalResource ) != 0 )
                    return false;

                // Get the node's current value
                strOldValue = pAttributes->Find ( "value" )->GetValue ();

                // Set the node's value
                pAttributes->Find ( "value" )->SetValue ( szContent );

                // If a prefix was given, set the node's name (to override any access operators)
                if ( bPrefix )
                    pAttributes->Find ( "name" )->SetValue ( szBuffer );
            }

            // Trigger onSettingChange
            CLuaArguments Arguments;
            Arguments.PushString ( szSetting );

            if ( strOldValue.length () > 0 )
                Arguments.PushString ( strOldValue.c_str () );
            else
                Arguments.PushNil ();

            Arguments.PushString ( szContent );

            g_pGame->GetMapManager ()->GetRootElement ()->CallEvent ( "onSettingChange", Arguments );

            // Save the XML file
            if ( m_pFile->Write () )
                return true;
            CLogger::ErrorPrintf ( "Error saving '%s'\n", FILENAME_SETTINGS );
        }
    }

    return false;
}

// Checks if there is a valid prefix
bool CSettings::HasPrefix ( char cCharacter ) {
    switch ( cCharacter ) {
        case SETTINGS_PREFIX_PRIVATE:       // Private variable
            return true;
        case SETTINGS_PREFIX_PUBLIC:        // Public variable
            return true;
        case SETTINGS_PREFIX_PROTECTED:     // Protected variable
            return true;
        default:                            // No variable
            return false;
    }
}

// Parses the prefix and returns the appropriate access type
CSettings::AccessType CSettings::GetAccessType ( char cCharacter ) {
    switch ( cCharacter ) {
        case SETTINGS_PREFIX_PRIVATE:       // Private variable
            return CSettings::Private;
        case SETTINGS_PREFIX_PUBLIC:        // Public variable
            return CSettings::Public;
        case SETTINGS_PREFIX_PROTECTED:     // Protected variable
            return CSettings::Protected;
        default:                            // Default variable (as declared in SETTINGS_NO_PREFIX)
            return SETTINGS_NO_PREFIX;
    }
}

// Parses the name and returns the resource name associated with the setting
inline const char* CSettings::GetResourceName ( const char *szSetting, char *szBuffer, unsigned int uiLength ) {
    const char *szChr = strchr ( szSetting, SETTINGS_DELIMITER );
    if ( szChr == NULL ) return NULL;           // Not found? Return NULL, because there's no resource name.

    // Strip off the prefix, if any
    if ( HasPrefix ( szSetting[0] ) ) szSetting++;

    // Get the position of the delimiter
    unsigned int uiPos = (unsigned int)(szChr - szSetting);

    // Locate the first occurence of the delimiter in the name and use it to substring the name
    strncpy ( szBuffer, szSetting, ( uiPos > uiLength ) ? uiLength : uiPos );
    szBuffer [ ( uiPos > uiLength ) ? uiLength : uiPos ] = 0;

    return szBuffer;
}

// Parses the name and returns whether a resource name was specified
inline bool CSettings::HasResourceName ( const char *szSetting ) {
    return ( strchr ( szSetting, SETTINGS_DELIMITER ) != NULL );
}

// Parses the name and returns the actual name of the variable
inline const char* CSettings::GetName ( const char *szSetting, unsigned int uiResourceLength ) {
    // Only calculate the resource length if it's not already specified
    if ( HasPrefix ( szSetting[0] ) ) szSetting++;

    // Since we know the resource length, we just return the pointer to whereever the resource part ends (and the name begins)
    const char * szName = ( szSetting + uiResourceLength );

    // If we don't have a name, return
    if ( szName[0] == 0 ) return NULL;

    // Strip any prefixes off the name
    if ( HasPrefix ( szName[0] ) ) szName++;

    return szName;
}
