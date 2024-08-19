#pragma once

#ifndef _WIN32
#ifndef SOCKET
#define SOCKET int
#endif
#endif

namespace dpp
{
	/**
	 * @brief Represents a socket file descriptor.
	 * This is used to ensure parity between windows and unix-like systems.
	 */
	typedef SOCKET socket;
} // namespace dpp

#ifndef SOCKET_ERROR
/**
 * @brief Represents a socket in error state
 */
#define SOCKET_ERROR -1
#endif

#ifndef INVALID_SOCKET
/**
 * @brief Represents a socket which is not yet assigned
 */
#define INVALID_SOCKET ~0
#endif
