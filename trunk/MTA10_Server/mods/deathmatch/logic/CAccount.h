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

class CAccountData;
class CAccount
{
public:
                                CAccount                ( class CAccountManager* pManager, bool bRegistered, const std::string& strName, const std::string& strPassword = "", const std::string& strIP = "", const std::string& strSerial = "");
                                ~CAccount               ( void );

    inline bool                 IsRegistered            ( void )                    { return m_bRegistered; }
    void                        Register                ( const char* szPassword );

    inline unsigned int         GetNameHash             ( void )                    { return m_uiNameHash; }
    inline const std::string&   GetName                 ( void )                    { return m_strName; }
    void                        SetName                 ( const std::string& strName );

    inline const std::string&   GetPassword             ( void )                    { return m_strPassword; }
    void                        SetPassword             ( const char* szPassword );
    bool                        HashPassword            ( const char* szPassword, std::string &strHashPassword );
    bool                        IsPassword              ( const char* szPassword );

    inline const std::string&   GetIP                   ( void )                    { return m_strIP; }
    void                        SetIP                   ( const std::string& strIP );

    inline const std::string&   GetSerial               ( void )                    { return m_strSerial; }
    void                        SetSerial               ( const std::string& strSerial );
    
    inline class CClient*       GetClient               ( void )                    { return m_pClient; }
    inline void                 SetClient               ( class CClient* pClient )  { m_pClient = pClient; }

    CLuaArgument *              GetData                 ( char* szKey );
    void                        SetData                 ( const char* szKey, CLuaArgument * pArgument );
    void                        CopyData                ( CAccount* pAccount );
    void                        RemoveData              ( char* szKey );
    void                        ClearData               ( void );

    std::list < CAccountData* > ::iterator DataBegin    ( void )                    { return m_Data.begin (); }
    std::list < CAccountData* > ::iterator DataEnd      ( void )                    { return m_Data.end (); }
    unsigned int                DataCount               ( void )                    { return static_cast < unsigned int > ( m_Data.size () ); }

protected:
    CAccountData*               GetDataPointer          ( const char* szKey );

    CAccountManager*            m_pManager;

    bool                        m_bRegistered;
    std::string                 m_strName;
    std::string                 m_strPassword;
    std::string                 m_strIP;
    std::string                 m_strSerial;

    unsigned int                m_uiNameHash;

    std::list < CAccountData* > m_Data;

    class CClient*              m_pClient;
};


class CAccountData
{
public:
    inline                  CAccountData            ( const std::string& strKey, CLuaArgument * pArgument )
    {
        m_strKey = strKey;
        m_Argument = *pArgument;
    }

    inline const std::string&   GetKey                  ( void )                    { return m_strKey; }
    inline void                 SetKey                  ( const std::string& strKey){ m_strKey = strKey; }
    inline CLuaArgument*        GetValue                ( void )                    { return &m_Argument; }
    inline void                 SetValue                ( CLuaArgument* pArgument ) { m_Argument = *pArgument; }

private:
    std::string             m_strKey;
    CLuaArgument            m_Argument;
};

#endif
