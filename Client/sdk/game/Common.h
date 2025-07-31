/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/sdk/game/Common.h
 *  PURPOSE:     Grand Theft Auto: San Andreas game definitions
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include <stdint.h>

// Limits for MTA
#define MAX_VEHICLES_MTA                    64          // Real limit is 110
#define MAX_PEDS_MTA                        110         // Real limit is 140
#define MAX_OBJECTS_MTA                     1000        // Real limit is 1200
#define MAX_ENTRY_INFO_NODES_MTA            72000       // Real limit is 72600  ( MAX_OBJECTS_MTA * 72 ) [Large col models are the cause of high usage]
#define MAX_POINTER_SINGLE_LINKS_MTA        85000       // Real limit is 90000 [Large col models are the cause of high usage]
#define MAX_POINTER_DOUBLE_LINKS_MTA        74000       // Real limit is 74800  ( MAX_OBJECTS_MTA * 72 + 2000 )

// Real limits for GTA
#define MAX_VEHICLES                        ( MAX_VEHICLES_MTA + 46 )               // 110
#define MAX_PEDS                            ( MAX_PEDS_MTA + 30 )                   // 140
#define MAX_OBJECTS                         ( MAX_OBJECTS_MTA + 200 )               // 1200
#define MAX_BUILDINGS                       13000
#define MAX_DUMMIES                         2500
#define MAX_ENTRY_INFO_NODES                ( MAX_ENTRY_INFO_NODES_MTA + 600 )      // 72600
#define MAX_POINTER_SINGLE_LINKS            ( MAX_POINTER_SINGLE_LINKS_MTA + 5000 ) // 90000 May be changed in runtime
#define MAX_POINTER_DOUBLE_LINKS            ( MAX_POINTER_DOUBLE_LINKS_MTA + 800 )  // 74800
#define MAX_RWOBJECT_INSTANCES              2500

/**
 * Flags used to define weapon characteristics
 */
// aiming options
#define WEAPONTYPE_CANAIM           (0x000001)  // can auto target to aim
#define WEAPONTYPE_CANAIMWITHARM    (0x000002)  // only needs arm to aim
#define WEAPONTYPE_FIRSTPERSON      (0x000004)  // uses 1st person aim
#define WEAPONTYPE_CANFREEAIM       (0x000008)  // can only use free aiming

// movement options
#define WEAPONTYPE_MOVEAIM          (0x000010)  // can move and aim at same time
#define WEAPONTYPE_MOVEFIRE         (0x000020)  // can move and fire at same time

// basic characteristics
#define WEAPONTYPE_THROW            (0x000100)  // is a throwing weapon
#define WEAPONTYPE_HEAVY            (0x000200)  // heavy weapon - can't jump
#define WEAPONTYPE_CONTINUOUS_FIRE  (0x000400)  // fires every frame within loop (ie paint spray)
#define WEAPONTYPE_TWIN_PISTOLS     (0x000800)  // can use 2x guns at same time

// these are gun anim options
#define WEAPONTYPE_ANIM_RELOAD      (0x001000)  // weapon has reload anims
#define WEAPONTYPE_ANIM_CROUCHFIRE  (0x002000)  // weapon has crouching anims
#define WEAPONTYPE_RELOAD2LOOPSTART (0x004000)  // loop from end of reload to fire loop start
#define WEAPONTYPE_LONG_RELOAD_TIME (0x008000)  // force a longer reload time!

// these are area effect/shot options
#define WEAPONTYPE_SLOWS_DOWN       (0x010000)  //
#define WEAPONTYPE_RANDOM_SPEED     (0x020000)  //
#define WEAPONTYPE_FORCE_FINISH_ANIM (0x040000)  // force the anim to finish player after aim/fire rather than blending out
#define WEAPONTYPE_EXPANDS          (0x080000)  //
#define WEAPONTYPE_CHAINGUN WEAPONTYPE_MINIGUN
