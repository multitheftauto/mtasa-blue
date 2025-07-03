---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Long: tlspassword
Arg: <string>
Help: TLS password
Added: 7.21.4
Protocols: TLS
Category: tls auth
Multi: single
See-also:
  - tlsuser
Example:
  - --tlspassword pwd --tlsuser user $URL
---

# `--tlspassword`

Set password to use with the TLS authentication method specified with
--tlsauthtype. Requires that --tlsuser is set.

This option does not work with TLS 1.3.
