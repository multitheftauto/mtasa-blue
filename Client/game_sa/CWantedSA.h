/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CWantedSA.h
 *  PURPOSE:     Header file for wanted level management
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include <game/Common.h>
#include <game/CWanted.h>

class CPedSAInterface;

#define FUNC_SetMaximumWantedLevel      0x561E70
#define FUNC_SetWantedLevel             0x562470
#define FUNC_SetWantedLevelNoDrop       0x562570

#define MAX_CRIMES_QD       16
#define MAX_COPS_PURSUIT    10

class CCrimeBeingQd
{
public:
    DWORD   CrimeType;            // was eCrimeType
    long    CrimeID;
    DWORD   TimeOfQing;
    CVector Coors;
    bool    bAlreadyReported;
    bool    bPoliceDontReallyCare;
};

class CWantedSAInterface
{
public:
    long  m_nWantedLevel;
    long  m_nWantedLevelBeforeParole;
    DWORD m_LastTimeWantedDecreased;
    DWORD m_LastTimeWantedLevelChanged;
    DWORD m_TimeOfParole;
    float m_fMultiplier;            // New crimes have their wanted level contribution multiplied by this
    BYTE  m_nCopsInPursuit;
    BYTE  m_nMaxCopsInPursuit;
    BYTE  m_nMaxCopCarsInPursuit;

    BYTE  m_nCopsBeatingSuspect;
    WORD  m_nChanceOnRoadBlock;                 // /127 for each new roadsegment
    BYTE  m_PoliceBackOff : 1;                  // If this is set the police will leave player alone (for cut-scenes)
    BYTE  m_PoliceBackOffGarage : 1;            // If this is set the police will leave player alone (for garages)
    BYTE  m_EverybodyBackOff : 1;               // If this is set then everybody (including police) will leave the player alone (for cut-scenes)
    BYTE  m_swatRequired : 1;                   // These three booleans are needed so that the
    BYTE  m_fbiRequired : 1;                    // streaming required vehicle stuff can be overrided
    BYTE  m_armyRequired : 1;
    DWORD current_chase_time;
    DWORD current_chase_time_counter;
    bool  m_bTimeCounting;

    eWantedLevelSA m_WantedLevel;
    eWantedLevelSA m_WantedLevelBeforeParole;

    CCrimeBeingQd CrimesBeingQd[MAX_CRIMES_QD];

    CPedSAInterface* m_pCopsInPursuit[MAX_COPS_PURSUIT];            // was CCopPed

    DWORD m_PoliceScannerAudioEntity;            // was CAEPoliceScannerAudioEntity
};

class CWantedSA : public CWanted
{
private:
    CWantedSAInterface* internalInterface;
    bool                m_bDontDelete;

public:
    CWantedSA();
    CWantedSA(CWantedSAInterface* wantedInterface);
    ~CWantedSA();

    // Internal game_sa accessors
    CWantedSAInterface* GetInterface() { return this->internalInterface; }
    bool                GetDontDelete() { return m_bDontDelete; };
    void                SetDontDelete(bool bDontDelete) { m_bDontDelete = bDontDelete; };

    // Exported methods
    void SetWantedLevel(DWORD dwWantedLevel);
    void SetWantedLevelNoFlash(DWORD dwWantedLevel);
    char GetWantedLevel() { return this->internalInterface->m_WantedLevel; };
};
