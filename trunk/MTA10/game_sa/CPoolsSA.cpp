/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPoolsSA.cpp
*  PURPOSE:     Game entity pools
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPoolsSA::CPoolsSA()
{
	DEBUG_TRACE("CPoolsSA::CPoolsSA()");
	
    m_bGetVehicleEnabled = true;
	dwVehicleCount = 0;
	dwObjectCount = 0;
	dwPedCount=0;
	dwBuildingCount= 0;

	memset(&Vehicles,0,sizeof(CVehicle *) * MAX_VEHICLES);
	memset(&Objects,0,sizeof(CObject *) * MAX_OBJECTS);
	memset(&Peds,0,sizeof(CPed *) * MAX_PEDS);
	memset(&Buildings,0,sizeof(CBuilding *) * MAX_BUILDINGS);

    EntryInfoNodePool = new CEntryInfoNodePoolSA();
    PointerNodeDoubleLinkPool = new CPointerNodeDoubleLinkPoolSA();
    PointerNodeSingleLinkPool = new CPointerNodeSingleLinkPoolSA();
}

CPoolsSA::~CPoolsSA ( void )
{
    // Make sure we've deleted all vehicles, objects, peds and buildings
    DeleteAllVehicles ();
    DeleteAllPeds ();
    DeleteAllObjects ();
    DeleteAllBuildings ();

    if ( EntryInfoNodePool )
        delete EntryInfoNodePool;

    if ( PointerNodeDoubleLinkPool )
        delete PointerNodeDoubleLinkPool;

    if ( PointerNodeSingleLinkPool )
        delete PointerNodeSingleLinkPool;
}


void CPoolsSA::DeleteAllVehicles ( void )
{
    for ( int i = 0; i < MAX_VEHICLES; i++ )
    {
        if ( Vehicles [i] )
        {
            RemoveVehicle ( Vehicles [i] );
        }
    }
}


void CPoolsSA::DeleteAllPeds ( void )
{
    for ( int i = 0; i < MAX_PEDS; i++ )
    {
        if ( Peds [i] )
        {
            RemovePed ( Peds [i], false );
        }
    }
}


void CPoolsSA::DeleteAllObjects ( void )
{
    for ( int i = 0; i < MAX_OBJECTS; i++ )
    {
        if ( Objects [i] )
        {
            RemoveObject ( Objects [i] );
        }
    }
}


void CPoolsSA::DeleteAllBuildings ( void )
{
    /*
    for ( int i = 0; i < MAX_BUILDINGS; i++ )
    {
        if ( Buildings [i] )
        {
            RemoveBuilding ( Buildings [i] );
        }
    }
    */
}

/**
 * Returns a vehicle pointer for a specific vehicle ID
 * @param ID DWORD containing a vehicle ID
 * @return CVehicle * pointing to the relevant vehicle
*/
CVehicle * CPoolsSA::GetVehicle ( DWORD ID )
{
	DEBUG_TRACE("CVehicle * CPoolsSA::GetVehicle ( DWORD ID )");

	CVehicleSAInterface * VehicleInterface = (CVehicleSAInterface*)this->GetVehicleInterface(ID);
	if(VehicleInterface)
	{
		return GetVehicle ( VehicleInterface );
	}

	return NULL;	
}

/**
 * Returns a vehicle pointer for a specific vehicle ID
 * @param VehicleInterface CVehicleSAInterface * pointing to a vehicle
 * @return CVehicle * pointing to the relevant vehicle
 * \todo CVehicle* should be either DWORD * or CVehicleSAInterface * (see note on GetPed)
 */
CVehicle * CPoolsSA::GetVehicle ( CVehicleSAInterface * VehicleInterface )
{
	DEBUG_TRACE("CVehicle * CPoolsSA::GetVehicle ( CVehicleSAInterface * VehicleInterface )");

    assert ( VehicleInterface );

    if ( m_bGetVehicleEnabled )
    {
        //return (CVehicle*) VehicleInterface->m_TimeOfCreation;
		return VehicleInterface->m_pVehicle;
    }
    
    return NULL;
}

/** 
 * Returns a CVehicleSAInterface pointer for a specific vehicle ID
 * @param ID DWORD containing a vehicle ID
 * @return CVehicleSAInterface * pointing to the relevant vehicle
*/
CVehicle * CPoolsSA::GetVehicleInterface ( DWORD ID )
{
	DEBUG_TRACE("CVehicle * CPoolsSA::GetVehicleInterface ( DWORD ID )");
	DWORD dwReturn;
	DWORD dwFunction = FUNC_GetVehicle;
	_asm
	{
		mov		ecx, dword ptr ds:[CLASS_CPool_Vehicle]
		push	ID
		call	dwFunction
		add		esp, 4
		mov		dwReturn, eax
	}
	// dwReturn is a CVehicleSAInterface
	return (CVehicle*)dwReturn;
}

/**
 * Gets the id for a specific vehicle
 * @param vehicle CVehicle * pointing to the vehicle the id is required for
 * @return DWORD containing the vehicle's ID
 * \todo Update this description and add for some below - fix return removed for .net
 */
DWORD CPoolsSA::GetVehicleRef ( CVehicle * vehicle )
{
	DEBUG_TRACE("DWORD CPoolsSA::GetVehicleRef ( CVehicle * vehicle )");

	DWORD dwFunc = FUNC_GetVehicleRef;
	DWORD dwReturn = 0;

	CVehicleSA* pVehicleSA = dynamic_cast < CVehicleSA* > ( vehicle );
	if ( pVehicleSA )
	{
		CVehicleSAInterface * vehicleInterface = pVehicleSA->GetVehicleInterface();
		_asm
		{
			push	vehicleInterface
			call	dwFunc
			add		esp, 4
			mov		dwReturn, eax
		}
	}

	return dwReturn;
	//return vehicleVC->internalID;
}

/**
 * Gets the number of vehicles in the vehicle pool
 * @return DWORD containing the number of vehicles in the vehicle pool
 */
DWORD CPoolsSA::GetVehicleCount (  )
{
	DEBUG_TRACE("DWORD CPoolsSA::GetVehicleCount (  )");
	return dwVehicleCount;
}

CBuilding * CPoolsSA::AddBuilding ( DWORD dwModelID )
{
	DEBUG_TRACE("CBuilding * CPoolsSA::AddBuilding ( DWORD dwModelID )");
	if(dwBuildingCount <= MAX_BUILDINGS)
	{
		for(int i = 0;i<MAX_BUILDINGS;i++)
		{
			if(Buildings[i] == 0)
			{
                CBuildingSA * pBuilding = new CBuildingSA(dwModelID);
                Buildings[i] = pBuilding;
                pBuilding->SetArrayID ( i );
				dwBuildingCount++;

				return pBuilding;
			}
		}
	}
	return NULL;
}

CObject * CPoolsSA::AddObject ( DWORD ID )
{
	DEBUG_TRACE("CObject * CPoolsSA::AddObject ( DWORD ID )");
	if(dwObjectCount <= MAX_OBJECTS)
	{
		for(int i = 0;i<MAX_OBJECTS;i++)
		{
			if(Objects[i] == 0)
			{
                CObjectSA * pObject = new CObjectSA(ID);
                if ( pObject->GetInterface () == NULL )
                {
                    delete pObject;
                    return NULL;
                }

                Objects[i] = pObject;
                pObject->SetArrayID ( i );
                dwObjectCount++;
				
				return pObject;
			}
		}
	}
	return NULL;
}

VOID CPoolsSA::RemoveObject ( CObject * object )
{
	DEBUG_TRACE("VOID CPoolsSA::RemoveObject ( CObject * object )");
	//OutputDebugString("CPoolsSA::RemoveObject\n");
	if(object == 0) return;
	static BOOL IsDeletingObjectAlready = FALSE; // to prevent delete being called twice (delete playerped will call this function too)

	int i = object->GetArrayID ();
	if(Objects[i] && Objects[i] == object)
	{			
		if(!IsDeletingObjectAlready) 
		{
			IsDeletingObjectAlready = TRUE;
			delete object;

			if ( dwObjectCount > 0 )
					dwObjectCount--;
		}

		Objects[i] = 0;
	}
	IsDeletingObjectAlready = FALSE;
}

/**
 * Returns a object pointer for a specific object ID
 * @param ID DWORD containing a object ID
 * @return CObject * pointing to the relevant object
*/
CObject * CPoolsSA::GetObject ( DWORD ID )
{
	DEBUG_TRACE("CObject * CPoolsSA::GetObject ( DWORD ID )");
	return Objects[ID];
	/*DWORD dwReturn;
	DWORD dwFunction = FUNC_GetObject;
	_asm
	{
		mov		ecx, dword ptr ds:[CLASS_CPool_Object]
		push	ID
		call	dwFunction
		mov		dwReturn, eax
	}*/
	return NULL; //(CObject *)dwReturn;
}

CObject * CPoolsSA::GetObject ( CObjectSAInterface * objectInterface )
{
	DEBUG_TRACE("CObject * CPoolsSA::GetObject ( CObjectSAInterface * objectInterface )");
	for ( int  i = 0; i < MAX_OBJECTS; i++ )
	{
        if ( Objects[i] )
        {
		    if ( Objects[i]->GetInterface() == objectInterface )
			    return Objects[i];
        }
	}
	
	return NULL; //dwReturn;
}


/**
 * Gets the id for a specific object
 * @param object CObject * pointing to the object the id is required for
 * @return DWORD containing the object's ID
 */
DWORD CPoolsSA::GetObjectRef ( CObject * object )
{
	DEBUG_TRACE("DWORD CPoolsSA::GetObjectRef ( CObject * object )");
/*	DWORD dwReturn;
	DWORD dwFunction = FUNC_GetObjectRef;
	_asm
	{
		mov		ecx, dword ptr ds:[CLASS_CPool_Object]
		push	object
		call	dwFunction
		mov		dwReturn, eax
	}*/
	return NULL; //dwReturn;
}

/**
 * Gets the number of objects in the objects pool
 * @return DWORD containing the number of objects in the objects pool
 * \todo replace code
 */
DWORD CPoolsSA::GetObjectCount (  )
{
	DEBUG_TRACE("DWORD CPoolsSA::GetObjectCount (  )");
	return dwObjectCount;
/*	DWORD dwReturn;
	DWORD dwFunction = FUNC_GetObjectCount;
	_asm
	{
		call	dwFunction
		mov		dwReturn, eax
	}
	return dwReturn;*/
	
}

/**
 * Returns a CPed for a specific ped interface
 * @param ped CPedSAInterface for a ped you want to get the ped for
 * @return CPed * pointing to the relevant ped
 * \todo Needs fixing (use DWORD instead of CPedSAInterface?)
 */
CPed * CPoolsSA::GetPed ( DWORD * pedInterface )
{
	DEBUG_TRACE("CPed * CPoolsSA::GetPed ( DWORD * pedInterface )");

    // 0x00400000 is used for bad player pointers some places in GTA
	if( pedInterface && pedInterface != (DWORD*)0x00400000 )
	{
		for(int i = 0; i < MAX_PEDS; i++)
		{
			if ( Peds[i] && Peds[i]->GetInterface() == (CPedSAInterface *)pedInterface )
			{
				return Peds[i];
			}
		}
	}
	return NULL;	
}

/**
 * Returns a CPed for a specific ped ID
 * @param ID DWORD containing a ped ID
 * @return CPed * pointing to the relevant ped
*/
CPed * CPoolsSA::GetPed ( DWORD ID )
{
	DEBUG_TRACE("CPed * CPoolsSA::GetPed ( DWORD ID )");
	CPedSAInterface * PedInterface = this->GetPedInterface(ID);
	if(PedInterface)
	{
		for(int i = 0; i < MAX_PEDS; i++)
		{
			if(Peds[i] && Peds[i]->GetInterface() == PedInterface)
			{
				return Peds[i];
			}
		}
	}
	return NULL;	
}

/** 
 * Returns a CPedSAInterface pointer for a specific ped ID
 * @param ID DWORD containing a ped ID
 * @return CPedSAInterface * pointing to the relevant ped
*/
CPedSAInterface * CPoolsSA::GetPedInterface ( DWORD ID )
{
	DEBUG_TRACE("CPedSAInterface * CPoolsSA::GetPedInterface ( DWORD ID )");
	DWORD dwReturn;
	DWORD dwFunction = FUNC_GetPed;
	_asm
	{
		mov		ecx, dword ptr ds:[CLASS_CPool_Ped]
		push	ID
		call	dwFunction
		add		esp, 4
		mov		dwReturn, eax
	}
	// dwReturn is a CPedSAInterface
	return (CPedSAInterface *)dwReturn;
}


/**
 * Gets the id for a specific ped
 * @param ped CPed * pointing to the ped the id is required for
 * @return DWORD containing the ped's ID
 */
DWORD CPoolsSA::GetPedRef ( CPed * ped )
{
	DEBUG_TRACE("DWORD CPoolsSA::GetPedRef ( CPed * ped )");
	
	DWORD dwReturn = 0;
	DWORD dwFunction = FUNC_GetPedRef;
	
	CPedSA * pedvc =  dynamic_cast < CPedSA* > ( ped );
	if ( pedvc )
	{
		DWORD dwPedInterface = (DWORD)pedvc->GetInterface();
		_asm
		{
			mov		ecx, dword ptr ds:[CLASS_CPool_Ped]
			push	dwPedInterface
			call	dwFunction
			mov		dwReturn, eax
		}
	}

	return dwReturn;
}

/**
 * Gets the id for a specific ped
 * @param ped CPed * pointing to the ped the id is required for
 * @return DWORD containing the ped's ID
 */
DWORD CPoolsSA::GetPedRef ( DWORD internalInterface )
{
	DEBUG_TRACE("DWORD CPoolsSA::GetPedRef ( DWORD internalInterface )");
	DWORD dwReturn;
	DWORD dwFunction = FUNC_GetPedRef;
	_asm
	{
		mov		ecx, dword ptr ds:[CLASS_CPool_Ped]
		push	internalInterface
		call	dwFunction
		add		esp, 4
		mov		dwReturn, eax
	}
	return dwReturn;
}

/**
 * Gets the number of peds in the peds pool
 * @return DWORD containing the number of peds in the peds pool
 * \todo Replace code
 */
DWORD CPoolsSA::GetPedCount (  )
{
	DEBUG_TRACE("DWORD CPoolsSA::GetPedCount (  )");
	return dwPedCount;
}

CVehicle * CPoolsSA::AddTrain ( CVector * vecPosition, DWORD dwModels[], int iSize, int iDirection )
{
    // clean the existing array
    memset ( (void *)VAR_TrainModelArray, 0, 32 * sizeof(DWORD) );

    // now load the models we're going to use and add them to the array
    for ( int i = 0; i < iSize; i++ )
    {
        if ( dwModels[i] == 449 || dwModels[i] == 537 || 
            dwModels[i] == 538 || dwModels[i] == 569 || 
            dwModels[i] == 590 )
        {
            *(DWORD *)(VAR_TrainModelArray + i * 4) = dwModels[i];
        }
    }

    CVehicleSAInterface * trainBegining;
    CVehicleSAInterface * trainEnd;

    float fX = vecPosition->fX;
    float fY = vecPosition->fY;
    float fZ = vecPosition->fZ;

    // Disable GetVehicle because CreateMissionTrain calls it before our CVehicleSA instance is inited
    m_bGetVehicleEnabled = false;

    DWORD dwFunc = FUNC_CTrain_CreateMissionTrain;
    _asm
    {
        push    0 // place as close to point as possible (rather than at node)? (maybe) (actually seems to have an effect on the speed, so changed from 1 to 0)
        push    0 // start finding closest from here 
        push    -1 // node to start at (-1 for closest node)
        lea     ecx, trainEnd
        push    ecx // end of train
        lea     ecx, trainBegining 
        push    ecx // begining of train
        push    0 // train type (always use 0 as thats where we're writing to)
        push    iDirection // direction 
        push    fZ // z
        push    fY // y
        push    fX // x
        call	dwFunc
        add     esp, 0x28   
    }

    // Enable GetVehicle
    m_bGetVehicleEnabled = true;

    CVehicleSA * trainHead = NULL;
    if ( trainBegining )
    {
        DWORD vehicleIndex = 0;

		if(dwVehicleCount <= MAX_VEHICLES)
		{
			for(;vehicleIndex<MAX_VEHICLES;vehicleIndex++)
			{
				if ( Vehicles[vehicleIndex] == NULL )
				{
                    trainHead = new CVehicleSA ( trainBegining );
                    Vehicles [ vehicleIndex ] = trainHead;
                    trainHead->SetArrayID ( vehicleIndex );
                    dwVehicleCount++;
                    vehicleIndex++;
                    break;
                }
            }
        }

        CVehicleSA * carriage = trainHead;
        
        while ( carriage )
        {
		    if(dwVehicleCount <= MAX_VEHICLES)
		    {
			    for(;vehicleIndex<MAX_VEHICLES;vehicleIndex++)
			    {
				    if ( Vehicles[vehicleIndex] == NULL )
				    {
                        CVehicleSAInterface * vehCarriage = carriage->GetNextCarriageInTrain();
                        if ( vehCarriage )
                        {
                            carriage = new CVehicleSA ( vehCarriage );
                            Vehicles [ vehicleIndex ] = carriage;
                            dwVehicleCount++;
                            vehicleIndex++;
                        }
                        else
                            carriage = NULL;

                        break;
                    }
                }
            }
        }
    }

	// Stops the train from moving at ludacrist speeds right after creation
	// due to some glitch in the node finding in CreateMissionTrain
	trainHead->SetMoveSpeed ( &CVector ( 0, 0, 0 ) );

    return trainHead;
}

CVehicle * CPoolsSA::AddVehicle ( eVehicleTypes vehicleType )
{
	DEBUG_TRACE("CVehicle * CPoolsSA::AddVehicle ( eVehicleTypes vehicleType )");
//	if(pGame->GetModelInfo(vehicleType)->IsVehicle( ))
	{
//		char szDebug[255]= {'\0'};
//		sprintf(szDebug, "dwVehicleCount = %d", dwVehicleCount);
//		OutputDebugString(szDebug);
		if(dwVehicleCount <= MAX_VEHICLES)
		{
			for(int i = 0;i<MAX_VEHICLES;i++)
			{
				if ( Vehicles[i] == NULL )
				{
                    /*
                    CModelInfo* pModelInfo = pGame->GetModelInfo ( vehicleType );
					if ( pModelInfo->IsBike () )
					{
						// Bikes
						Vehicles [i] = (CVehicleSA*) new CBikeSA ( vehicleType );
						++dwVehicleCount;
						return Vehicles [i];	
					}
					else if ( pModelInfo->IsPlane () )
					{
						// Plane
						Vehicles [i] = (CVehicleSA*) new CPlaneSA ( vehicleType );	
						++dwVehicleCount;
						return Vehicles [i];	
					}
					else if ( pModelInfo->IsMonsterTruck () )
					{
						// Monster Truck
						Vehicles [i] = (CVehicleSA*) new CMonsterTruckSA ( vehicleType );
						++dwVehicleCount;
						return Vehicles [i];	
					}
					else if ( pModelInfo->IsQuadBike () )
					{
						// Quad Bike
						Vehicles [i] = (CVehicleSA*) new CQuadBikeSA ( vehicleType );
						++dwVehicleCount;
						return Vehicles [i];	
					}
					else if ( pModelInfo->IsBmx () )
					{
						// BMX
						Vehicles [i] = (CVehicleSA*) new CBmxSA ( vehicleType );
						++dwVehicleCount;
						return Vehicles [i];	
					}
					else if ( pModelInfo->IsTrailer () )
					{
						// Trailer
						Vehicles [i] = (CVehicleSA*) new CTrailerSA ( vehicleType );
						++dwVehicleCount;
						return Vehicles [i];	
					}
					else if ( pModelInfo->IsBoat () )
					{
						// Boats
						Vehicles [i] = (CVehicleSA*) new CBoatSA ( vehicleType );
						++dwVehicleCount;
						return Vehicles[i];	
					}
					else 
					{
						// Automobiles and Helis
						Vehicles[i] = (CVehicleSA*) new CAutomobileSA ( vehicleType );
						++dwVehicleCount;
						return Vehicles[i];					
					}
                    */

					Vehicles[i] = (CVehicleSA*) new CVehicleSA ( vehicleType );
                    Vehicles[i]->SetArrayID ( i );
					++dwVehicleCount;
					return Vehicles [i];
				}
			}
		}
	}

	return NULL;
}		

/**
 * Create a new ped and add it to the ped pool
 * @param pedType ePedModel containing a valid model id
 * @return CPlayerPedSA * for the ped
 */
CPed * CPoolsSA::AddPed ( ePedModel pedType )
{
	DEBUG_TRACE("CPed * CPoolsSA::AddPed ( ePedModel pedType )");
	CPlayerPedSA * playerped;

	if(dwPedCount <= MAX_PEDS)
	{
		for(int i = 0;i<MAX_PEDS;i++)
		{
			if(Peds[i] == 0)
			{
				// for now, we assume all peds are CPlayerPeds. See the scm create_actor for how we could check. No need really.
				playerped = new CPlayerPedSA(pedType);	
				Peds[i] = (CPedSA *)playerped;
                playerped->SetArrayID ( i );
				dwPedCount++;
				return Peds[i];				
				break;
			}
		}
	}
	return NULL;
}		

/**
 * Add an already existing ped to the ped pool (i.e. the player ped)
 * @param ped CPedSAInterface for the ped to add
 * @return CPlayerPedSA * for the ped
 */
CPed * CPoolsSA::AddPed ( CPedSAInterface * ped )
{
	DEBUG_TRACE("CPed * CPoolsSA::AddPed ( CPedSAInterface * ped )");
	char szTest[255] = { '\0'};
	CPlayerPedSA* playerped;
	int i = 0;

	if(dwPedCount <= MAX_PEDS)
	{
		for(i = 0;i<MAX_PEDS;i++)
		{
			if(Peds[i] && (CPedSAInterface *)Peds[i]->GetInterface() == ped)
				return NULL; // its already in the list
		}

		for(i = 0;i<MAX_PEDS;i++)
		{
			if(Peds[i] == 0)
			{
				// for now, we assume all peds are CPlayerPeds. See the scm create_actor for how we could check. No need really.
				playerped = new CPlayerPedSA ( static_cast < CPlayerPedSAInterface* > ( ped ) );	
				//Peds[i] = static_cast<CPedSA *>(playerped);
				Peds[i] = (CPedSA *)playerped;
                playerped->SetArrayID ( i );
				dwPedCount++;
				return Peds[i];				
				break;
			}
		}
	}
	return NULL;
}	

/**
 * Create a CPed for a civilian ped that already exists
 * @param ped CPedSAInterface for the ped to add
 * @return CPlayerPedSA * for the ped
 */
CPed * CPoolsSA::CreateCivilianPed ( DWORD * pedInterface )
{
	DEBUG_TRACE("CPed * CPoolsSA::CreateCivilianPed ( CPedSAInterface * ped )");
	int i = 0;
	CCivilianPed * pPed;
	if ( pedInterface )
	{
		if(dwPedCount <= MAX_PEDS)
		{
			for(i = 0;i<MAX_PEDS;i++)
			{
				if(Peds[i] && (CPedSAInterface *)Peds[i]->GetInterface() == (CPedSAInterface *)pedInterface)
					return Peds[i]; // its already in the list
			}

			for(i = 0;i<MAX_PEDS;i++)
			{
				if(Peds[i] == 0)
				{
					// for now, we assume all peds are CPlayerPeds. See the scm create_actor for how we could check. No need really.
					pPed = new CCivilianPedSA((CPedSAInterface *)pedInterface);	
					Peds[i] = dynamic_cast < CPedSA* > ( pPed );
                    Peds[i]->SetArrayID ( i );
					dwPedCount++;
					return Peds[i];				
					break;
				}
			}
		}
	}
	
	return NULL;
}		

VOID CPoolsSA::RemovePed ( CPed * ped, bool bDelete )
{
	DEBUG_TRACE("VOID CPoolsSA::RemovePed ( CPed * ped, bool bDelete )");
	if(ped == 0) return;
	static BOOL IsDeletingPedAlready = FALSE; // to prevent delete being called twice (delete playerped will call this function too)

    int i = ped->GetArrayID ();
	if(Peds[i] && Peds[i] == ped)
	{
		if(!IsDeletingPedAlready) 
		{
			IsDeletingPedAlready = TRUE;
			if ( Peds[i]->GetType() == PLAYER_PED )
			{
                CPlayerPedSA * playerped = dynamic_cast < CPlayerPedSA* > ( Peds[i] );
				if ( playerped )
				{
					if ( !bDelete )
						playerped->SetDoNotRemoveFromGameWhenDeleted ( true );

					delete playerped;
				}
			}
			else
			{
                CCivilianPedSA * civped = dynamic_cast < CCivilianPedSA* > ( Peds[i] );
				if ( civped )
				{
					if ( !bDelete )
						civped->SetDoNotRemoveFromGameWhenDeleted ( true );

					delete civped;
				}
			}

			if ( dwPedCount > 0 )
					dwPedCount--;
		}

		Peds[i] = 0;
	}
	IsDeletingPedAlready = FALSE;
}

VOID CPoolsSA::RemoveVehicle ( CVehicle * vehicle )
{
	DEBUG_TRACE("VOID CPoolsSA::RemoveVehicle ( CVehicle * vehicle )");
    assert ( vehicle );

	CVehicleSA * vehiclevc = dynamic_cast < CVehicleSA* > ( vehicle );
	if ( !vehiclevc ) return;

	//DWORD dwModelID = vehiclevc->GetModelIndex();

	static BOOL IsDeletingVehicleAlready = FALSE; // to prevent delete being called twice (delete vehiclevc will call this function too)
    if ( !IsDeletingVehicleAlready ) 
    {
        IsDeletingVehicleAlready = TRUE;

        int i = vehiclevc->GetArrayID ();
		if ( Vehicles[i] == vehiclevc )
		{			    
            delete vehiclevc;
            dwVehicleCount--;
			Vehicles[i] = NULL;
        }
        else _asm int 3

        IsDeletingVehicleAlready = FALSE;
    }
}

VOID CPoolsSA::RemovePed ( DWORD ID )
{
	DEBUG_TRACE("VOID CPoolsSA::RemovePed ( DWORD ID )");
	static BOOL IsDeletingPedAlready = FALSE; // to prevent delete being called twice (delete playerped will call this function too)
	if(ID <= MAX_PEDS)
	{
		if(!IsDeletingPedAlready)
		{
			IsDeletingPedAlready= TRUE;
			delete Peds[ID];
		}

		Peds[ID] = 0;
	}
	IsDeletingPedAlready = FALSE;
}

char szOutput[1024];

void CPoolsSA::DumpPoolsStatus ()
{
    char*  poolNames[] = {"Buildings", "Peds", "Objects", "Dummies", "Vehicles", "ColModels", 
        "Tasks", "Events", "TaskAllocators", "PedIntelligences", "PedAttractors", 
        "EntryInfoNodes", "NodeRoutes", "PatrolRoutes", "PointRoutes", 
        "PointerNodeDoubleLinks", "PointerNodeSingleLinks" };

    int poolSizes[] = {13000,140,350,2500,110,10150,500,200,16,140,64,500,64,32,64,3200,70000};
    int iPosition = 0;
    char percent = '%';
    iPosition += _snprintf ( szOutput, 1024, "-----------------\n" );
    for ( int i = 0; i < MAX_POOLS; i++ )
    {
        int usedSpaces = GetNumberOfUsedSpaces ( (ePools)i );
        iPosition += _snprintf ( szOutput + iPosition, 1024 - iPosition, "%s: %d (%d) (%.2f%c)\n", poolNames[i], usedSpaces, poolSizes[i], ((float)usedSpaces / (float)poolSizes[i] * 100), percent  );
    }
    #ifdef MTA_DEBUG
    OutputDebugString ( szOutput );
    #endif
}

int CPoolsSA::GetNumberOfUsedSpaces ( ePools pool )
{
    DWORD dwFunc = NULL;
    DWORD dwThis = NULL;
    switch ( pool )
    {
        case BUILDING_POOL: dwFunc = FUNC_CBuildingPool_GetNoOfUsedSpaces; dwThis = CLASS_CBuildingPool; break;
        case PED_POOL: dwFunc = FUNC_CPedPool_GetNoOfUsedSpaces; dwThis = CLASS_CPedPool; break;
        case OBJECT_POOL: dwFunc = FUNC_CObjectPool_GetNoOfUsedSpaces; dwThis = CLASS_CObjectPool; break;
        case DUMMY_POOL: dwFunc = FUNC_CDummyPool_GetNoOfUsedSpaces; dwThis = CLASS_CDummyPool; break;
        case VEHICLE_POOL: dwFunc = FUNC_CVehiclePool_GetNoOfUsedSpaces; dwThis = CLASS_CVehiclePool; break;
        case COL_MODEL_POOL: dwFunc = FUNC_CColModelPool_GetNoOfUsedSpaces; dwThis = CLASS_CColModelPool; break;
        case TASK_POOL: dwFunc = FUNC_CTaskPool_GetNoOfUsedSpaces; dwThis = CLASS_CTaskPool; break;
        case EVENT_POOL: dwFunc = FUNC_CEventPool_GetNoOfUsedSpaces; dwThis = CLASS_CEventPool; break;
        case TASK_ALLOCATOR_POOL: dwFunc = FUNC_CTaskAllocatorPool_GetNoOfUsedSpaces; dwThis = CLASS_CTaskAllocatorPool; break;
        case PED_INTELLIGENCE_POOL: dwFunc = FUNC_CPedIntelligencePool_GetNoOfUsedSpaces; dwThis = CLASS_CPedIntelligencePool; break;
        case PED_ATTRACTOR_POOL: dwFunc = FUNC_CPedAttractorPool_GetNoOfUsedSpaces; dwThis = CLASS_CPedAttractorPool; break;
        case ENTRY_INFO_NODE_POOL: dwFunc = FUNC_CEntryInfoNodePool_GetNoOfUsedSpaces; dwThis = CLASS_CEntryInfoNodePool; break;
        case NODE_ROUTE_POOL: dwFunc = FUNC_CNodeRoutePool_GetNoOfUsedSpaces; dwThis = CLASS_CNodeRoutePool; break;
        case PATROL_ROUTE_POOL: dwFunc = FUNC_CPatrolRoutePool_GetNoOfUsedSpaces; dwThis = CLASS_CPatrolRoutePool; break;
        case POINT_ROUTE_POOL: dwFunc = FUNC_CPointRoutePool_GetNoOfUsedSpaces; dwThis = CLASS_CPointRoutePool; break;
        case POINTER_DOUBLE_LINK_POOL: dwFunc = FUNC_CPtrNodeDoubleLinkPool_GetNoOfUsedSpaces; dwThis = CLASS_CPtrNodeDoubleLinkPool; break;
        case POINTER_SINGLE_LINK_POOL: dwFunc = FUNC_CPtrNodeSingleLinkPool_GetNoOfUsedSpaces; dwThis = CLASS_CPtrNodeSingleLinkPool; break;
        default: return -1;
    }

    int iOut = -2;
    if ( *(DWORD *)dwThis != NULL )
    {
        _asm
        {
            mov     ecx, dwThis
            mov     ecx, [ecx]
            call    dwFunc
            mov     iOut, eax

        }
    }

    return iOut;
}

CEntryInfoNodePool * CPoolsSA::GetEntryInfoNodePool ( void )
{
    return EntryInfoNodePool;
}

int CEntryInfoNodePoolSA::GetNumberOfUsedSpaces ( void )
{
    DWORD dwFunc = FUNC_CEntryInfoNodePool_GetNoOfUsedSpaces;
    int iOut = 0;
    _asm
    {
        mov     ecx, CLASS_CEntryInfoNodePool
        mov     ecx, [ecx]
        call    dwFunc
        mov     iOut, eax
    }

    return iOut;
}

CPointerNodeDoubleLinkPool * CPoolsSA::GetPointerNodeDoubleLinkPool ( void )
{
    return PointerNodeDoubleLinkPool;
}

int CPointerNodeDoubleLinkPoolSA::GetNumberOfUsedSpaces ( void )
{
    DWORD dwFunc = FUNC_CPtrNodeDoubleLinkPool_GetNoOfUsedSpaces;
    int iOut = 0;
    _asm
    {
        mov     ecx, CLASS_CPtrNodeDoubleLinkPool
        mov     ecx, [ecx]
        call    dwFunc
        mov     iOut, eax
    }

    return iOut;
}

CPointerNodeSingleLinkPool * CPoolsSA::GetPointerNodeSingleLinkPool ( void )
{
    return PointerNodeSingleLinkPool;
}

int CPointerNodeSingleLinkPoolSA::GetNumberOfUsedSpaces ( void )
{
    DWORD dwFunc = FUNC_CPtrNodeSingleLinkPool_GetNoOfUsedSpaces;
    int iOut = 0;
    _asm
    {
        mov     ecx, CLASS_CPtrNodeSingleLinkPool
        mov     ecx, [ecx]
        call    dwFunc
        mov     iOut, eax
    }

    return iOut;
}