/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CFoo.cpp
*  PURPOSE:     Debugging class (not used in Release mode)
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#ifdef MTA_DEBUG

bool bFoo_PlayerLimitCrash = false;

void CFoo::DoPulse ( void )
{
    CClientManager* pManager = g_pClientGame->GetManager ();
    if ( pManager->IsGameLoaded () )
    {
        CClientPlayerManager* pPlayerManager = pManager->GetPlayerManager ();
        CClientPlayer* pLocal = pManager->GetPlayerManager ()->GetLocalPlayer ();
        if ( !pLocal ) return;
        CVector vecLocal;
        pLocal->GetPosition ( vecLocal );
        CClientCamera* pCamera = pManager->GetCamera ();


        /*
        CClientVehicle* pVehicle = pLocal->GetOccupiedVehicle ();
        if ( pVehicle )
        {
            unsigned char ucGear = pVehicle->GetGameVehicle ()->GetCurrentGear ();
            g_pCore->GetGraphics ()->DrawText ( 200, 200, 0xFFFFFFFF, 1.0f, "Gear = %u", ucGear );
        }
        */


        // ChrML: Reproduces issue #2741
        if ( bFoo_PlayerLimitCrash )
        {
            static CClientPed* pPed = NULL;
            if ( !pPed )
            {
                pPed = new CClientPlayer ( pManager, 50 );

                CVector vecLocal;
                pPlayerManager->GetLocalPlayer ()->GetPosition ( vecLocal );
                vecLocal.fX += 10;
                pPed->SetPosition ( vecLocal );
            }

            static unsigned long ulTestTime = 0;
            if ( ulTestTime == 0 )
            {
                ulTestTime = CClientTime::GetTime () + 50;
            }

            if ( CClientTime::GetTime () > ulTestTime )
            {
                ulTestTime = CClientTime::GetTime () + 50;

                static unsigned int uiIndex = 0;
                ++uiIndex;

                pPed->SetModel ( uiIndex );

                if ( uiIndex > 290 )
                    uiIndex = 0;
            }
        }
    }
}

class CAnim
{
    
};


class CAnimGroup
{
public:
    char            szGroupName [16];
    char            szSomething [16];
    unsigned long   ulUnknown;          // 0x07 or 0xFFFFFFFF on the first ones
    unsigned long   ulAnimCount;        // ??
    const char**    pAnimNames;
    void*           pSomeArray;
};

CAnimGroup* pGroups = (CAnimGroup*) 0x008AA5A8;



void CFoo::Test ( const char* szString )
{
    CClientManager* pManager = g_pClientGame->GetManager ();
    CClientPlayer* pLocal = pManager->GetPlayerManager ()->GetLocalPlayer ();
    CClientVehicleManager* pVehicleManager = pManager->GetVehicleManager ();
    CVector vecLocal;
    pLocal->GetPosition ( vecLocal );
    CClientCamera* pCamera = pManager->GetCamera ();



    // ChrML: Trying to reproduce mantis issue #2760
    if ( stricmp ( szString, "2760" ) == 0 )
    {
        vecLocal = CVector ( 0.0f, 0.0f, 5.0f );

        for ( int i = 0; i < 20; i++ )
        {
            vecLocal.fX += 5.0f;
            CClientPlayer* pPlayer = new CClientPlayer ( pManager, i + 50 );
            pPlayer->SetDeadOnNetwork ( false );
            pPlayer->SetModel ( 168 + i );
            pPlayer->AttachTo ( NULL );
            pPlayer->SetFrozen ( false );
            pPlayer->RemoveAllWeapons ();
            pPlayer->Teleport ( vecLocal );
            pPlayer->SetCameraRotation ( 0 );
            pPlayer->ResetInterpolation ();
            pPlayer->SetMoveSpeed ( CVector () );
            pPlayer->SetHealth ( 100.0f );
            pPlayer->SetArmor ( 0 );
            pPlayer->SetCurrentRotation ( 0 );
            pPlayer->SetInterior ( 0 );
            pPlayer->SetDimension ( 0 );
        }

        pLocal->SetDeadOnNetwork ( false );
        pLocal->SetModel ( 145 );
        pLocal->AttachTo ( NULL );
        pLocal->SetFrozen ( false );
        pLocal->RemoveAllWeapons ();
        pLocal->Teleport ( vecLocal );
        pLocal->SetCameraRotation ( 0 );
        pLocal->ResetInterpolation ();
        pLocal->SetMoveSpeed ( CVector () );
        pLocal->SetHealth ( 100.0f );
        pLocal->SetArmor ( 0 );
        pLocal->SetCurrentRotation ( 0 );
        pLocal->SetInterior ( 0 );
        pLocal->SetDimension ( 0 );
        g_pClientGame->SetAllDimensions ( 0 );

        // Reset return position so we can't warp back to where we were if local player
        g_pClientGame->GetNetAPI ()->ResetReturnPosition ();

        // Make sure the camera is normal
        pCamera->SetFocusToLocalPlayer ();
        pCamera->FadeIn ( 0.0f );
    }
    




    // Player load crash
    else if ( stricmp ( szString, "2741" ) == 0 )
    {
        bFoo_PlayerLimitCrash = true;
    }


    // 
    else if ( strnicmp ( szString, "interp", 6 ) == 0 )
    {
        if ( pVehicleManager->Count () > 0 )
        {
            CClientVehicle* pVehicle = *pVehicleManager->IterBegin ();

            float fdelta = atof ( szString + 7 );

            CVector vecT;
            pVehicle->GetPosition ( vecT );
            vecT.fZ = fdelta;
            pVehicle->SetTargetPosition ( vecT, TICK_RATE );

            g_pCore->ChatPrintf ( "Done %f", false, fdelta );

            static_cast < CDeathmatchVehicle* > ( pVehicle )->SetIsSyncing ( false );
        }
    }


    // 
    else if ( strnicmp ( szString, "interr", 6 ) == 0 )
    {
        if ( pVehicleManager->Count () > 0 )
        {
            CClientVehicle* pVehicle = *pVehicleManager->IterBegin ();

            CVector vecT;
            pVehicle->GetRotationDegrees ( vecT );
            vecT.fZ = atof ( szString + 7 );
            pVehicle->SetTargetRotation ( vecT, TICK_RATE );

            g_pCore->ChatPrintf ( "Done %f", false, atof ( szString + 7 ) );

            static_cast < CDeathmatchVehicle* > ( pVehicle )->SetIsSyncing ( false );
        }
    }


    else if ( stricmp ( szString, "choke" ) == 0 )
    {
        g_pClientGame->GetLocalPlayer ()->SetChoking ( true );
    }


    // 
    else if ( strnicmp ( szString, "static", 6 ) == 0 )
    {
        if ( pVehicleManager->Count () > 0 )
        {
            CClientVehicle* pVehicle = *pVehicleManager->IterBegin ();

            pVehicle->GetGameVehicle ()->SetRemap ( atoi ( szString + 7 ) );
            g_pCore->ChatPrintf ( "Set %i", false, atoi ( szString + 7 ) );
        }
    }


    // 
    else if ( strnicmp ( szString, "getmass", 7 ) == 0 )
    {
        CClientVehicle* pVehicle = pLocal->GetOccupiedVehicle ();
        if ( pVehicle )
        {
            g_pCore->ChatPrintf ( "Mass == %f", false, pVehicle->GetGameVehicle ()->GetMass () );
        }
    }

    else if ( strnicmp ( szString, "setmass", 7 ) == 0 )
    {
        CClientVehicle* pVehicle = pLocal->GetOccupiedVehicle ();
        if ( pVehicle )
        {
            pVehicle->GetGameVehicle ()->SetMass ( atof ( szString + 8 ) );
            g_pCore->ChatPrintf ( "Set mass to %f", false, pVehicle->GetGameVehicle ()->GetMass () );
        }
    }


    // 
    /*
    else if ( strnicmp ( szString, "setmm", 5 ) == 0 )
    {
        CClientVehicle* pVehicle = pLocal->GetOccupiedVehicle ();
        if ( pVehicle )
        {
            float fVal = atof ( szString + 6);
            szString += 4;
            float* fMass = (float*) atoi ( szString + 6 );
            *fMass = fVal;
            g_pCore->ChatPrintf ( "Set %X to %f", false, fMass, fVal );
        }
    }
    */

    /*
    else if ( stricmp ( szString, "getmm" ) == 0 )
    {
        CClientVehicle* pVehicle = pLocal->GetOccupiedVehicle ();
        if ( pVehicle )
        {
            float* fMass = (float*) atoi ( szString );
            g_pCore->ChatPrintf ( "Get %f", false, *fMass );
        }
    }
    */



    /*
    else if ( stricmp ( szString, "dump" ) == 0 )
    {
        FILE* poo = fopen ( "vehs.txt", "w+" );
        if ( poo )
        {
            tHandlingData* pHandling = (tHandlingData*) 0xC2B9E0;
            unsigned int uiIndex = 0;
            for ( ; uiIndex < 219; uiIndex++ )
            {
                fprintf ( poo, "\n\n\n\n####### VEHICLE ID %u #######\n", uiIndex );

                fprintf ( poo, "fMass = %f\n", pHandling->fMass );
                fprintf ( poo, "fUnknown1 = %f\n", pHandling->fUnknown1 );
                fprintf ( poo, "fTurnMass = %f\n", pHandling->fTurnMass );

                fprintf ( poo, "fDragCoeff = %f\n", pHandling->fDragCoeff );
                fprintf ( poo, "vecCenterOfMass = %f, %f, %f\n", pHandling->vecCenterOfMass.fX, pHandling->vecCenterOfMass.fY, pHandling->vecCenterOfMass.fZ );
                fprintf ( poo, "uiPercentSubmerged = %u\n", pHandling->uiPercentSubmerged );

                fprintf ( poo, "fUnknown2 = %f\n", pHandling->fUnknown2 );

                fprintf ( poo, "fTractionMultiplier = %f\n", pHandling->fTractionMultiplier );

                fprintf ( poo, "Transmission.fUnknown [0] = %f\n", pHandling->Transmission.fUnknown [0] );
                fprintf ( poo, "Transmission.fUnknown [1] = %f\n", pHandling->Transmission.fUnknown [1] );
                fprintf ( poo, "Transmission.fUnknown [2] = %f\n", pHandling->Transmission.fUnknown [2] );
                fprintf ( poo, "Transmission.fUnknown [3] = %f\n", pHandling->Transmission.fUnknown [3] );
                fprintf ( poo, "Transmission.fUnknown [4] = %f\n", pHandling->Transmission.fUnknown [4] );
                fprintf ( poo, "Transmission.fUnknown [5] = %f\n", pHandling->Transmission.fUnknown [5] );
                fprintf ( poo, "Transmission.fUnknown [6] = %f\n", pHandling->Transmission.fUnknown [6] );
                fprintf ( poo, "Transmission.fUnknown [7] = %f\n", pHandling->Transmission.fUnknown [7] );
                fprintf ( poo, "Transmission.fUnknown [8] = %f\n", pHandling->Transmission.fUnknown [8] );
                fprintf ( poo, "Transmission.fUnknown [9] = %f\n", pHandling->Transmission.fUnknown [9] );
                fprintf ( poo, "Transmission.fUnknown [10] = %f\n", pHandling->Transmission.fUnknown [10] );
                fprintf ( poo, "Transmission.fUnknown [11] = %f\n", pHandling->Transmission.fUnknown [11] );
                fprintf ( poo, "Transmission.fUnknown [12] = %f\n", pHandling->Transmission.fUnknown [12] );
                fprintf ( poo, "Transmission.fUnknown [13] = %f\n", pHandling->Transmission.fUnknown [13] );
                fprintf ( poo, "Transmission.fUnknown [14] = %f\n", pHandling->Transmission.fUnknown [14] );
                fprintf ( poo, "Transmission.fUnknown [15] = %f\n", pHandling->Transmission.fUnknown [15] );
                fprintf ( poo, "Transmission.fUnknown [16] = %f\n", pHandling->Transmission.fUnknown [16] );
                fprintf ( poo, "Transmission.fUnknown [17] = %f\n", pHandling->Transmission.fUnknown [17]  );

                fprintf ( poo, "Transmission.ucDriveType = %c\n", pHandling->Transmission.ucDriveType );
                fprintf ( poo, "Transmission.ucEngineType = %c\n", pHandling->Transmission.ucEngineType );
                fprintf ( poo, "Transmission.ucNumberOfGears = %u\n", pHandling->Transmission.ucNumberOfGears );
                fprintf ( poo, "Transmission.ucUnknown = %u\n", pHandling->Transmission.ucUnknown );

                fprintf ( poo, "Transmission.uiHandlingFlags = 0x%X\n", pHandling->Transmission.uiHandlingFlags );

                fprintf ( poo, "Transmission.fEngineAcceleration = %f\n", pHandling->Transmission.fEngineAcceleration );
                fprintf ( poo, "Transmission.fEngineInertia = %f\n", pHandling->Transmission.fEngineInertia );
                fprintf ( poo, "Transmission.fMaxVelocity = %f\n", pHandling->Transmission.fMaxVelocity );

                fprintf ( poo, "Transmission.fUnknown2 [0] = %f\n", pHandling->Transmission.fUnknown2 [0]  );
                fprintf ( poo, "Transmission.fUnknown2 [1] = %f\n", pHandling->Transmission.fUnknown2 [1]  );
                fprintf ( poo, "Transmission.fUnknown2 [2] = %f\n", pHandling->Transmission.fUnknown2 [2]  );

                fprintf ( poo, "fBrakeDeceleration = %f\n", pHandling->fBrakeDeceleration );
                fprintf ( poo, "fBrakeBias = %f\n", pHandling->fBrakeBias );
                fprintf ( poo, "bABS = %u\n", pHandling->bABS );

                fprintf ( poo, "fSteeringLock = %f\n", pHandling->fSteeringLock );
                fprintf ( poo, "fTractionLoss = %f\n", pHandling->fTractionLoss );
                fprintf ( poo, "fTractionBias = %f\n", pHandling->fTractionBias );

                fprintf ( poo, "fSuspensionForceLevel = %f\n", pHandling->fSuspensionForceLevel );
                fprintf ( poo, "fSuspensionDamping = %f\n", pHandling->fSuspensionDamping );
                fprintf ( poo, "fSuspensionHighSpdDamping = %f\n", pHandling->fSuspensionHighSpdDamping );
                fprintf ( poo, "fSuspensionUpperLimit = %f\n", pHandling->fSuspensionUpperLimit );
                fprintf ( poo, "fSuspensionLowerLimit = %f\n", pHandling->fSuspensionLowerLimit );
                fprintf ( poo, "fSuspensionFrontRearBias = %f\n", pHandling->fSuspensionFrontRearBias );
                fprintf ( poo, "fSuspensionAntiDiveMultiplier = %f\n", pHandling->fSuspensionAntiDiveMultiplier );

                fprintf ( poo, "fCollisionDamageMultiplier = %f\n", pHandling->fCollisionDamageMultiplier );

                fprintf ( poo, "uiModelFlags = %X\n", pHandling->uiModelFlags );
                fprintf ( poo, "uiHandlingFlags = %X\n", pHandling->uiHandlingFlags );
                fprintf ( poo, "fSeatOffsetDistance = %f\n", pHandling->fSeatOffsetDistance );
                fprintf ( poo, "uiMonetary = %u\n", pHandling->uiMonetary );

                fprintf ( poo, "ucHeadLight = 0x%X\n", pHandling->ucHeadLight );
                fprintf ( poo, "ucTailLight = 0x%X\n", pHandling->ucTailLight );
                fprintf ( poo, "ucAnimGroup = 0x%X\n", pHandling->ucAnimGroup );
                fprintf ( poo, "ucUnused = 0x%X\n", pHandling->ucUnused );

                fprintf ( poo, "iUnknown7 = %f, %X\n", pHandling->iUnknown7, pHandling->iUnknown7 );

                pHandling += 1;
            }


            g_pCore->ChatPrintf ( "Dumped", false );
            fclose ( poo );
        }
    }
    */



    else if ( strnicmp ( szString, "moveug", 6 ) == 0 )
    {
        if ( pVehicleManager->Count () > 0 )
        {
            CClientVehicle* pVehicle = *pVehicleManager->IterBegin ();

            /*
            CClientPed* pModel = pVehicle->GetOccupant ( 0 );
            if ( !pModel )
            {
                CClientPlayer* pPlayer = new CClientPlayer ( g_pClientGame->GetManager (), 50 );
                pModel = pPlayer->LoadModel ( 0 );
                pModel->WarpIntoVehicle ( pVehicle );
            }
            */

            

            pVehicle->RemoveTargetPosition ();
            pVehicle->RemoveTargetRotation ();


            CVector vecT;
            pVehicle->GetPosition ( vecT );
            vecT.fZ = atof ( szString + 7 );
            pVehicle->SetPosition ( vecT );

            g_pCore->ChatPrintf ( "Done", false );
        }
    }

    else if ( strnicmp ( szString, "nocol", 5 ) == 0 )
    {
        if ( pVehicleManager->Count () > 0 )
        {
            CClientVehicle* pVehicle = *pVehicleManager->IterBegin ();
            pVehicle->SetCollisionEnabled ( false );

            g_pCore->ChatPrintf ( "Done", false );
        }
    }

    else if ( stricmp ( szString, "resetdamage" ) == 0 )
    {
        g_pClientGame->GetPlayerManager ()->GetLocalPlayer ()->GetGamePlayer ()->ResetLastDamage ();
    }


    else if ( strnicmp ( szString, "fuckveh", 7 ) == 0 )
    {
        CClientVehicle* pVehicle = pLocal->GetOccupiedVehicle ();
        if ( pVehicle )
        {
            pVehicle->SetTargetPosition ( CVector ( 0, 0, 0 ), TICK_RATE );
            pVehicle->SetTargetRotation ( CVector ( 0, 0, 0 ), TICK_RATE );

            g_pCore->ChatPrintf ( "Done", false );
        }
    }

    else if ( stricmp ( szString, "ped" ) == 0 )
    {
        CClientPed* pPed = new CClientPed ( g_pClientGame->GetManager (), INVALID_ELEMENT_ID, 9 );
        vecLocal.fX += 5.0f;
        pPed->SetPosition ( vecLocal );
    }

    else if ( strnicmp ( szString, "callit", 6 ) == 0 )
    {
        FILE* pFile = fopen ( "C:/dump.txt", "w+" );

        for ( int i = 0; i < 400; i++ )
        {
            int iIndex = i;
            const char* szName = NULL;
            _asm
            {
                mov     eax, 0x4D3A30
                push    iIndex
                call    eax
                mov     szName, eax
                add     esp, 4
            }

            fprintf ( pFile, "%i: %s\n", iIndex, szName );
        }

        fclose ( pFile );
    }


    else if ( strnicmp ( szString, "veh", 3 ) == 0 )
    {
        int i = 600;
        FILE* p = fopen ( "C:/dump.txt", "w+" );

        for ( int a = 0; a < 13; a++ )
        {
            g_pGame->GetModelInfo ( i )->AddRef ( true );

            CVehicle* pVehicle = g_pGame->GetPools ()->AddVehicle ( (eVehicleTypes)i );
            DWORD* dw2 = (DWORD*)(((DWORD)pVehicle->GetVehicleInterface ()) + 0xE1 * 4 );
            DWORD dw = *dw2;
            dw = dw + 4;
            dw = dw - 0xC2B9E0;
            dw = dw / 224;
            fprintf ( p, "Array [%u] = %u;\n", i, dw );

            g_pGame->GetPools ()->RemoveVehicle ( pVehicle );

            fflush ( p );

            g_pGame->GetModelInfo ( i )->RemoveRef ( );

            i++;
        }

        fclose ( p );
    }


    else if ( strnicmp ( szString, "groups", 6 ) == 0 )
    {
        FILE* pFile = fopen ( "C:/dump.txt", "w+" );


        int i = 0;
        for ( ; i < 139; i++ )
        {
            fprintf ( pFile, "==%s [%s] (%i)==\n", pGroups [i].szGroupName, pGroups [i].szSomething, i );
            int i2 = 0;
            for ( ; i2 < pGroups [i].ulAnimCount; i2++ )
            {
                const char* szAnimName = pGroups [i].pAnimNames [i2];
                if ( szAnimName [0] )
                {
                    fprintf ( pFile, "''%i'': %s<br>\n", i2, szAnimName );
                }
            }

            fprintf ( pFile, "\n" );
        }

        fclose ( pFile );
    }


    else if ( strnicmp ( szString, "getshot", 7 ) == 0 )
    {
        if ( pLocal->GetGamePlayer ()->GetCanBeShotInVehicle () )
        {
            g_pCore->ChatEcho ( "true" );
        }
        else
        {
            g_pCore->ChatEcho ( "false" );
        }
    }

    else if ( strnicmp ( szString, "gettest", 7 ) == 0 )
    {
        if ( pLocal->GetGamePlayer ()->GetTestForShotInVehicle () )
        {
            g_pCore->ChatEcho ( "true" );
        }
        else
        {
            g_pCore->ChatEcho ( "false" );
        }
    }


    else if ( strnicmp ( szString, "setshot", 7 ) == 0 )
    {
        pLocal->GetGamePlayer ()->SetCanBeShotInVehicle ( true );
    }


    else if ( strnicmp ( szString, "settest", 8 ) == 0 )
    {
        pLocal->GetGamePlayer ()->SetTestForShotInVehicle ( true );
    }


    else if ( stricmp ( szString, "applytest" ) == 0 )
    {
        DWORD oldProt, oldProt2;
        VirtualProtect((LPVOID)0x5E8FFB,6,PAGE_EXECUTE_READWRITE,&oldProt);     

        *(unsigned char*) (0x5E8FFB ) = 0x90;
        *(unsigned char*) (0x5E8FFC ) = 0x90;
        *(unsigned char*) (0x5E8FFD ) = 0x90;
        *(unsigned char*) (0x5E8FFE ) = 0x90;
        *(unsigned char*) (0x5E8FFF ) = 0x90;
        *(unsigned char*) (0x5E9000 ) = 0x90;

        VirtualProtect((LPVOID)0x5E8FFB,6,oldProt,&oldProt2); 
    }
}

#endif
