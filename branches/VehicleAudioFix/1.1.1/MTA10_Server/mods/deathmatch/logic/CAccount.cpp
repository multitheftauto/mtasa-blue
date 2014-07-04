/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CAccount.cpp
*  PURPOSE:     User account class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CAccount::CAccount ( CAccountManager* pManager, bool bRegistered, const std::string& strName, const std::string& strPassword, const std::string& strIP, int iUserID, const std::string& strSerial )
{
    m_pClient = NULL;

    m_iUserID = 0;
    m_bChanged = false;

    m_pManager = pManager;

    m_bRegistered = bRegistered;

    m_uiNameHash = 0;
    SetName ( strName );

    m_strPassword = strPassword;
    SetIP ( strIP );
    SetSerial ( strSerial );
    SetID ( iUserID );

    m_pManager->AddToList ( this );
    m_pManager->MarkAsChanged ( this );
}


CAccount::~CAccount ( void )
{
    
    if ( m_pClient )
        m_pClient->SetAccount ( NULL );

    m_pManager->RemoveFromList ( this );
    m_pManager->MarkAsChanged ( this );
}


void CAccount::Register ( const char* szPassword )
{
    HashPassword ( szPassword, m_strPassword );
    m_bRegistered = true;

    m_pManager->MarkAsChanged ( this );
}


void CAccount::SetName ( const std::string& strName )
{
    if ( m_strName != strName )
    {
        m_pManager->ChangingName ( this, m_strName, strName );

        m_strName = strName;

        if ( !m_strName.empty () )
            m_uiNameHash = HashString ( m_strName.c_str () );

        m_pManager->MarkAsChanged ( this );
    }
}


bool CAccount::IsPassword ( const char* szPassword )
{
    if ( szPassword )
    {
        std::string strPassword(szPassword);
        //First check if the raw string matches the account password
        if ( strPassword == m_strPassword )
        {
            //We have an unhashed password, so lets update it
            SetPassword ( szPassword );
            return true;
        }
        HashPassword ( szPassword, strPassword );
        // Lower case, we dont need a case sensetive comparsion on hashes
        std::transform ( strPassword.begin(), strPassword.end(), strPassword.begin(), ::tolower );
        std::transform ( m_strPassword.begin(), m_strPassword.end(), m_strPassword.begin(), ::tolower );
        return m_strPassword == strPassword;
    }

    return false;
}


void CAccount::SetPassword ( const char* szPassword )
{
    string strNewPassword;
    HashPassword ( szPassword, strNewPassword );
    if ( stricmp ( m_strPassword.c_str (), strNewPassword.c_str () ) != 0 )
    {
        m_strPassword = strNewPassword;
        m_pManager->MarkAsChanged ( this );
    }
}


bool CAccount::HashPassword ( const char* szPassword, std::string& strHashPassword )
{
    char szHashed[33];
    if ( szPassword && strlen ( szPassword ) > 0 )
    {
        MD5 Password;
        CMD5Hasher Hasher;
        Hasher.Calculate ( szPassword, strlen ( szPassword ), Password );
        Hasher.ConvertToHex ( Password, szHashed );
        strHashPassword = szHashed;
        return true;
    }
    return false;
}

void CAccount::SetIP ( const std::string& strIP )
{
    if ( m_strIP != strIP )
    {
        m_strIP = strIP;
        m_pManager->MarkAsChanged ( this );
    }
}

void CAccount::SetSerial ( const std::string& strSerial )
{
    if ( m_strSerial != strSerial )
    {
        m_strSerial = strSerial;
        m_pManager->MarkAsChanged ( this );
    }
}
void CAccount::SetID ( int iUserID )
{
    if ( m_iUserID != iUserID )
    {
        m_iUserID = iUserID;
    }
}
