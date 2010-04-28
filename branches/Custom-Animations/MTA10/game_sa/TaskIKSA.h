/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/TaskIKSA.h
*  PURPOSE:     Inverse kinematics game tasks
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_TASKIK
#define __CGAMESA_TASKIK

#include <game/TaskIK.h>

#include "TaskSA.h"

typedef DWORD IKChain_c;

#define FUNC_CTaskSimpleIKChain__Constructor            0x6339C0
#define FUNC_CTaskSimpleIKLookAt__Constructor           0x633E00
#define FUNC_CTaskSimpleIKManager__Constructor          0x6337F0

#define FUNC_CTaskSimpleTriggerLookAt__Constructor      0x634440


// ##############################################################################
// ## Name:    CTaskSimpleIKChain                                    
// ## Purpose: Move the ped's bones (animation stuff)
// ##############################################################################

class CTaskSimpleIKChainSAInterface : public CTaskSimpleSAInterface
{
public:
    CPed*       m_pPed;
    int     m_time;
    int     m_blendTime;
    IKChain_c*  m_pIKChain;
    short       m_slotID;
    short       m_pivotBoneTag;
    short       m_effectorBoneTag;
    CVector     m_effectorVec;
    CEntity*    m_pEntity;
    int     m_offsetBoneTag;
    CVector     m_offsetPos;
    float       m_speed;
    unsigned char       m_nonNullEntity;
    // blending info
    float       m_blend;
    int     m_endTime;
    float       m_targetBlend;
    int     m_targetTime;
    int     m_isBlendingOut;
#ifndef FINAL
    char        m_idString[32];
#endif
};

class CTaskSimpleIKChainSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleIKChain
{
public:
                CTaskSimpleIKChainSA ( void ) {};
                CTaskSimpleIKChainSA ( char* idString,
                                        int effectorBoneTag,
                                        CVector effectorVec,
                                        int pivotBoneTag,
                                        CEntity* pEntity,
                                        int offsetBoneTag,
                                        CVector offsetPos,
                                        float speed,
                                        int time=99999999,
                                        int blendTime=1000 );
};


// ##############################################################################
// ## Name:    CTaskSimpleIKLookAt                                    
// ## Purpose: Make the ped look at something
// ##############################################################################

class CTaskSimpleIKLookAtSAInterface : public CTaskSimpleIKChainSAInterface
{
public:
    unsigned char    m_useTorso;
    char     m_priority;
};

class CTaskSimpleIKLookAtSA : public virtual CTaskSimpleIKChainSA, public virtual CTaskSimpleIKLookAt
{
public:
    CTaskSimpleIKLookAtSA ( void ) {};
    CTaskSimpleIKLookAtSA ( char* idString,
                            CEntity* pEntity,
                            int time,
                            int offsetBoneTag,
                            CVector offsetPos,
                            unsigned char useTorso=false,
                            float speed=0.25f,
                            int blendTime=1000,
                            int m_priority=3 );
};


// ##############################################################################
// ## Name:    CTaskSimpleIKManager                                    
// ## Purpose: Move the ped's bones (animation stuff)
// ##############################################################################

class CTaskSimpleIKManagerSAInterface : public CTaskSimpleSAInterface
{
public:    
    CTaskSimpleIKChainSAInterface * m_pIKChainTasks[NUM_IK_CHAIN_SLOTS];
    unsigned char                           m_aborting;
};

class CTaskManagementSystemSA;

class CTaskSimpleIKManagerSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleIKManager
{
private:
    CTaskManagementSystemSA *   m_pTaskManagementSystem;
public:
                                CTaskSimpleIKManagerSA      ( void );
                                //CTaskSimpleIKManagerSA    ( /*fill me*/ );

    int                       AddIKChainTask              ( CTaskSimpleIKChain * pIKChainTask, int slotID=-1 );
    void                        RemoveIKChainTask           ( int slotID );
    void                        BlendOut                    ( int slotID, int blendOutTime );
    unsigned char                       IsSlotEmpty                 ( int slotID );
    CTaskSimpleIKChain *        GetTaskAtSlot               ( int slotID );

    /*
    friend class CTaskCounter;
    CTaskSimpleIKManager();
    ~CTaskSimpleIKManager();

    virtual CTask* Clone() const;
    virtual int GetTaskType() const {return CTaskTypes::TASK_SIMPLE_IK_MANAGER;}
    virtual bool MakeAbortable(CPed* pPed, const int iPriority, const CEvent* pEvent);
    virtual bool ProcessPed(CPed* pPed);    
    int AddIKChainTask(CTaskSimpleIKChain* pIKChainTask, int slotID=-1);
    void RemoveIKChainTask(int slotID);
    void BlendOut(int slotID, int blendOutTime);

    unsigned char IsSlotEmpty(int slotID);
    CTaskSimpleIKChain* GetTaskAtSlot(int slotID);
    
private:*/
};


// ##############################################################################
// ## Name:    CTaskSimpleTriggerLookAt                                    
// ## Purpose: Wrapper class for CTaskSimpleIKLookAt
// ##############################################################################


class CTaskSimpleTriggerLookAtSAInterface : public CTaskSimpleSAInterface
{
public:
    CEntity*    m_pEntity;
    int     m_time;
    int     m_offsetBoneTag;
    RwV3d       m_offsetPos;
    unsigned char       m_useTorso;
    float       m_speed;
    int     m_blendTime;
    
    unsigned char       m_nonNullEntity;
    char        m_priority;
};

class CTaskSimpleTriggerLookAtSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleTriggerLookAt
{
public:
    CTaskSimpleTriggerLookAtSA ( void ) {};
    CTaskSimpleTriggerLookAtSA ( CEntity* pEntity,
                                 int time,
                                 int offsetBoneTag,
                                 CVector offsetPos,
                                 unsigned char useTorso=false,
                                 float speed=0.25f,
                                 int blendTime=1000,
                                 int priority=3 );
};

#endif
