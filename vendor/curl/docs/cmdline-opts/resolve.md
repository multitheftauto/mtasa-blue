---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Long: resolve
Arg: <[+]host:port:addr[,addr]...>
Help: Resolve host+port to address
Added: 7.21.3
Category: connection dns
Multi: append
See-also:
  - connect-to
  - alt-svc
Example:
  - --resolve example.com:443:127.0.0.1 $URL
  - --resolve example.com:443:[2001:db8::252f:efd6] $URL
---

# `--resolve`

Provide a custom address for a specific host and port pair. Using this, you
can make the curl requests(s) use a specified address and prevent the
otherwise normally resolved address to be used. Consider it a sort of
/etc/hosts alternative provided on the command line. The port number should be
the number used for the specific protocol the host is used for. It means you
need several entries if you want to provide addresses for the same host but
different ports.

By specifying `*` as host you can tell curl to resolve any host and specific
port pair to the specified address. Wildcard is resolved last so any --resolve
with a specific host and port is used first.

The provided address set by this option is used even if --ipv4 or --ipv6 is
set to make curl use another IP version.

By prefixing the host with a '+' you can make the entry time out after curl's
default timeout (1 minute). Note that this only makes sense for long running
parallel transfers with a lot of files. In such cases, if this option is used
curl tries to resolve the host as it normally would once the timeout has
expired.

Provide IPv6 addresses within [brackets].

To redirect connects from a specific hostname or any hostname, independently
of port number, consider the --connect-to option.

Support for resolving with wildcard was added in 7.64.0.

Support for the '+' prefix was added in 7.75.0.

Support for specifying the host component as an IPv6 address was added in 8.13.0.
