---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLMOPT_MAXCONNECTS
Section: 3
Source: libcurl
See-also:
  - CURLMOPT_MAX_HOST_CONNECTIONS (3)
  - CURLOPT_MAXCONNECTS (3)
Protocol:
  - All
Added-in: 7.16.3
---

# NAME

CURLMOPT_MAXCONNECTS - size of connection cache

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLMcode curl_multi_setopt(CURLM *handle, CURLMOPT_MAXCONNECTS, long max);
~~~

# DESCRIPTION

Pass a long indicating the **max**, the maximum amount of connections that
libcurl may keep alive in its connection cache after use. By default libcurl
enlarges the size for each added easy handle to make it fit 4 times the number
of added easy handles.

By setting this option, you prevent the cache size from growing beyond the
limit set by you.

When the cache is full, curl closes the oldest connection present in the cache
to prevent the number of connections from increasing.

This option is for the multi handle's use only, when using the easy interface
you should instead use the CURLOPT_MAXCONNECTS(3) option.

See CURLMOPT_MAX_TOTAL_CONNECTIONS(3) for limiting the number of active
connections.

Changing this value when there are transfers in progress is possible, and the
new value is then used the next time checks are performed. Lowering the value
does not close down any active transfers, it simply does not allow new ones to
get made.

# DEFAULT

See DESCRIPTION

# %PROTOCOLS%

# EXAMPLE

~~~c
int main(void)
{
  CURLM *m = curl_multi_init();
  /* only keep 10 connections in the cache */
  curl_multi_setopt(m, CURLMOPT_MAXCONNECTS, 10L);
}
~~~

# %AVAILABILITY%

# RETURN VALUE

curl_multi_setopt(3) returns a CURLMcode indicating success or error.

CURLM_OK (0) means everything was OK, non-zero means an error occurred, see
libcurl-errors(3).
