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
protected:
    SString     GenerateSalt        ( void );

    SString     m_strSha256;
    SString     m_strSalt;
    SString     m_strType;
};

class CAccountData;
class CAccount
{
public:
                                CAccount                ( class CAccountManager* pManager, bool bRegistered, const std::string& strName, const std::string& strPassword = "", const std::string& strIP = "", int iUserID = 0, const std::string& strSerial = "");
                                ~CAccount               ( void );

    inline bool                 IsRegistered            ( void )                    { return m_bRegistered; }
    void                        Register                ( const char* szPassword );

    inline unsigned int         GetNameHash             ( void )                    { return m_uiNameHash; }
    inline const std::string&   GetName                 ( void )                    { return m_strName; }
    void                        SetName                 ( const std::string& strName );

    void                        SetPassword             ( const SString& strPassword );
    bool                        IsPassword              ( const SString& strPassword );
    SString                     GetPasswordHash         ( void );

    inline const std::string&   GetIP                   ( void )                    { return m_strIP; }
    void                        SetIP                   ( const std::string& strIP );

    inline const std::string&   GetSerial               ( void )                    { return m_strSerial; }
    void                        SetSerial               ( const std::string& strSerial );

    inline int                  GetID                   ( void )                    { return m_iUserID; }
    void                        SetID                   ( int iUserID );

    inline class CClient*       GetClient               ( void )                    { return m_pClient; }
    inline void                 SetClient               ( class CClient* pClient )  { m_pClient = pClient; }

    inline void                 SetChanged              ( bool bChanged )           { m_bChanged = bChanged; }
    inline bool                 HasChanged              ( void )                    { return m_bChanged; }
    uint                        GetScriptID             ( void ) const              { return m_uiScriptID; }

    CLuaArgument*               GetData                 ( const std::string& strKey );
    void                        SetData                 ( const std::string& strKey, const std::string& strValue, int iType );
    void                        RemoveData              ( const std::string& strKey );
    std::list < CAccountData >  ::iterator DataBegin    ( void )                    { return m_Data.begin (); }
    std::list < CAccountData >  ::iterator DataEnd      ( void )                    { return m_Data.end (); }

 protected:
    CAccountData*               GetDataPointer ( const std::string& strKey );

    CAccountManager*            m_pManager;

    bool                        m_bRegistered;
    std::string                 m_strName;
    CAccountPassword            m_Password;
    std::string                 m_strIP;
    std::string                 m_strSerial;
    int                         m_iUserID;

    bool                        m_bChanged;

    unsigned int                m_uiNameHash;

    class CClient*              m_pClient;
    uint                        m_uiScriptID;

    std::list < CAccountData >  m_Data;
};


class CAccountData
{
public:
                            CAccountData ( const std::string& strKey, const std::string& strValue, int iType )
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
