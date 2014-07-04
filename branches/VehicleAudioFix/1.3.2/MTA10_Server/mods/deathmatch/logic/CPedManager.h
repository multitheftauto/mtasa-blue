/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPedManager.h
*  PURPOSE:     Ped entity manager class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CPedManager;

#ifndef __CPedManager_H
#define __CPedManager_H

class CPedManager
{
    friend class CPed;
public:
                                    CPedManager                 ( void );
                                    ~CPedManager                ( void );

    class CPed*                     Create                      ( unsigned short usModel, CElement* pParent, CXMLNode* pNode = NULL );
    class CPed*                     CreateFromXML               ( CElement* pParent, CXMLNode& Node, CLuaMain* pLuaMain, CEvents* pEvents );
    void                            DeleteAll                   ( void );

    inline unsigned int             Count                       ( void )                            { return static_cast < unsigned int > ( m_List.size () ); };
    bool                            Exists                      ( class CPed* pPed );

    list < class CPed* > ::const_iterator IterBegin             ( void )                            { return m_List.begin (); }
    list < class CPed* > ::const_iterator IterEnd               ( void )                            { return m_List.end (); }

    static bool                     IsValidModel                ( unsigned short usModel );

protected:
    inline void                     AddToList                   ( class CPed* pPed )                { m_List.push_back ( pPed ); }
    void                            RemoveFromList              ( class CPed* pPed );

    list < class CPed* >            m_List;
};

#endif
