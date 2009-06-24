/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CEventDamage.h
*  PURPOSE:		Event damage interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CEventDamage_H
#define __CEventDamage_H

enum eWeaponType;
enum ePedPieceTypes;
class CEntity;
typedef unsigned long AssocGroupId;
typedef unsigned long AnimationId;
class CEventDamageSAInterface;
class CPed;

class CEventDamage
{
public:
    virtual CEventDamageSAInterface *   GetInterface        ( void ) = 0;

    virtual void                        Destroy             ( void ) = 0;

    // Flag accessors
    virtual bool                        DidPedFallDown      ( void ) = 0;
    virtual bool                        WasStealthAttack    ( void ) = 0;

    virtual void                        MakePedFallDown     ( void ) = 0;

    virtual DWORD                       GetDamageTime       ( void ) = 0;
    virtual eWeaponType                 GetWeaponUsed       ( void ) = 0;
    virtual ePedPieceTypes              GetPedPieceType     ( void ) = 0;
    virtual char                        GetDirection        ( void ) = 0;
    
    virtual CEntity *                   GetInflictingEntity ( void ) = 0;

    virtual bool                        HasKilledPed        ( void ) = 0;
    virtual float                       GetDamageApplied    ( void ) = 0;
    virtual AssocGroupId                GetAnimGroup        ( void ) = 0;
    virtual AnimationId                 GetAnimId           ( void ) = 0;
    virtual void                        ComputeDeathAnim    ( CPed * pPed, bool bUnk ) = 0;
    virtual void                        ComputeDamageAnim   ( CPed * pPed, bool bUnk ) = 0;
};

#endif