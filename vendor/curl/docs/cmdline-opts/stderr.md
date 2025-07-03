---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Long: stderr
Arg: <file>
Help: Where to redirect stderr
Category: verbose global
Added: 6.2
Multi: single
Scope: global
See-also:
  - verbose
  - silent
Example:
  - --stderr output.txt $URL
---

# `--stderr`

Redirect all writes to stderr to the specified file instead. If the filename
is a plain '-', it is instead written to stdout.
