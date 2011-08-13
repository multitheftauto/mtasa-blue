/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CAccessControlListRight.h
*  PURPOSE:     Access control list rights class
*  DEVELOPERS:  Oliver Brown <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CAccessControlListRight;

#ifndef __CACCESSCONTROLLISTRIGHT_H
#define __CACCESSCONTROLLISTRIGHT_H

#define MAX_ACL_RIGHT_NAME_LENGTH 256

void OnACLChange ();

class CAccessControlListRight
{
public:
    enum ERightType
    {
        RIGHT_TYPE_COMMAND,
        RIGHT_TYPE_FUNCTION,
        RIGHT_TYPE_RESOURCE,
        RIGHT_TYPE_GENERAL
    };

public:
                                                CAccessControlListRight     ( const char* szRightName, ERightType eRightType, bool bAccess, class CAccessControlListManager* pACLManager );
    virtual                                     ~CAccessControlListRight    ( void )            { OnACLChange (); }

    inline const char*                          GetRightName                ( void )            { return m_szRightName; };
    inline unsigned int                         GetRightNameHash            ( void )            { return m_uiNameHash; };
    inline ERightType                           GetRightType                ( void )            { return m_eRightType; };

    inline bool                                 GetRightAccess              ( void )            { return m_bAccess; };
    inline void                                 SetRightAccess              ( bool bAccess)     { m_bAccess = bAccess; OnACLChange (); };

private:
    char                                        m_szRightName               [ MAX_ACL_RIGHT_NAME_LENGTH ];
    unsigned int                                m_uiNameHash;
    ERightType                                  m_eRightType;
    bool                                        m_bAccess;
    class CAccessControlListManager*            m_pACLManager;

};

#endif
