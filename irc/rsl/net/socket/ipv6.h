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

#ifndef _RSL_NET_SOCKET_IPV6_H
#define _RSL_NET_SOCKET_IPV6_H

#include "ip.h"

namespace Rsl { namespace Net { namespace Socket {

  /**
   * IPV4 Address conversion and storage
   */
  class IPV6Addr : public IPAddr
  {
  public:
    IPV6Addr();
    /**
     * Main constructor
     * @param addr Null terminated character string with desired address
     */
    IPV6Addr(const char* addr, const char* service);

    IPV6Addr(const IPV6Addr& Right);

    void Create(const char* addr, const char* service);
    IPAddr* Clone() const;

    /**
     * State of DNS query.
     * @return True if it was ok, false if not
     */
    bool Ok() const;
    /**
     * If error, shows the error string message.
     * @return Last error message
     * @see Errno()
     */
    const char * Error() const;
    /**
     * If Error, returns the error id.
     * @return Last error id
     * @see Error()
     */
    int Errno() const;

  private:
    int m_errno;
  };

}; }; };

#endif /* #ifndef _RSL_NET_SOCKET_IPV6_H */

