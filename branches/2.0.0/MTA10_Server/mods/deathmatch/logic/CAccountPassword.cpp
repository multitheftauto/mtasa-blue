/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CAccountPassword.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

///////////////////////////////////////////////////////////////
//
// CAccountPassword::SetPassword
//
// Set password from clear text, or previously saved hash
// Returns true if might need saving
//
///////////////////////////////////////////////////////////////
bool CAccountPassword::SetPassword( const SString& strPassword )
{
    if ( strPassword.empty() )
    {
        // Special case for empty password
        bool bChanged = !m_strSha256.empty();
        m_strSha256 = "";
        m_strSalt = "";
        m_strType = "";
        return bChanged;
    }
    else
    if ( strPassword.length() == 64 + 32 + 1 )
    {
        // If SHA256 + salt + type, then store as such
        m_strSha256 = strPassword.SubStr( 0, 64 );
        m_strType = strPassword.SubStr( 64, 1 );
        m_strSalt = strPassword.SubStr( 65, 32 );
        return false;
    }
    else
    if ( strPassword.length() == 32 )
    {
        // If MD5, convert to SHA256 + salt + type
        m_strSalt = GenerateSalt();
        m_strType = "1";
        m_strSha256 = GenerateSha256HexString( m_strSalt + strPassword.ToUpper() );
        return true;
    }
    else
    {
        // Plain text, convert to SHA256 + salt + type
        m_strSalt = GenerateSalt();
        m_strType = "0";
        m_strSha256 = GenerateSha256HexString( m_strSalt + strPassword );
        return true;
    }
}


///////////////////////////////////////////////////////////////
//
// CAccountPassword::IsPassword
//
// Check if supplied clear text password is correct
//
///////////////////////////////////////////////////////////////
bool CAccountPassword::IsPassword( const SString& strPassword )
{
    // Empty passwords never match
    if ( strPassword.empty() || m_strSha256.empty() )
        return false;

    if ( m_strType == "1" )
    {
        // Password hash was generated from MD5, so do the same thing for the test
        SString strMd5 = CMD5Hasher::CalculateHexString( strPassword.c_str(), strPassword.length() ).ToUpper();
        SString strPasswordHashed = GenerateSha256HexString( m_strSalt + strMd5 );
        return strPasswordHashed == m_strSha256;
    }
    else
    {
        // Password hash was generated from clear text
        SString strPasswordHashed = GenerateSha256HexString( m_strSalt + strPassword );
        return strPasswordHashed == m_strSha256;
    }
}


///////////////////////////////////////////////////////////////
//
// CAccountPassword::CanChangePasswordTo
//
// Check if supplied clear text password is different to what we have
//
///////////////////////////////////////////////////////////////
bool CAccountPassword::CanChangePasswordTo( const SString& strPassword )
{
    // No change if going from empty to empty
    if ( strPassword.empty() && m_strSha256.empty() )
        return false;

    // Yes change if going to/from empty
    if ( strPassword.empty() || m_strSha256.empty() )
        return true;

    // No change if already same
    if ( IsPassword( strPassword ) )
        return false;

    // Yes change if different
    return true;
}


///////////////////////////////////////////////////////////////
//
// CAccountPassword::GetPasswordHash
//
// Return password hash for saving
//
///////////////////////////////////////////////////////////////
SString CAccountPassword::GetPasswordHash( void )
{
    return m_strSha256 + m_strType + m_strSalt;
}


///////////////////////////////////////////////////////////////
//
// CAccountPassword::GenerateSalt
//
// Generate new random salt string
//
///////////////////////////////////////////////////////////////
SString CAccountPassword::GenerateSalt( void )
{
    char randomData[16];
    g_pNetServer->GenerateRandomData( randomData, sizeof( randomData ) );
    randomData[8] = rand();

    SString strSalt = ConvertDataToHexString( randomData, sizeof( randomData ) );

    assert( strSalt.length() == 32 );
    return strSalt;
}
