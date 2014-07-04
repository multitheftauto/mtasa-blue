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
#include "Utils.h"
#include <xml/CXMLNode.h>
#include <list>

#include <google/dense_hash_map>

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
                                        m_uiHashId = GenerateHashId ( szName, eObjectType );
                                        m_strName = szName;
                                        m_eObjectType = eObjectType;
                                    }

    virtual                         ~CAccessControlListGroupObject      ( void )        { };

    static inline unsigned int      GenerateHashId                      ( const char* szName, EObjectType eObjectType )
                                    {
                                        SString strID ( "%s_%d", szName, (unsigned int) eObjectType );
                                        return ( HashString( strID ) & (unsigned int)0x7FFFFFFF );
                                    }

    inline const char*              GetObjectName                       ( void )        { return m_strName; };
    inline EObjectType              GetObjectType                       ( void )        { return m_eObjectType; };
    inline unsigned int             GetObjectHashId                     ( void )        { return m_uiHashId; };

private:
    SString                         m_strName;
    EObjectType                     m_eObjectType;
    unsigned int                    m_uiHashId;

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

    typedef google::dense_hash_map < unsigned int, class CAccessControlListGroupObject* >
                                                    ObjectMap;

    SString                                         m_strGroupName;
    
    ACLsList                                        m_ACLs;

    ObjectList                                      m_Objects;
    ObjectMap                                       m_ObjectsById;
    uint                                            m_uiScriptID;
};

#endif

