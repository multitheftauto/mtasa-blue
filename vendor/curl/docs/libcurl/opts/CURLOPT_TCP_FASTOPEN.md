---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLOPT_TCP_FASTOPEN
Section: 3
Source: libcurl
See-also:
  - CURLOPT_SSL_FALSESTART (3)
Protocol:
  - TCP
Added-in: 7.49.0
---

# NAME

CURLOPT_TCP_FASTOPEN - TCP Fast Open

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_easy_setopt(CURL *handle, CURLOPT_TCP_FASTOPEN, long enable);
~~~

# DESCRIPTION

Pass a long as parameter set to 1L to enable or 0 to disable.

TCP Fast Open (RFC 7413) is a mechanism that allows data to be carried in the
SYN and SYN-ACK packets and consumed by the receiving end during the initial
connection handshake, saving up to one full round-trip time (RTT).

Beware: the TLS session cache does not work when TCP Fast Open is enabled. TCP
Fast Open is also known to be problematic on or across certain networks.

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
    curl_easy_setopt(curl, CURLOPT_TCP_FASTOPEN, 1L);
    curl_easy_perform(curl);
  }
}
~~~

# NOTES

This option is only supported on Linux and macOS 10.11 or later.

# %AVAILABILITY%

# RETURN VALUE

curl_easy_setopt(3) returns a CURLcode indicating success or error.

CURLE_OK (0) means everything was OK, non-zero means an error occurred, see
libcurl-errors(3).
