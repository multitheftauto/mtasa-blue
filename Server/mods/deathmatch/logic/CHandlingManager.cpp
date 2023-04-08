/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CHandlingManager.cpp
 *  PURPOSE:     Vehicle handling manager
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CHandlingManager.h"
#include "CCommon.h"

CHandlingManager::CHandlingManager()
{
    // http://www.gtamodding.com/index.php?title=Handling.cfg#GTA_San_Andreas
    // http://projectcerbera.com/gta/sa/tutorials/handling

    m_HandlingNames["mass"] = HANDLING_MASS;                                                             // works (mass > 0)
    m_HandlingNames["turnMass"] = HANDLING_TURNMASS;                                                     // works
    m_HandlingNames["dragCoeff"] = HANDLING_DRAGCOEFF;                                                   // works
    m_HandlingNames["centerOfMass"] = HANDLING_CENTEROFMASS;                                             // works
    m_HandlingNames["percentSubmerged"] = HANDLING_PERCENTSUBMERGED;                                     // works
    m_HandlingNames["tractionMultiplier"] = HANDLING_TRACTIONMULTIPLIER;                                 // works
    m_HandlingNames["driveType"] = HANDLING_DRIVETYPE;                                                   // works
    m_HandlingNames["engineType"] = HANDLING_ENGINETYPE;                                                 // works
    m_HandlingNames["numberOfGears"] = HANDLING_NUMOFGEARS;                                              // works
    m_HandlingNames["engineAcceleration"] = HANDLING_ENGINEACCELERATION;                                 // works
    m_HandlingNames["engineInertia"] = HANDLING_ENGINEINERTIA;                                           // works
    m_HandlingNames["maxVelocity"] = HANDLING_MAXVELOCITY;                                               // works
    m_HandlingNames["brakeDeceleration"] = HANDLING_BRAKEDECELERATION;                                   // works
    m_HandlingNames["brakeBias"] = HANDLING_BRAKEBIAS;                                                   // works
    m_HandlingNames["ABS"] = HANDLING_ABS;                                                               // has no effect in vanilla gta either
    m_HandlingNames["steeringLock"] = HANDLING_STEERINGLOCK;                                             // works
    m_HandlingNames["tractionLoss"] = HANDLING_TRACTIONLOSS;                                             // works
    m_HandlingNames["tractionBias"] = HANDLING_TRACTIONBIAS;                                             // works
    m_HandlingNames["suspensionForceLevel"] = HANDLING_SUSPENSION_FORCELEVEL;                            // works
    m_HandlingNames["suspensionDamping"] = HANDLING_SUSPENSION_DAMPING;                                  // works
    m_HandlingNames["suspensionHighSpeedDamping"] = HANDLING_SUSPENSION_HIGHSPEEDDAMPING;                // works
    m_HandlingNames["suspensionUpperLimit"] = HANDLING_SUSPENSION_UPPER_LIMIT;                           // works
    m_HandlingNames["suspensionLowerLimit"] = HANDLING_SUSPENSION_LOWER_LIMIT;                           // works
    m_HandlingNames["suspensionFrontRearBias"] = HANDLING_SUSPENSION_FRONTREARBIAS;                      // works
    m_HandlingNames["suspensionAntiDiveMultiplier"] = HANDLING_SUSPENSION_ANTIDIVEMULTIPLIER;            // works
    m_HandlingNames["collisionDamageMultiplier"] = HANDLING_COLLISIONDAMAGEMULTIPLIER;                   // works
    m_HandlingNames["seatOffsetDistance"] = HANDLING_SEATOFFSETDISTANCE;                                 // works
    m_HandlingNames["monetary"] = HANDLING_MONETARY;                      // useless as it only influences SP stats (value of damaged property)
    m_HandlingNames["handlingFlags"] = HANDLING_HANDLINGFLAGS;            // works
    m_HandlingNames["modelFlags"] = HANDLING_MODELFLAGS;                  // works
    m_HandlingNames["headLight"] = HANDLING_HEADLIGHT;                    // doesn't work
    m_HandlingNames["tailLight"] = HANDLING_TAILLIGHT;                    // doesn't seem to work*
    m_HandlingNames["animGroup"] = HANDLING_ANIMGROUP;                    // works model based
}
//* needs testing by someone who knows more about handling

CHandlingManager::~CHandlingManager()
{
}

eHandlingProperty CHandlingManager::GetPropertyEnumFromName(std::string strName)
{
    std::map<std::string, eHandlingProperty>::iterator it;
    it = m_HandlingNames.find(strName);

    if (it != m_HandlingNames.end())
    {
        return it->second;
    }

    return HANDLING_MAX;
}
