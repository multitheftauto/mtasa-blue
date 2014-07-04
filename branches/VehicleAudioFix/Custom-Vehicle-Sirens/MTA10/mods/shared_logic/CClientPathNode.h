/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPathNode.h
*  PURPOSE:     Path node entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

class CClientPathNode;

#ifndef __CCLIENTPATHNODE_H
#define __CCLIENTPATHNODE_H

#include "CClientManager.h"

class CClientPathNode : public CClientEntity
{
    DECLARE_CLASS( CClientPathNode, CClientEntity )
    friend class CClientPathManager;

public:
    enum ePathNodeStyle
    {
        PATH_STYLE_JUMP = 0,
        PATH_STYLE_SMOOTH
    };
                            CClientPathNode             ( CClientManager* pManager,
                                                          CVector& vecPosition,
                                                          CVector& vecRotation,
                                                          int iTime,
                                                          ElementID ID,
                                                          ePathNodeStyle PathStyle = PATH_STYLE_JUMP,
                                                          CClientPathNode* pNextNode = NULL );
                            ~CClientPathNode            ( void );

    void                    Unlink                      ( void );

    void                    DoPulse                     ( void );

    eClientEntityType       GetType                     ( void ) const                  { return CCLIENTPATHNODE; }
    
    inline void             GetPosition                 ( CVector& vecPosition ) const  { vecPosition = m_vecPosition; }
    inline void             SetPosition                 ( const CVector& vecPosition )  { m_vecPosition = vecPosition; }
    inline void             GetRotation                 ( CVector& vecRotation )        { vecRotation = m_vecRotation; }
    inline void             SetRotation                 ( const CVector& vecRotation )  { m_vecRotation = vecRotation; }
    inline int              GetTime                     ( void )                        { return m_iTime; }
    inline ePathNodeStyle   GetStyle                    ( void )                        { return m_Style; }
    inline CClientPathNode* GetPreviousNode             ( void )                        { return m_pPreviousNode; }
    inline void             SetPreviousNode             ( CClientPathNode* pPathNode )  { m_pPreviousNode = pPathNode; }
    inline CClientPathNode* GetNextNode                 ( void )                        { return m_pNextNode; }
    inline void             SetNextNode                 ( CClientPathNode* pPathNode )  { m_pNextNode = pPathNode; }
    inline void             SetNextNodeID               ( ElementID ID )                { m_NextNodeID = ID; }
    
    inline void             AttachEntity                ( CClientEntity* pEntity )      { m_List.push_back ( pEntity ); }
    inline void             DetachEntity                ( CClientEntity* pEntity )      { if ( !m_List.empty() ) m_List.remove ( pEntity ); }
    bool                    IsEntityAttached            ( CClientEntity* pEntity );

    inline std::list < CClientEntity* > ::iterator AttachedIterBegin ( void )                        { return m_List.begin (); }
    inline std::list < CClientEntity* > ::iterator AttachedIterEnd   ( void )                        { return m_List.end (); }
 
private:
    CClientPathManager*         m_pPathManager;

    CVector                     m_vecPosition;
    CVector                     m_vecRotation;
    int                         m_iTime;
    ePathNodeStyle              m_Style;
    ElementID                   m_NextNodeID;
    CClientPathNode             *m_pPreviousNode, *m_pNextNode;

    std::list < CClientEntity* > m_List;
};

#endif