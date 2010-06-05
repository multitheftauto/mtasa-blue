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

CAccount::CAccount ( CAccountManager* pManager, bool bRegistered, std::string strName, std::string strPassword, std::string strIP, std::string strSerial )
{
    m_pClient = NULL;

    m_pManager = pManager;

    m_bRegistered = bRegistered;

    m_uiNameHash = 0;
    SetName ( strName );

    m_strPassword = strPassword;
    SetIP ( strIP );
    SetSerial ( strSerial );

    m_pManager->AddToList ( this );
    m_pManager->MarkAsChanged ();
}


CAccount::~CAccount ( void )
{
    ClearData ();
    
    if ( m_pClient )
        m_pClient->SetAccount ( NULL );

    m_pManager->RemoveFromList ( this );
    m_pManager->MarkAsChanged ();
}


void CAccount::Register ( const char* szPassword )
{
    HashPassword ( szPassword, m_strPassword );
    m_bRegistered = true;

    m_pManager->MarkAsChanged ();
}


void CAccount::SetName ( std::string strName )
{
    m_strName = strName;

    if ( !m_strName.empty () )
        m_uiNameHash = HashString ( m_strName.c_str () );

    m_pManager->MarkAsChanged ();
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
    HashPassword ( szPassword, m_strPassword );
    m_pManager->MarkAsChanged ();
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


CLuaArgument * CAccount::GetData ( char* szKey )
{
    CAccountData* pData = GetDataPointer ( szKey );
    if ( pData )
    {
        return pData->GetValue ();
    }

    return NULL;
}


void CAccount::SetData ( const char* szKey, CLuaArgument * pArgument )
{
    if ( szKey && szKey [ 0 ] && pArgument )
    {
        CAccountData* pData = GetDataPointer ( szKey );
        if ( pData )
        {
            pData->SetValue ( pArgument );
        }
        else
        {
            pData = new CAccountData ( szKey, pArgument );
            m_Data.push_back ( pData );
        }

        m_pManager->MarkAsChanged ();
    }
}


void CAccount::CopyData ( CAccount* pAccount )
{
    list < CAccountData* > ::iterator iter = pAccount->DataBegin ();
    for ( ; iter != pAccount->DataEnd () ; iter++ )
    {
        m_Data.push_back ( new CAccountData ( (*iter)->GetKey (), (*iter)->GetValue () ) );
    }

    m_pManager->MarkAsChanged ();
}


void CAccount::RemoveData ( char* szKey )
{
    CAccountData* pData = GetDataPointer ( szKey );
    if ( pData )
    {
        if ( !m_Data.empty() ) m_Data.remove ( pData );
        delete pData;

        m_pManager->MarkAsChanged ();
    }
}


void CAccount::ClearData ( void )
{
    list < CAccountData* > ::iterator iter = m_Data.begin ();
    for ( ; iter != m_Data.end () ; iter++ )
    {
        delete *iter;
    }

    m_Data.clear ();
    m_pManager->MarkAsChanged ();
}


CAccountData* CAccount::GetDataPointer ( const char* szKey )
{
    if ( szKey && szKey [ 0 ] )
    {
        list < CAccountData* > ::iterator iter = m_Data.begin ();
        for ( ; iter != m_Data.end () ; iter++ )
        {
            if ( (*iter)->GetKey ().compare ( szKey ) == 0 )
            {
                return *iter;
            }
        }
    }
    return NULL;
}
