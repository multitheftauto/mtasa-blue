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
    m_uiScriptID = CIdArray::PopUniqueId ( this, EIdClass::ACCOUNT );
    m_pClient = NULL;

    m_iUserID = 0;
    m_bChanged = false;

    m_pManager = pManager;

    m_bRegistered = bRegistered;

    m_uiNameHash = 0;
    SetName ( strName );

    SetIP ( strIP );
    SetSerial ( strSerial );
    SetID ( iUserID );

    m_pManager->AddToList ( this );

    m_bChanged = false;
    if ( m_Password.SetPassword( strPassword ) )
        m_pManager->MarkAsChanged ( this );     // Save if password upgraded
}


CAccount::~CAccount ( void )
{
    CIdArray::PushUniqueId ( this, EIdClass::ACCOUNT, m_uiScriptID );
    if ( m_pClient )
        m_pClient->SetAccount ( NULL );

    m_pManager->RemoveFromList ( this );
    m_pManager->MarkAsChanged ( this );
}


void CAccount::Register ( const char* szPassword )
{
    SetPassword( szPassword );
    m_bRegistered = true;
    m_Data.clear();

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


void CAccount::SetClient( CClient* pClient )
{
    m_pClient = pClient;
    // Clear data cache if not linked to a client
    if ( !m_pClient )
        m_Data.clear();
}


bool CAccount::IsPassword ( const SString& strPassword )
{
    return m_Password.IsPassword( strPassword );
}


void CAccount::SetPassword ( const SString& strPassword )
{
    if ( m_Password.CanChangePasswordTo( strPassword ) )
    {
        m_Password.SetPassword( strPassword );
        m_pManager->MarkAsChanged ( this );
    }
}


SString CAccount::GetPasswordHash ( void )
{
    return m_Password.GetPasswordHash();
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

CAccountData* CAccount::GetDataPointer ( const std::string& strKey )
{
    return MapFind( m_Data, strKey );
}

CLuaArgument* CAccount::GetData ( const std::string& strKey )
{
    CAccountData* pData = GetDataPointer ( strKey );
    CLuaArgument* pResult = new CLuaArgument ();

    if ( pData )
    {
        if ( pData->GetType () == LUA_TBOOLEAN )
        {
            pResult->ReadBool ( pData->GetStrValue () == "true" );
        }
        else
        if ( pData->GetType () == LUA_TNUMBER )
        {
            pResult->ReadNumber ( strtod ( pData->GetStrValue ().c_str(), NULL ) );
        }
        else
        {
            pResult->ReadString ( pData->GetStrValue () );
        }
    }
    else
    {
        pResult->ReadBool ( false );
    }
    return pResult;
}

// Return true if data was changed
bool CAccount::SetData ( const std::string& strKey, const std::string& strValue, int iType )
{
    if ( strValue == "false" && iType == LUA_TBOOLEAN )
    {
        if ( HasData( strKey ) )
        {
            RemoveData ( strKey );
            return true;
        }
    }
    else
    {
        CAccountData* pData = GetDataPointer ( strKey );
        
        if ( pData )
        {
            if ( pData->GetType() != iType || pData->GetStrValue() != strValue )
            {
                pData->SetStrValue ( strValue );
                pData->SetType ( iType );
                return true;
            }
        }
        else
        {
            MapSet ( m_Data, strKey, CAccountData ( strKey, strValue, iType ) );
            return true;
        }
    }
    return false;
}

bool CAccount::HasData ( const std::string& strKey )
{
    return MapContains( m_Data, strKey );
}

void CAccount::RemoveData ( const std::string& strKey )
{
    MapRemove( m_Data, strKey );
}