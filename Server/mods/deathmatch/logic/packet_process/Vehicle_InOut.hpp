void CGame::Packet_Vehicle_InOut(CVehicleInOutPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer)
    {
        // Joined?
        if (pPlayer->IsJoined())
        {
            // Grab the vehicle with the chosen ID
            ElementID     ID = Packet.GetID();
            unsigned char ucAction = Packet.GetAction();

            // Spawned?
            if (pPlayer->IsSpawned())
            {
                CElement* pVehicleElement = CElementIDs::GetElement(ID);
                if (pVehicleElement && IS_VEHICLE(pVehicleElement))
                {
                    CVehicle* pVehicle = static_cast<CVehicle*>(pVehicleElement);

                    // Handle it depending on the action
                    switch (ucAction)
                    {
                        // Vehicle get in request?
                        case VEHICLE_REQUEST_IN:
                        {
                            bool bFailed = true;
                            enum eFailReasons
                            {
                                FAIL_INVALID = 0,
                                FAIL_SCRIPT,
                                FAIL_SCRIPT_2,
                                FAIL_JACKED_ACTION,
                                FAIL_SEAT,
                                FAIL_DISTANCE,
                                FAIL_IN_VEHICLE,
                                FAIL_ACTION,
                                FAIL_TRAILER,
                            } failReason = FAIL_INVALID;

                            // Is this vehicle enterable? (not a trailer)
                            unsigned short usVehicleModel = pVehicle->GetModel();
                            if (!CVehicleManager::IsTrailer(usVehicleModel))
                            {
                                // He musn't already be doing something
                                if (pPlayer->GetVehicleAction() == CPlayer::VEHICLEACTION_NONE)
                                {
                                    // He musn't already be in a vehicle
                                    if (!pPlayer->GetOccupiedVehicle())
                                    {
                                        float fCutoffDistance = 50.0f;
                                        bool  bWarpIn = false;
                                        // Jax: is he in water and trying to get in a floating vehicle
                                        // Cazomino05: changed to check if the vehicle is in water not player
                                        if ((pPlayer->IsInWater() || Packet.GetOnWater()) && (usVehicleModel == VT_SKIMMER || usVehicleModel == VT_SEASPAR ||
                                                                                              usVehicleModel == VT_LEVIATHN || usVehicleModel == VT_VORTEX))
                                        {
                                            fCutoffDistance = 10.0f;
                                            bWarpIn = true;
                                        }
                                        if (usVehicleModel == VT_RCBARON)
                                        {            // warp in for rc baron.
                                            fCutoffDistance = 10.0f;
                                            bWarpIn = true;
                                        }

                                        CPed* pOccupant = pVehicle->GetOccupant(0);
                                        // If he's going to be jacking this vehicle, lets make sure he's very close to it
                                        if (pOccupant)
                                            fCutoffDistance = 10.0f;

                                        // Is he close enough to the vehicle?
                                        if (IsPointNearPoint3D(pPlayer->GetPosition(), pVehicle->GetPosition(), fCutoffDistance))
                                        {
                                            unsigned char ucSeat = Packet.GetSeat();
                                            unsigned char ucDoor = Packet.GetDoor();

                                            // Temp fix: Disable driver seat for train carriages since the whole vehicle sync logic is based on the the player
                                            // on the first seat being the vehicle syncer (Todo)
                                            if (pVehicle->GetVehicleType() == VEHICLE_TRAIN && ucSeat == 0 && pVehicle->GetTowedByVehicle())
                                                ucSeat++;

                                            // Going for driver?
                                            if (ucSeat == 0)
                                            {
                                                // Does it already have an occupant/occupying?
                                                if (!pOccupant)
                                                {
                                                    // Mark him as entering the vehicle
                                                    pPlayer->SetOccupiedVehicle(pVehicle, 0);
                                                    pPlayer->SetVehicleAction(CPlayer::VEHICLEACTION_ENTERING);
                                                    pVehicle->m_bOccupantChanged = false;

                                                    // Call the entering vehicle event
                                                    CLuaArguments Arguments;
                                                    Arguments.PushElement(pPlayer);            // player
                                                    Arguments.PushNumber(0);                   // seat
                                                    Arguments.PushBoolean(false);              // jacked
                                                    Arguments.PushNumber(ucDoor);              // Door
                                                    if (pVehicle->CallEvent("onVehicleStartEnter", Arguments))
                                                    {
                                                        // HACK?: check the player's vehicle-action is still the same (not warped in?)
                                                        if (pPlayer->GetVehicleAction() == CPlayer::VEHICLEACTION_ENTERING)
                                                        {
                                                            // Force the player as the syncer of the vehicle to which they are entering
                                                            m_pUnoccupiedVehicleSync->OverrideSyncer(pVehicle, pPlayer);

                                                            if (bWarpIn)
                                                            {
                                                                // Unmark him as entering the vehicle so WarpPedIntoVehicle will work
                                                                if (!pVehicle->m_bOccupantChanged)
                                                                {
                                                                    pPlayer->SetOccupiedVehicle(NULL, 0);
                                                                    pVehicle->SetOccupant(NULL, 0);
                                                                }

                                                                if (CStaticFunctionDefinitions::WarpPedIntoVehicle(pPlayer, pVehicle, 0))
                                                                {
                                                                    bFailed = false;
                                                                }
                                                                else
                                                                {
                                                                    // Warp failed
                                                                    pPlayer->SetVehicleAction(CPlayer::VEHICLEACTION_NONE);
                                                                    failReason = FAIL_SCRIPT;
                                                                }
                                                            }
                                                            else
                                                            {
                                                                // Tell everyone he can start entering the vehicle from his current position
                                                                CVehicleInOutPacket Reply(ID, 0, VEHICLE_REQUEST_IN_CONFIRMED, ucDoor);
                                                                Reply.SetSourceElement(pPlayer);
                                                                m_pPlayerManager->BroadcastOnlyJoined(Reply);
                                                                bFailed = false;
                                                            }
                                                        }
                                                    }
                                                    else
                                                    {
                                                        if (!pVehicle->m_bOccupantChanged)
                                                        {
                                                            pPlayer->SetOccupiedVehicle(NULL, 0);
                                                            pVehicle->SetOccupant(NULL, 0);
                                                        }
                                                        pPlayer->SetVehicleAction(CPlayer::VEHICLEACTION_NONE);
                                                        failReason = FAIL_SCRIPT;
                                                    }
                                                }
                                                else
                                                {
                                                    // TODO: support jacking peds (not simple!)
                                                    // Is the jacked person a player and stationary in the car (ie not getting in or out)
                                                    if (IS_PLAYER(pOccupant) && pOccupant->GetVehicleAction() == CPlayer::VEHICLEACTION_NONE)
                                                    {
                                                        // He's now jacking the car and the occupant is getting jacked
                                                        pPlayer->SetVehicleAction(CPlayer::VEHICLEACTION_JACKING);
                                                        pPlayer->SetJackingVehicle(pVehicle);
                                                        pVehicle->SetJackingPlayer(pPlayer);
                                                        pOccupant->SetVehicleAction(CPlayer::VEHICLEACTION_JACKED);

                                                        // Call the entering vehicle event
                                                        CLuaArguments EnterArguments;
                                                        EnterArguments.PushElement(pPlayer);              // player
                                                        EnterArguments.PushNumber(0);                     // seat
                                                        EnterArguments.PushElement(pOccupant);            // jacked
                                                        EnterArguments.PushNumber(ucDoor);                // Door
                                                        if (pVehicle->CallEvent("onVehicleStartEnter", EnterArguments))
                                                        {
                                                            // HACK?: check the player's vehicle-action is still the same (not warped in?)
                                                            if (pPlayer->GetVehicleAction() == CPlayer::VEHICLEACTION_JACKING)
                                                            {
                                                                // Call the exiting vehicle event
                                                                CLuaArguments ExitArguments;
                                                                ExitArguments.PushElement(pOccupant);            // player
                                                                ExitArguments.PushNumber(ucSeat);                // seat
                                                                ExitArguments.PushElement(pPlayer);              // jacker
                                                                if (pVehicle->CallEvent("onVehicleStartExit", ExitArguments))
                                                                {
                                                                    // HACK?: check the player's vehicle-action is still the same (not warped out?)
                                                                    if (pOccupant->GetVehicleAction() == CPlayer::VEHICLEACTION_JACKED)
                                                                    {
                                                                        /* Jax: we don't need to worry about a syncer if we already have and will have a driver
                                                                        // Force the player as the syncer of the vehicle to which they are entering
                                                                        m_pUnoccupiedVehicleSync->OverrideSyncer ( pVehicle, pPlayer );
                                                                        */

                                                                        // Broadcast a jack message (tells him he can get in, but he must jack it)
                                                                        CVehicleInOutPacket Reply(ID, 0, VEHICLE_REQUEST_JACK_CONFIRMED, ucDoor);
                                                                        Reply.SetSourceElement(pPlayer);
                                                                        m_pPlayerManager->BroadcastOnlyJoined(Reply);

                                                                        bFailed = false;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        else
                                                        {
                                                            pPlayer->SetVehicleAction(CPlayer::VEHICLEACTION_NONE);
                                                            pOccupant->SetVehicleAction(CPlayer::VEHICLEACTION_NONE);
                                                            failReason = FAIL_SCRIPT_2;
                                                        }
                                                    }
                                                    else
                                                        failReason = FAIL_JACKED_ACTION;
                                                }
                                            }
                                            else
                                            {
                                                CPed* pCurrentOccupant = pVehicle->GetOccupant(ucSeat);
                                                if (pCurrentOccupant || ucSeat > pVehicle->GetMaxPassengers())
                                                {
                                                    // Grab a free passenger spot in the vehicle
                                                    ucSeat = pVehicle->GetFreePassengerSeat();
                                                }
                                                if (ucSeat <= 8)
                                                {
                                                    // Mark him as entering the vehicle
                                                    pPlayer->SetOccupiedVehicle(pVehicle, ucSeat);
                                                    pPlayer->SetVehicleAction(CPlayer::VEHICLEACTION_ENTERING);
                                                    pVehicle->m_bOccupantChanged = false;

                                                    // Call the entering vehicle event
                                                    CLuaArguments Arguments;
                                                    Arguments.PushElement(pPlayer);            // player
                                                    Arguments.PushNumber(ucSeat);              // seat
                                                    Arguments.PushBoolean(false);              // jacked
                                                    Arguments.PushNumber(ucDoor);              // Door
                                                    if (pVehicle->CallEvent("onVehicleStartEnter", Arguments))
                                                    {
                                                        // HACK?: check the player's vehicle-action is still the same (not warped in?)
                                                        if (pPlayer->GetVehicleAction() == CPlayer::VEHICLEACTION_ENTERING)
                                                        {
                                                            if (bWarpIn)
                                                            {
                                                                // Unmark him as entering the vehicle so WarpPedIntoVehicle will work
                                                                if (!pVehicle->m_bOccupantChanged)
                                                                {
                                                                    pPlayer->SetOccupiedVehicle(NULL, 0);
                                                                    pVehicle->SetOccupant(NULL, ucSeat);
                                                                }

                                                                if (CStaticFunctionDefinitions::WarpPedIntoVehicle(pPlayer, pVehicle, ucSeat))
                                                                {
                                                                    bFailed = false;
                                                                }
                                                                else
                                                                {
                                                                    // Warp failed
                                                                    pPlayer->SetVehicleAction(CPlayer::VEHICLEACTION_NONE);
                                                                    failReason = FAIL_SCRIPT;
                                                                }
                                                            }
                                                            else
                                                            {
                                                                // Tell everyone he can start entering the vehicle from his current position
                                                                CVehicleInOutPacket Reply(ID, ucSeat, VEHICLE_REQUEST_IN_CONFIRMED, ucDoor);
                                                                Reply.SetSourceElement(pPlayer);
                                                                m_pPlayerManager->BroadcastOnlyJoined(Reply);
                                                                bFailed = false;
                                                            }
                                                        }
                                                    }
                                                    else
                                                    {
                                                        if (!pVehicle->m_bOccupantChanged)
                                                        {
                                                            pPlayer->SetOccupiedVehicle(NULL, 0);
                                                            pVehicle->SetOccupant(NULL, ucSeat);
                                                        }
                                                        pPlayer->SetVehicleAction(CPlayer::VEHICLEACTION_NONE);
                                                        failReason = FAIL_SCRIPT;
                                                    }
                                                }
                                                else
                                                    failReason = FAIL_SEAT;
                                            }
                                        }
                                        else
                                            failReason = FAIL_DISTANCE;
                                    }
                                    else
                                        failReason = FAIL_IN_VEHICLE;
                                }
                                else
                                    failReason = FAIL_ACTION;
                            }
                            else
                                failReason = FAIL_TRAILER;

                            if (bFailed)
                            {
                                // He can't get in
                                CVehicleInOutPacket Reply(ID, 0, VEHICLE_ATTEMPT_FAILED);
                                Reply.SetSourceElement(pPlayer);
                                Reply.SetFailReason((unsigned char)failReason);
                                // Was he too far away from the vehicle?
                                if (failReason == FAIL_DISTANCE)
                                {
                                    // Correct the vehicles position
                                    Reply.SetCorrectVector(pVehicle->GetPosition());
                                }
                                pPlayer->Send(Reply);
                            }

                            break;
                        }

                        // Vehicle in notification?
                        case VEHICLE_NOTIFY_IN:
                        {
                            // Is he entering?
                            if (pPlayer->GetVehicleAction() == CPlayer::VEHICLEACTION_ENTERING)
                            {
                                // Is he the occupant? (he must unless the client has fucked up)
                                unsigned char ucOccupiedSeat = pPlayer->GetOccupiedVehicleSeat();
                                if (pPlayer == pVehicle->GetOccupant(ucOccupiedSeat))
                                {
                                    // Mark him as successfully entered
                                    pPlayer->SetVehicleAction(CPlayer::VEHICLEACTION_NONE);

                                    // Update our engine State
                                    pVehicle->SetEngineOn(true);

                                    // Tell everyone he's in (they should warp him in)
                                    CVehicleInOutPacket Reply(ID, ucOccupiedSeat, VEHICLE_NOTIFY_IN_RETURN);
                                    Reply.SetSourceElement(pPlayer);
                                    m_pPlayerManager->BroadcastOnlyJoined(Reply);

                                    // Call the player->vehicle event
                                    CLuaArguments Arguments;
                                    Arguments.PushElement(pVehicle);                 // vehice
                                    Arguments.PushNumber(ucOccupiedSeat);            // seat
                                    Arguments.PushBoolean(false);                    // jacked
                                    pPlayer->CallEvent("onPlayerVehicleEnter", Arguments);

                                    // Call the vehicle->player event
                                    CLuaArguments Arguments2;
                                    Arguments2.PushElement(pPlayer);                  // player
                                    Arguments2.PushNumber(ucOccupiedSeat);            // seat
                                    Arguments2.PushBoolean(false);                    // jacked
                                    pVehicle->CallEvent("onVehicleEnter", Arguments2);
                                }
                            }

                            break;
                        }

                        // Vehicle in aborted notification?
                        case VEHICLE_NOTIFY_IN_ABORT:
                        {
                            // Is he entering?
                            if (pPlayer->GetVehicleAction() == CPlayer::VEHICLEACTION_ENTERING)
                            {
                                // Is he the occupant? (he must unless the client has fucked up)
                                unsigned char ucOccupiedSeat = pPlayer->GetOccupiedVehicleSeat();
                                if (pPlayer == pVehicle->GetOccupant(ucOccupiedSeat))
                                {
                                    unsigned char ucDoor = Packet.GetDoor();
                                    float         fDoorAngle = Packet.GetDoorAngle();

                                    // Mark that he's in no vehicle
                                    pPlayer->SetVehicleAction(CPlayer::VEHICLEACTION_NONE);
                                    pPlayer->SetOccupiedVehicle(NULL, 0);
                                    pVehicle->SetOccupant(NULL, ucOccupiedSeat);

                                    // Update the door angle.
                                    pVehicle->SetDoorOpenRatio(ucDoor + 2, fDoorAngle);

                                    // Tell everyone he's in (they should warp him in)
                                    CVehicleInOutPacket Reply(ID, ucOccupiedSeat, VEHICLE_NOTIFY_IN_ABORT_RETURN, ucDoor);
                                    Reply.SetSourceElement(pPlayer);
                                    Reply.SetDoorAngle(fDoorAngle);
                                    m_pPlayerManager->BroadcastOnlyJoined(Reply);
                                }
                            }

                            break;
                        }

                        // Vehicle get out request?
                        case VEHICLE_REQUEST_OUT:
                        {
                            // Is he getting jacked?
                            if (pPlayer->GetVehicleAction() == CPlayer::VEHICLEACTION_NONE)
                            {
                                // Does it have an occupant and is the occupant the requesting player?
                                unsigned ucOccupiedSeat = pPlayer->GetOccupiedVehicleSeat();
                                if (pPlayer == pVehicle->GetOccupant(ucOccupiedSeat))
                                {
                                    // Call the exiting vehicle event
                                    CLuaArguments Arguments;
                                    Arguments.PushElement(pPlayer);                    // player
                                    Arguments.PushNumber(ucOccupiedSeat);              // seat
                                    Arguments.PushBoolean(false);                      // jacked
                                    Arguments.PushNumber(Packet.GetDoor());            // door being used
                                    if (pVehicle->CallEvent("onVehicleStartExit", Arguments) && pPlayer->GetOccupiedVehicle() == pVehicle)
                                    {
                                        // Mark him as exiting the vehicle
                                        pPlayer->SetVehicleAction(CPlayer::VEHICLEACTION_EXITING);

                                        // Tell everyone he can start exiting the vehicle
                                        CVehicleInOutPacket Reply(ID, ucOccupiedSeat, VEHICLE_REQUEST_OUT_CONFIRMED, Packet.GetDoor());
                                        Reply.SetSourceElement(pPlayer);
                                        m_pPlayerManager->BroadcastOnlyJoined(Reply);
                                    }
                                    else
                                    {
                                        // Tell him he can't exit (script denied it or he was
                                        // already warped out)
                                        CVehicleInOutPacket Reply(ID, 0, VEHICLE_ATTEMPT_FAILED);
                                        Reply.SetSourceElement(pPlayer);
                                        pPlayer->Send(Reply);
                                    }
                                }
                                else
                                {
                                    // Tell him he can't exit
                                    CVehicleInOutPacket Reply(ID, 0, VEHICLE_ATTEMPT_FAILED);
                                    Reply.SetSourceElement(pPlayer);
                                    pPlayer->Send(Reply);
                                }
                            }
                            else
                            {
                                // Tell him he can't exit
                                CVehicleInOutPacket Reply(ID, 0, VEHICLE_ATTEMPT_FAILED);
                                Reply.SetSourceElement(pPlayer);
                                pPlayer->Send(Reply);
                            }

                            break;
                        }

                        // Vehicle out notify?
                        case VEHICLE_NOTIFY_OUT:
                        {
                            // Is he already getting out?
                            if (pPlayer->GetVehicleAction() == CPlayer::VEHICLEACTION_EXITING)
                            {
                                // Does it have an occupant and is the occupant the requesting player?
                                unsigned char ucOccupiedSeat = pPlayer->GetOccupiedVehicleSeat();
                                if (pPlayer == pVehicle->GetOccupant(ucOccupiedSeat))
                                {
                                    // Mark the player/vehicle as empty
                                    pVehicle->SetOccupant(NULL, ucOccupiedSeat);
                                    pPlayer->SetOccupiedVehicle(NULL, 0);
                                    pPlayer->SetVehicleAction(CPlayer::VEHICLEACTION_NONE);

                                    // Force the player that just left the vehicle as the syncer
                                    m_pUnoccupiedVehicleSync->OverrideSyncer(pVehicle, pPlayer);

                                    // Tell everyone he can start exiting the vehicle
                                    CVehicleInOutPacket Reply(ID, ucOccupiedSeat, VEHICLE_NOTIFY_OUT_RETURN);
                                    Reply.SetSourceElement(pPlayer);
                                    m_pPlayerManager->BroadcastOnlyJoined(Reply);

                                    // Call the player->vehicle event
                                    CLuaArguments Arguments;
                                    Arguments.PushElement(pVehicle);                 // vehicle
                                    Arguments.PushNumber(ucOccupiedSeat);            // seat
                                    Arguments.PushBoolean(false);                    // jacker
                                    Arguments.PushBoolean(false);                    // forcedByScript
                                    pPlayer->CallEvent("onPlayerVehicleExit", Arguments);

                                    // Call the vehicle->player event
                                    CLuaArguments Arguments2;
                                    Arguments2.PushElement(pPlayer);                  // player
                                    Arguments2.PushNumber(ucOccupiedSeat);            // seat
                                    Arguments2.PushBoolean(false);                    // jacker
                                    Arguments2.PushBoolean(false);                    // forcedByScript
                                    pVehicle->CallEvent("onVehicleExit", Arguments2);
                                }
                            }

                            break;
                        }

                        // Vehicle out aborted notification?
                        case VEHICLE_NOTIFY_OUT_ABORT:
                        {
                            // Is he entering?
                            if (pPlayer->GetVehicleAction() == CPlayer::VEHICLEACTION_EXITING)
                            {
                                // Is he the occupant?
                                unsigned char ucOccupiedSeat = pPlayer->GetOccupiedVehicleSeat();
                                if (pPlayer == pVehicle->GetOccupant(ucOccupiedSeat))
                                {
                                    // Mark that he's in no vehicle
                                    pPlayer->SetVehicleAction(CPlayer::VEHICLEACTION_NONE);

                                    // Tell everyone he's in (they should warp him in)
                                    CVehicleInOutPacket Reply(ID, ucOccupiedSeat, VEHICLE_NOTIFY_OUT_ABORT_RETURN);
                                    Reply.SetSourceElement(pPlayer);
                                    m_pPlayerManager->BroadcastOnlyJoined(Reply);
                                }
                            }

                            break;
                        }

                        case VEHICLE_NOTIFY_FELL_OFF:
                        {
                            // Check that the player is in the given vehicle
                            unsigned char ucOccupiedSeat = pPlayer->GetOccupiedVehicleSeat();
                            if (pVehicle->GetOccupant(ucOccupiedSeat) == pPlayer)
                            {
                                // Remove him from the vehicle
                                pPlayer->SetOccupiedVehicle(NULL, 0);
                                pVehicle->SetOccupant(NULL, ucOccupiedSeat);

                                // Force the player that just left the vehicle as the syncer
                                m_pUnoccupiedVehicleSync->OverrideSyncer(pVehicle, pPlayer);

                                pPlayer->SetVehicleAction(CPlayer::VEHICLEACTION_NONE);
                                // Tell the other players about it
                                CVehicleInOutPacket Reply(ID, ucOccupiedSeat, VEHICLE_NOTIFY_FELL_OFF_RETURN);
                                Reply.SetSourceElement(pPlayer);
                                m_pPlayerManager->BroadcastOnlyJoined(Reply);

                                // Call the player->vehicle event
                                CLuaArguments Arguments;
                                Arguments.PushElement(pVehicle);                 // vehicle
                                Arguments.PushNumber(ucOccupiedSeat);            // seat
                                Arguments.PushBoolean(false);                    // jacker
                                Arguments.PushBoolean(false);                    // forcedByScript
                                pPlayer->CallEvent("onPlayerVehicleExit", Arguments);

                                // Call the vehicle->player event
                                CLuaArguments Arguments2;
                                Arguments2.PushElement(pPlayer);                  // player
                                Arguments2.PushNumber(ucOccupiedSeat);            // seat
                                Arguments2.PushBoolean(false);                    // jacker
                                Arguments2.PushBoolean(false);                    // forcedByScript
                                pVehicle->CallEvent("onVehicleExit", Arguments2);
                            }

                            break;
                        }

                        case VEHICLE_NOTIFY_JACK:            // Finished jacking him
                        {
                            // Is the sender jacking?
                            if (pPlayer->GetVehicleAction() == CPlayer::VEHICLEACTION_JACKING)
                            {
                                // Grab the jacked player
                                CPed* pJacked = pVehicle->GetOccupant(0);
                                if (pJacked)
                                {
                                    // TODO! CHECK THE CAR ID
                                    // Throw the jacked player out
                                    pJacked->SetOccupiedVehicle(NULL, 0);
                                    pJacked->SetVehicleAction(CPlayer::VEHICLEACTION_NONE);

                                    // Put the jacking player into it
                                    pPlayer->SetOccupiedVehicle(pVehicle, 0);
                                    pPlayer->SetVehicleAction(CPlayer::VEHICLEACTION_NONE);
                                    pPlayer->SetJackingVehicle(NULL);
                                    pVehicle->SetJackingPlayer(NULL);

                                    // Tell everyone about it
                                    ElementID           PlayerID = pPlayer->GetID();
                                    ElementID           JackedID = pJacked->GetID();
                                    CVehicleInOutPacket Reply(ID, 0, VEHICLE_NOTIFY_JACK_RETURN, PlayerID, JackedID);
                                    Reply.SetSourceElement(pPlayer);
                                    m_pPlayerManager->BroadcastOnlyJoined(Reply);

                                    // Execute the player->vehicle script function for the jacked player
                                    CLuaArguments ArgumentsExit;
                                    ArgumentsExit.PushElement(pVehicle);            // vehicle
                                    ArgumentsExit.PushNumber(0);                    // seat
                                    ArgumentsExit.PushElement(pPlayer);             // jacker
                                    ArgumentsExit.PushBoolean(false);               // forcedByScript
                                    pJacked->CallEvent("onPlayerVehicleExit", ArgumentsExit);

                                    // Execute the vehicle->vehicle script function for the jacked player
                                    CLuaArguments ArgumentsExit2;
                                    ArgumentsExit2.PushElement(pJacked);            // player
                                    ArgumentsExit2.PushNumber(0);                   // seat
                                    ArgumentsExit2.PushElement(pPlayer);            // jacker
                                    ArgumentsExit2.PushBoolean(false);              // forcedByScript
                                    pVehicle->CallEvent("onVehicleExit", ArgumentsExit2);

                                    // Execute the player->vehicle script function
                                    CLuaArguments ArgumentsEnter;
                                    ArgumentsEnter.PushElement(pVehicle);            // vehicle
                                    ArgumentsEnter.PushNumber(0);                    // seat
                                    ArgumentsEnter.PushElement(pJacked);             // jacked
                                    pPlayer->CallEvent("onPlayerVehicleEnter", ArgumentsEnter);

                                    // Execute the vehicle->player script function
                                    CLuaArguments ArgumentsEnter2;
                                    ArgumentsEnter2.PushElement(pPlayer);            // player
                                    ArgumentsEnter2.PushNumber(0);                   // seat
                                    ArgumentsEnter2.PushElement(pJacked);            // jacked
                                    pVehicle->CallEvent("onVehicleEnter", ArgumentsEnter2);
                                }
                                else
                                {
                                    // Put the jacking player into it
                                    pPlayer->SetOccupiedVehicle(pVehicle, 0);
                                    pPlayer->SetVehicleAction(CPlayer::VEHICLEACTION_NONE);
                                    pPlayer->SetJackingVehicle(NULL);
                                    pVehicle->SetJackingPlayer(NULL);

                                    // Tell everyone about it
                                    CVehicleInOutPacket Reply(ID, 0, VEHICLE_NOTIFY_IN_RETURN);
                                    Reply.SetSourceElement(pPlayer);
                                    m_pPlayerManager->BroadcastOnlyJoined(Reply);

                                    // Execute the player->vehicle script function
                                    CLuaArguments Arguments;
                                    Arguments.PushElement(pVehicle);            // vehice
                                    Arguments.PushNumber(0);                    // seat
                                    Arguments.PushBoolean(false);               // jacked
                                    pPlayer->CallEvent("onPlayerVehicleEnter", Arguments);

                                    // Execute the vehicle->player script function
                                    CLuaArguments Arguments2;
                                    Arguments2.PushElement(pPlayer);            // player
                                    Arguments2.PushNumber(0);                   // seat
                                    Arguments2.PushBoolean(false);              // jacked
                                    pVehicle->CallEvent("onVehicleEnter", Arguments2);
                                }
                            }

                            break;
                        }

                        case VEHICLE_NOTIFY_JACK_ABORT:
                        {
                            // Is the sender jacking?
                            if (pPlayer->GetVehicleAction() == CPlayer::VEHICLEACTION_JACKING)
                            {
                                unsigned char ucDoor = Packet.GetDoor();
                                float         fAngle = Packet.GetDoorAngle();
                                CPed*         pJacked = pVehicle->GetOccupant(0);

                                // Mark that the jacker is in no vehicle
                                pPlayer->SetVehicleAction(CPlayer::VEHICLEACTION_NONE);
                                pPlayer->SetOccupiedVehicle(NULL, 0);
                                pPlayer->SetJackingVehicle(NULL);
                                pVehicle->SetJackingPlayer(NULL);

                                // Set the door angle.
                                pVehicle->SetDoorOpenRatio(ucDoor, fAngle);

                                // Tell everyone he aborted
                                CVehicleInOutPacket Reply(ID, 0, VEHICLE_NOTIFY_IN_ABORT_RETURN, ucDoor);
                                Reply.SetSourceElement(pPlayer);
                                Reply.SetDoorAngle(fAngle);
                                m_pPlayerManager->BroadcastOnlyJoined(Reply);

                                // The jacked is still inside?
                                if (pJacked)
                                {
                                    // Did he already start jacking him? (we're trusting the client here!)
                                    if (Packet.GetStartedJacking() == 1)
                                    {
                                        // Get the jacked out
                                        pJacked->SetOccupiedVehicle(NULL, 0);

                                        // No driver in this vehicle
                                        pVehicle->SetOccupant(NULL, 0);

                                        // Tell everyone to get the jacked person out
                                        CVehicleInOutPacket JackedReply(ID, 0, VEHICLE_NOTIFY_OUT_RETURN);
                                        JackedReply.SetSourceElement(pJacked);
                                        m_pPlayerManager->BroadcastOnlyJoined(JackedReply);
                                    }
                                    pJacked->SetVehicleAction(CPlayer::VEHICLEACTION_NONE);
                                }
                            }

                            break;
                        }

                        default:
                        {
                            DisconnectConnectionDesync(this, *pPlayer, 2);
                            return;
                        }
                    }
                }
                else
                {
                    CVehicleInOutPacket Reply(ID, 0, VEHICLE_ATTEMPT_FAILED);
                    Reply.SetSourceElement(pPlayer);
                    pPlayer->Send(Reply);
                }
            }
            else
            {
                CVehicleInOutPacket Reply(ID, 0, VEHICLE_ATTEMPT_FAILED);
                Reply.SetSourceElement(pPlayer);
                pPlayer->Send(Reply);
            }
        }
        else
        {
            DisconnectConnectionDesync(this, *pPlayer, 5);
            return;
        }
    }
}
