---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLOPT_TCP_KEEPALIVE
Section: 3
Source: libcurl
See-also:
  - CURLOPT_LOW_SPEED_LIMIT (3)
  - CURLOPT_MAX_RECV_SPEED_LARGE (3)
  - CURLOPT_TCP_KEEPIDLE (3)
  - CURLOPT_TCP_KEEPINTVL (3)
  - CURLOPT_TCP_KEEPCNT (3)
Protocol:
  - TCP
Added-in: 7.25.0
---

# NAME

CURLOPT_TCP_KEEPALIVE - TCP keep-alive probing

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_easy_setopt(CURL *handle, CURLOPT_TCP_KEEPALIVE, long probe);
~~~

# DESCRIPTION

Pass a long. If set to 1, TCP keepalive probes are used. The delay and
frequency of these probes can be controlled by the
CURLOPT_TCP_KEEPIDLE(3), CURLOPT_TCP_KEEPINTVL(3), and CURLOPT_TCP_KEEPCNT(3)
options, provided the operating system supports them. Set to 0 (default behavior)
to disable keepalive probes.

# DEFAULT

0

# %PROTOCOLS%

# EXAMPLE

~~~c
int main(void)
{
  CURL *curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");

    /* enable TCP keep-alive for this transfer */
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

    /* keep-alive idle time to 120 seconds */
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 120L);

    /* interval time between keep-alive probes: 60 seconds */
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 60L);

    /* maximum number of keep-alive probes: 3 */
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPCNT, 3L);

    curl_easy_perform(curl);
  }
}
~~~

# %AVAILABILITY%

# RETURN VALUE

curl_easy_setopt(3) returns a CURLcode indicating success or error.

CURLE_OK (0) means everything was OK, non-zero means an error occurred, see
libcurl-errors(3).
