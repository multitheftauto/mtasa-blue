/// \file
/// \brief \b [Internal] Defines the default maximum transfer unit.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef DEFAULT_MTU_SIZE

/// The MTU size to use if RakPeer::SetMTUSize() is not called.
/// \remarks I think many people forget to call RakPeer::SetMTUSize() so I'm setting this to 1500 by default for efficiency.
/// \li \em 17914 16 Mbit/Sec Token Ring
/// \li \em 4464 4 Mbits/Sec Token Ring
/// \li \em 4352 FDDI
/// \li \em 1500. The largest Ethernet packet size \b recommended. This is the typical setting for non-PPPoE, non-VPN connections. The default value for NETGEAR routers, adapters and switches.
/// \li \em 1492. The size PPPoE prefers.
/// \li \em 1472. Maximum size to use for pinging. (Bigger packets are fragmented.)
/// \li \em 1468. The size DHCP prefers.
/// \li \em 1460. Usable by AOL if you don't have large email attachments, etc.
/// \li \em 1430. The size VPN and PPTP prefer.
/// \li \em 1400. Maximum size for AOL DSL.
/// \li \em 576. Typical value to connect to dial-up ISPs.
#if defined(_XBOX) || defined(X360)
#define DEFAULT_MTU_SIZE 1264
#else
#define DEFAULT_MTU_SIZE 1492
#endif

/// The largest value for an UDP datagram
/// \sa RakPeer::SetMTUSize()
#if defined(_XBOX) || defined(X360)
#define MAXIMUM_MTU_SIZE 1264
#else
#define MAXIMUM_MTU_SIZE 1492
#endif

#endif
