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

                                        _snprintf ( m_szName, 256, "%s", szName );
                                        m_szName [ 255 ] = 0;

                                        m_eObjectType = eObjectType;
                                    }

    virtual                         ~CAccessControlListGroupObject      ( void )        { };

    static inline unsigned int      GenerateHashId                      ( const char* szName, EObjectType eObjectType )
                                    {
                                        char szID [ 256 ];
                                        _snprintf ( szID, 256, "%s_%d", szName, (unsigned int) eObjectType );
                                        szID [ 255 ] = 0;
                                        return ( HashString( szID ) & (unsigned int)0x7FFFFFFF );
                                    }

    inline const char*              GetObjectName                       ( void )        { return m_szName; };
    inline EObjectType              GetObjectType                       ( void )        { return m_eObjectType; };
    inline unsigned int             GetObjectHashId                     ( void )        { return m_uiHashId; };

private:
    char                            m_szName                            [ 256 ];
    EObjectType                     m_eObjectType;
    unsigned int                    m_uiHashId;

};

class CAccessControlListGroup
{
public:
                                                    CAccessControlListGroup     ( const char* szGroupName );
                                                    ~CAccessControlListGroup    ( void );

    inline const char*                              GetGroupName                ( void )        { return m_szGroupName; };

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

private:
    typedef std::list < class CAccessControlList* >
                                                    ACLsList;

    typedef std::list < class CAccessControlListGroupObject* >
                                                    ObjectList;

    typedef google::dense_hash_map < unsigned int, class CAccessControlListGroupObject* >
                                                    ObjectMap;

    char                                            m_szGroupName               [ 256 ];
    
    ACLsList                                        m_ACLs;

    ObjectList                                      m_Objects;
    ObjectMap                                       m_ObjectsById;
};

#endif

