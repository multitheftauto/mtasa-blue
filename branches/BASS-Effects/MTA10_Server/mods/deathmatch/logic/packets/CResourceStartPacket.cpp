/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CResourceStartPacket.cpp
*  PURPOSE:     Resource start packet class
*  DEVELOPERS:  Kevin Whiteside <>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CResourceStartPacket::CResourceStartPacket ( const char* szResourceName, CResource* pResource )
{
    m_strResourceName = szResourceName;
    m_pResource = pResource;
}


bool CResourceStartPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    if ( !m_strResourceName.empty () )
    {
        // Write the resource name
        unsigned char sizeResourceName = static_cast < unsigned char > ( m_strResourceName.size () );
        BitStream.Write ( sizeResourceName );
        if ( sizeResourceName > 0 )
        {
            BitStream.Write ( m_strResourceName.c_str (), sizeResourceName );
        }

        // Write the resource id
        BitStream.Write ( m_pResource->GetNetID () );
        
        // Write the resource element id
        BitStream.Write ( m_pResource->GetResourceRootElement ()->GetID () );

        // Write the resource dynamic element id
        BitStream.Write ( m_pResource->GetDynamicElementRoot ()->GetID () );
        
        // Count the amount of protected scripts
        unsigned short usProtectedScriptCount = 0;
        if ( m_pResource->IsClientScriptsOn () == true )
        {
            list < CResourceFile* > ::iterator iter = m_pResource->IterBegin();
            for ( ; iter != m_pResource->IterEnd (); iter++ )
            {
                if ( ( *iter )->GetType () == CResourceScriptItem::RESOURCE_FILE_TYPE_CLIENT_SCRIPT &&
                     static_cast<CResourceClientScriptItem*>(*iter)->IsProtected() == true )
                {
                    ++usProtectedScriptCount;
                }
            }
        }
        BitStream.Write ( usProtectedScriptCount );

        // Send the resource files info
        list < CResourceFile* > ::iterator iter = m_pResource->IterBegin();
        for ( ; iter != m_pResource->IterEnd (); iter++ )
        {
            if ( ( ( *iter )->GetType () == CResourceScriptItem::RESOURCE_FILE_TYPE_CLIENT_CONFIG && m_pResource->IsClientConfigsOn () ) ||
                 ( ( *iter )->GetType () == CResourceScriptItem::RESOURCE_FILE_TYPE_CLIENT_SCRIPT && m_pResource->IsClientScriptsOn () && static_cast<CResourceClientScriptItem*>(*iter)->IsProtected() == false ) ||
                 ( ( *iter )->GetType () == CResourceScriptItem::RESOURCE_FILE_TYPE_CLIENT_FILE && m_pResource->IsClientFilesOn () ) )
            {
                if ( ( *iter )->GetType () == CResourceScriptItem::RESOURCE_FILE_TYPE_CLIENT_FILE )
                {
                    CResourceClientFileItem* pRCFItem = reinterpret_cast < CResourceClientFileItem* > ( *iter );
                    // Write the Type of chunk to read (F - File, E - Exported Function)
                    BitStream.Write ( static_cast < unsigned char > ( 'F' ) );

                    // Write the map name
                    const char* szFileName = ( *iter )->GetWindowsName();
                    size_t sizeFileName = strlen ( szFileName );

                    // Make sure we don't have any backslashes in the name
                    char * szCleanedFilename = new char [ sizeFileName + 1 ];
                    strcpy ( szCleanedFilename, szFileName );
                    for ( unsigned int i = 0 ; i < sizeFileName ; i++ )
                    {
                        if ( szCleanedFilename [ i ] == '\\' )
                            szCleanedFilename [ i ] = '/';
                    }

                    BitStream.Write ( static_cast < unsigned char > ( sizeFileName ) );
                    if ( sizeFileName > 0 )
                    {
                        BitStream.Write ( szCleanedFilename, sizeFileName );
                    }

                    // ChrML: Don't forget this...
                    delete [] szCleanedFilename;

                    BitStream.Write ( static_cast < unsigned char > ( ( *iter )->GetType() ) );
                    CChecksum checksum = ( *iter )->GetLastChecksum ();
                    BitStream.Write ( checksum.ulCRC );
                    BitStream.Write ( (const char*)checksum.mD5, sizeof ( checksum.mD5 ) );
                    BitStream.Write ( ( *iter )->GetApproxSize () );
                    // write bool whether to download or not
                    BitStream.WriteBit ( pRCFItem->IsAutoDownload() );
                }
                else
                {
                    // Write the Type of chunk to read (F - File, E - Exported Function)
                    BitStream.Write ( static_cast < unsigned char > ( 'F' ) );

                    // Write the map name
                    const char* szFileName = ( *iter )->GetWindowsName();
                    size_t sizeFileName = strlen ( szFileName );

                    // Make sure we don't have any backslashes in the name
                    char * szCleanedFilename = new char [ sizeFileName + 1 ];
                    strcpy ( szCleanedFilename, szFileName );
                    for ( unsigned int i = 0 ; i < sizeFileName ; i++ )
                    {
                        if ( szCleanedFilename [ i ] == '\\' )
                            szCleanedFilename [ i ] = '/';
                    }

                    BitStream.Write ( static_cast < unsigned char > ( sizeFileName ) );
                    if ( sizeFileName > 0 )
                    {
                        BitStream.Write ( szCleanedFilename, sizeFileName );
                    }

                    // ChrML: Don't forget this...
                    delete [] szCleanedFilename;

                    BitStream.Write ( static_cast < unsigned char > ( ( *iter )->GetType() ) );
                    CChecksum checksum = ( *iter )->GetLastChecksum ();
                    BitStream.Write ( checksum.ulCRC );
                    BitStream.Write ( (const char*)checksum.mD5, sizeof ( checksum.mD5 ) );
                    BitStream.Write ( ( *iter )->GetApproxSize () );
                }
            }
        }

        // Loop through the exported functions
        list < CExportedFunction >::iterator iterExportedFunction = m_pResource->IterBeginExportedFunctions();
        for ( ; iterExportedFunction != m_pResource->IterEndExportedFunctions (); iterExportedFunction++)
        {
            // Check to see if the exported function is 'client'
            if ( iterExportedFunction->GetType () == CExportedFunction::EXPORTED_FUNCTION_TYPE_CLIENT )
            {
                // Write the Type of chunk to read (F - File, E - Exported Function)
                BitStream.Write ( static_cast < unsigned char > ( 'E' ) ) ;

                // Write the exported function
                std::string strFunctionName = iterExportedFunction->GetFunctionName ();
                size_t sizeFunctionName = strFunctionName.length ();

                BitStream.Write ( static_cast < unsigned char > ( sizeFunctionName ) );
                if ( sizeFunctionName > 0 )
                {
                    BitStream.Write ( strFunctionName.c_str (), sizeFunctionName );
                }
            }
        }

        return true;
    }

    return false;
}
