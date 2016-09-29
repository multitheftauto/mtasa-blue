/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CAccessControlListGroup.h
*  PURPOSE:     Access control list group definition class
*  DEVELOPERS:  Oliver Brown <>
*               aru <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CAccessControlListGroup;

#ifndef __CACCESSCONTROLLISTGROUP_H
#define __CACCESSCONTROLLISTGROUP_H

#include "../Config.h"
#include <xml/CXMLNode.h>
#include <list>

class CAccessControlListGroupObject
{
public:
    enum EObjectType
    {
        OBJECT_TYPE_USER,
        OBJECT_TYPE_RESOURCE,
    };

public:
    inline explicit                 CAccessControlListGroupObject       ( const char* szName, EObjectType eObjectType )
                                    {
                                        m_strKey = GenerateKey ( szName, eObjectType );
                                        m_strName = szName;
                                        m_eObjectType = eObjectType;
                                    }

    virtual                         ~CAccessControlListGroupObject      ( void )        { };

    static SString                  GenerateKey                         ( const char* szName, EObjectType eObjectType )
                                    {
                                        return SString( "%s_%d", szName, (unsigned int) eObjectType );
                                    }

    inline const char*              GetObjectName                       ( void )        { return m_strName; };
    inline EObjectType              GetObjectType                       ( void )        { return m_eObjectType; };
    inline const SString&           GetObjectKey                        ( void )        { return m_strKey; };

private:
    SString                         m_strName;
    EObjectType                     m_eObjectType;
    SString                         m_strKey;

};

class CAccessControlListGroup
{
public:
                                                    CAccessControlListGroup     ( const char* szGroupName );
                                                    ~CAccessControlListGroup    ( void );

    inline const char*                              GetGroupName                ( void )        { return m_strGroupName; };

    CAccessControlListGroupObject*                  AddObject                   ( const char* szObjectName, CAccessControlListGroupObject::EObjectType eObjectType );
    bool                                            FindObjectMatch             ( const char* szObjectName, CAccessControlListGroupObject::EObjectType eObjectType );
    bool                                            RemoveObject                ( const char* szObjectName, CAccessControlListGroupObject::EObjectType eObjectType );

    bool                                            AddACL                      ( class CAccessControlList* pACL );
    bool                                            IsACLPresent                ( class CAccessControlList* pACL );
    class CAccessControlList*                       GetACL                      ( const char* szACLName );
    void                                            RemoveACL                   ( class CAccessControlList* pACL );

    list < class CAccessControlList* > ::iterator   IterBeginACL                ( void )        { return m_ACLs.begin (); };
    list < class CAccessControlList* > ::iterator   IterEndACL                  ( void )        { return m_ACLs.end (); };
    list < class CAccessControlListGroupObject* > ::iterator IterBeginObjects   ( void )        { return m_Objects.begin (); };
    list < class CAccessControlListGroupObject* > ::iterator IterEndObjects     ( void )        { return m_Objects.end (); };

    void                                            WriteToXMLNode              ( CXMLNode* pNode );
    uint                                            GetScriptID                 ( void ) const  { return m_uiScriptID; }

private:
    void                                            OnChange                    ( void );

    typedef std::list < class CAccessControlList* >
                                                    ACLsList;

    typedef std::list < class CAccessControlListGroupObject* >
                                                    ObjectList;

    typedef CFastHashMap < SString, class CAccessControlListGroupObject* >
                                                    ObjectMap;

    SString                                         m_strGroupName;
    
    ACLsList                                        m_ACLs;

    ObjectList                                      m_Objects;
    ObjectMap                                       m_ObjectsById;
    uint                                            m_uiScriptID;
};

#endif

