/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CAccount.h
*  PURPOSE:     User account class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CACCOUNT_H
#define __CACCOUNT_H

#include <list>
#include <string>
#include "lua/CLuaArgument.h"

class CAccountPassword
{
public:
    bool        SetPassword         ( const SString& strPassword );
    bool        IsPassword          ( const SString& strPassword );
    bool        CanChangePasswordTo ( const SString& strPassword );
    SString     GetPasswordHash     ( void );

    enum EAccountPasswordType {
        NONE, // "" (empty string) possibly means 'not a password'
        PLAINTEXT,
        MD5, // pass stored as sha256(md5)+salt+type, stored as "1"
        SHA256 // pass stored as sha256(plaintext)+salt+type, stored as "0"
    };
protected:
    SString     GenerateSalt        ( void );

    SString     m_strSha256;
    SString     m_strSalt;

    SString     m_strType; // can be "1", "0" or "" as shown in EAccountPasswordType
};

enum class EAccountType
{
    Guest,
    Console,
    Player,
};

class CAccountData;
class CAccount
{
public:
    struct SSerialUsage
    {
        SString     strSerial;
        SString     strAddedIp;
        time_t      tAddedDate;
        SString     strAuthWho;
        time_t      tAuthDate;
        SString     strLastLoginIp;
        time_t      tLastLoginDate;
        time_t      tLastLoginHttpDate;
        bool        IsAuthorized ( void ) const   { return tAuthDate != 0; }
    };

    ZERO_ON_NEW
                                CAccount                ( class CAccountManager* pManager, EAccountType accountType, const std::string& strName, const std::string& strPassword = "", int iUserID = 0, const std::string& strIP = "", const std::string& strSerial = "", const SString& strHttpPassAppend = "" );
                                ~CAccount               ( void );

    bool                        IsRegistered            ( void )                    { return m_AccountType != EAccountType::Guest; }
    bool                        IsConsoleAccount        ( void )                    { return m_AccountType == EAccountType::Console; }
    void                        OnLoginSuccess          ( const SString& strSerial, const SString& strIp );
    void                        OnLoginHttpSuccess      ( const SString& strIp );

    const SString&              GetName                 ( void )                    { return m_strName; }
    void                        SetName                 ( const std::string& strName );

    void                        SetPassword             ( const SString& strPassword );
    bool                        IsPassword              ( const SString& strPassword, bool* pbUsedHttpPassAppend=nullptr );
    SString                     GetPasswordHash         ( void );
    const SString&              GetHttpPassAppend       ( void )                    { return m_strHttpPassAppend; }
    void                        SetHttpPassAppend       ( const SString& strHttpPassAppend );

    inline const std::string&   GetIP                   ( void )                    { return m_strIP; }
    inline const std::string&   GetSerial               ( void )                    { return m_strSerial; }
    inline int                  GetID                   ( void )                    { return m_iUserID; }

    bool                        HasLoadedSerialUsage    ( void );
    void                        EnsureLoadedSerialUsage ( void );
    std::vector< SSerialUsage >& GetSerialUsageList     ( void );
    SSerialUsage*               GetSerialUsage          ( const SString& strSerial );
    bool                        IsIpAuthorized          ( const SString& strIp );
    bool                        IsSerialAuthorized      ( const SString& strSerial );
    bool                        AddSerialForAuthorization ( const SString& strSerial, const SString& strIp );
    bool                        AuthorizeSerial         ( const SString& strSerial, const SString& strWho );
    bool                        RemoveSerial            ( const SString& strSerial );
    void                        RemoveUnauthorizedSerials ( void );

    CClient*                    GetClient               ( void )                    { return m_pClient; }
    void                        SetClient               ( CClient* pClient );

    inline void                 SetChanged              ( bool bChanged )           { m_bChanged = bChanged; }
    inline bool                 HasChanged              ( void )                    { return m_bChanged; }
    uint                        GetScriptID             ( void ) const              { return m_uiScriptID; }

    std::shared_ptr<CLuaArgument> GetData               ( const std::string& strKey );
    bool                        SetData                 ( const std::string& strKey, const std::string& strValue, int iType );
    bool                        HasData                 ( const std::string& strKey );
    void                        RemoveData              ( const std::string& strKey );
    std::map < SString, CAccountData >::iterator DataBegin  ( void )                { return m_Data.begin (); }
    std::map < SString, CAccountData >::iterator DataEnd    ( void )                { return m_Data.end (); }

 protected:
    CAccountData*               GetDataPointer ( const std::string& strKey );

    CAccountManager*            m_pManager;

    EAccountType                m_AccountType;
    SString                     m_strName;
    CAccountPassword            m_Password;
    SString                     m_strHttpPassAppend;
    std::string                 m_strIP;
    std::string                 m_strSerial;
    int                         m_iUserID;
    bool                        m_bLoadedSerialUsage;
    std::vector< SSerialUsage > m_SerialUsageList;

    bool                        m_bChanged;

    class CClient*              m_pClient;
    uint                        m_uiScriptID;

    std::map < SString, CAccountData >  m_Data;
};


class CAccountData
{
public:
                            CAccountData ( const std::string& strKey = "", const std::string& strValue = "", int iType = 0 )
                                                                         { m_strKey = strKey; m_strValue = strValue; m_iType = iType; }

    const std::string&      GetKey       ( void )                        { return m_strKey; }
    const std::string&      GetStrValue  ( void )                        { return m_strValue; }
    int                     GetType      ( void )                        { return m_iType; }
    void                    SetStrValue  ( const std::string& strValue ) { m_strValue = strValue; }
    void                    SetType      ( int iType )                   { m_iType = iType; }

private:
    std::string              m_strKey;
    std::string              m_strValue;
    int                      m_iType;
};

#endif
