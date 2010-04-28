/*
 * Copyright (c) 2007, Alberto Alonso Pinto
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions
 *       and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 *       and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Alberto Alonso Pinto nor the names of its contributors may be used to endorse or
 *       promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RSL_NET_SOCKET_IP_H
#define _RSL_NET_SOCKET_IP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

namespace Rsl { namespace Net { namespace Socket {

  /**
   * IP Address conversion and storage base class.
   * Not for direct use. You must use IPV4Addr and IPV6Addr instead.
   */
  class IPAddr
  {
  protected:
    void CloneAddrinfo(const addrinfo* from);

  public:
    IPAddr();
    IPAddr(const IPAddr& copyFrom);
    const IPAddr& operator=(const IPAddr& copyFrom);
    virtual ~IPAddr();

    virtual void Create(const char* addr, const char* service) {};
    virtual IPAddr* Clone() const;

    /**
     * State of DNS query.
     * @return True if was ok, false if not.
     */
    virtual bool Ok() const {};
    /**
     * If error, shows the error string message.
     * @return Last error message
     * @see Errno()
     */
    virtual const char * Error() const {};
    /**
     * If Error, returns the error id.
     * @return Last error id
     * @see Error()
     */
    virtual int Errno() const {};
    /**
     * Returns the solved IP Address.
     * @return Solved IP Address
     * @see GetAddrStr()
     */
    const addrinfo* GetAddr() const;
    /**
     * Returns human-readable solved IP address.
     * @param buffer Character array where solved IP address will be stored to
     * @param length Length of character array
     * @see GetAddr()
     */
    const char * GetAddrStr(char* buffer, size_t length) const;

    unsigned short GetPort() const;

  protected:
    /**
     * Inherited classes must fill this structure
     */
    addrinfo *m_info;
  };

}; }; };

#endif /* #ifndef _RSL_NET_SOCKET_IP_H */

