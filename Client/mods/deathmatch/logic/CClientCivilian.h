/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientCivilian.h
 *  PURPOSE:     Civilian ped entity class
 *
 *****************************************************************************/

class CClientCivilian;

#pragma once

#include <game/CCivilianPed.h>
#include <game/TaskGoTo.h>
#include <game/TaskCarAccessories.h>

#include "CClientCommon.h"
#include "CClientEntity.h"

class CClientCivilian final : public CClientEntity
{
    DECLARE_CLASS(CClientCivilian, CClientEntity)
    friend class CClientCivilianManager;

public:
    ~CClientCivilian();

    void Unlink();

    eClientEntityType GetType() const { return CCLIENTCIVILIAN; };

    CCivilianPed* GetGameCivilian() { return m_pCivilianPed; }

    void GetPosition(CVector& vecPosition) const { vecPosition = *m_pCivilianPed->GetPosition(); };
    void SetPosition(const CVector& vecPosition) { m_pCivilianPed->SetPosition(const_cast<CVector*>(&vecPosition)); };
    void GetRotationRadians(CVector& vecRotation) const;
    void SetRotationRadians(const CVector& vecRotation);
    int  GetRotation();
    void SetRotation(int iRotation);

    void ModelRequestCallback(unsigned short usModelID){};

    float GetDistanceFromCentreOfMassToBaseOfModel();

    bool GetMatrix(CMatrix& Matrix) const;
    bool SetMatrix(const CMatrix& Matrix);

    void GetMoveSpeed(CVector& vecMoveSpeed) const;
    void SetMoveSpeed(const CVector& vecMoveSpeed);
    void GetTurnSpeed(CVector& vecTurnSpeed) const;
    void SetTurnSpeed(const CVector& vecTurnSpeed);

    bool IsVisible();
    void SetVisible(bool bVisible);

    float GetHealth() const;
    void  SetHealth(float fHealth);

    int  GetModelID();
    void SetModelID(int iModelID);

private:
    CClientCivilian(class CClientManager* pManager, ElementID ID, int iPedModel);
    CClientCivilian(class CClientManager* pManager, ElementID ID, CCivilianPed* pCivilianPed);

    void Dump(FILE* pFile, bool bDumpDetails, unsigned int uiIndex);

    void Create();
    void Destroy();
    void ReCreate();

    class CClientCivilianManager* m_pCivilianManager;

    int m_iModel;

    CCivilianPed* m_pCivilianPed;
};
