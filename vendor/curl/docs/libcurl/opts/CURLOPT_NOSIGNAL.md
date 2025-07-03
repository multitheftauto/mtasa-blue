---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLOPT_NOSIGNAL
Section: 3
Source: libcurl
See-also:
  - CURLOPT_TIMEOUT (3)
Protocol:
  - All
Added-in: 7.10
---

# NAME

CURLOPT_NOSIGNAL - skip all signal handling

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_easy_setopt(CURL *handle, CURLOPT_NOSIGNAL, long onoff);
~~~

# DESCRIPTION

If *onoff* is 1, libcurl uses no functions that install signal handlers or
any functions that cause signals to be sent to the process. This option is
here to allow multi-threaded Unix applications to still set/use all timeout
options etc, without risking getting signals.

If this option is set and libcurl has been built with the standard name
resolver, timeouts cannot occur while the name resolve takes place. Consider
building libcurl with the c-ares or threaded resolver backends to enable
asynchronous DNS lookups, to enable timeouts for name resolves without the use
of signals.

Setting CURLOPT_NOSIGNAL(3) to 1 makes libcurl NOT ask the system to
ignore SIGPIPE signals, which otherwise are sent by the system when trying to
send data to a socket which is closed in the other end. libcurl makes an
effort to never cause such SIGPIPE signals to trigger, but some operating
systems have no way to avoid them and even on those that have there are some
corner cases when they may still happen, contrary to our desire.

# DEFAULT

0

# %PROTOCOLS%

# EXAMPLE

~~~c
int main(void)
{
  CURL *curl = curl_easy_init();
  if(curl) {
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com/");

    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

    res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
  }
}
~~~

# %AVAILABILITY%

# RETURN VALUE

curl_easy_setopt(3) returns a CURLcode indicating success or error.

CURLE_OK (0) means everything was OK, non-zero means an error occurred, see
libcurl-errors(3).
