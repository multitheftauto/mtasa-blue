---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Long: proxy-tlspassword
Arg: <string>
Help: TLS password for HTTPS proxy
Added: 7.52.0
Category: proxy tls auth
Multi: single
See-also:
  - proxy
  - proxy-tlsuser
Example:
  - --proxy-tlspassword passwd -x https://proxy $URL
---

# `--proxy-tlspassword`

Set password to use with the TLS authentication method specified with
--proxy-tlsauthtype when using HTTPS proxy. Requires that --proxy-tlsuser is
set.

This option does not work with TLS 1.3.

Equivalent to --tlspassword but used in HTTPS proxy context.
