#ifndef __RAKNET_TIME_H
#define __RAKNET_TIME_H

#include "NativeTypes.h"
#include "RakNetDefines.h"

// Define __GET_TIME_64BIT if you want to use large types for GetTime (takes more bandwidth when you transmit time though!)
// You would want to do this if your system is going to run long enough to overflow the millisecond counter (over a month)
#ifdef __GET_TIME_64BIT
typedef uint64_t RakNetTime;
typedef uint64_t RakNetTimeUS;
typedef uint64_t RakNetTimeMS;
typedef uint64_t RakNetTimeUS;
#else
typedef uint32_t RakNetTime;
typedef uint32_t RakNetTimeMS;
typedef uint64_t RakNetTimeUS;
typedef uint64_t RakNetTimeUS;
#endif

#endif
