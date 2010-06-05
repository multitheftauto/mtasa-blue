/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/common/CGenericManager.h
*  PURPOSE:     Generic manager template class
*  DEVELOPERS:  Oliver Brown <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

/*
CGenericManager notes by Oli:

 - When creating a new manager for managing an object, create a new manager which inherits from CGenericManager as follows
   I will use CPlayer's as an example

     class CPlayerManager : public CGenericManager < CPlayerManager, CPlayer >

     where CPlayer is the class which will be added to the managed list

 - When creating the new managed object, create the new managed object which inherit from CGeneric as follows
   I will use CPlayer's as an example

     class CPlayer : public CGeneric < CPlayerManager, CPlayer >

- Once this is done, to add a new element to the list in the manager you just need to create a new CManaged. ie:

     CPlayer* pPlayer = new CPlayer;

  This will automatically add pPlayer to the managers list.

- IMPORTANT: The AddToList function in YOUR manager class will be called BEFORE the CManaged constructor is executed,
             so don't try to do anything with CManaged* in that function, or your program will crash.

- The CManager's destructor is called before the list is cleared, so there is little point doing anything with
  the list here as it is still full of CManaged objects.

- The CManager's list functions will be called, which will then link to the CGenericManagers functions.
  If you wish to overload these functions then you must add a call to the CGenericManager's function as follows:
  I will use CPlayer's as an example

    CPlayerManager::AddToList ( CPlayer* pPlayer )
    {
        CGenericManager < CPlayerManager, CPlayer > ::AddToList ( pPlayer );
    }

- You can iterate the list in your CManager class like you would any other time, ie:
  I will use CPlayer's as an example

    CPlayerManager::Get ( const char* szNick )
    {
        list < CPlayer* > ::iterator iter = m_List.begin ();
        for ( ; iter != m_List.end (); iter++ )
        {
            if ( strcmp ( szNick, (*iter)->GetNickPointer () ) == 0 )
            {
                return *iter;
            }
        }
        return NULL;
    }

  This function would work fine with the list in CGenericManager.

- Finally, i'd recommend just doing this by the book, inheriting these two classes and then just creating your new CManaged.
  This will add it to the list fine and dandy, then after the new statement, do whatever you want with it.
*/

#ifndef __CGENERICMANAGER_H
#define __CGENERICMANAGER_H

#include <stdio.h>
#include <list>

template < typename TManager, typename TManaged > class CGenericManager : protected std::list < TManaged* >
{
public:
    explicit CGenericManager ( void )
    {
        m_pParent = static_cast < TManager* > ( this );
        m_bCanRemoveFromList = true;
    }

    virtual ~CGenericManager ( void )
    {
        m_pParent->RemoveAll ();
    }

    virtual void AddToList ( TManaged* pManaged )
    {
        push_back ( pManaged );
    }

    virtual void RemoveFromList ( TManaged* pManaged )
    {
        if ( m_bCanRemoveFromList )
        {
            remove ( pManaged );
        }
    }

    virtual void RemoveAll ( void )
    {
		typename std::list< TManaged* >::const_iterator iter;
		m_bCanRemoveFromList = false;

        for ( iter = this->begin (); iter != this->end (); iter++ )
        {
            delete *iter;
        }
        m_bCanRemoveFromList = true;

        this->clear ();
    }

    inline TManager* GetParent ( void )
    {
        return m_pParent;
    }

protected:
    TManager*               m_pParent;
    bool                    m_bCanRemoveFromList;

};

#endif
