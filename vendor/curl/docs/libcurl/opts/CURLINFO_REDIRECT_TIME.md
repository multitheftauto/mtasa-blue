---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLINFO_REDIRECT_TIME
Section: 3
Source: libcurl
See-also:
  - CURLINFO_REDIRECT_COUNT (3)
  - CURLINFO_REDIRECT_TIME_T (3)
  - CURLINFO_REDIRECT_URL (3)
  - curl_easy_getinfo (3)
  - curl_easy_setopt (3)
Protocol:
  - HTTP
Added-in: 7.9.7
---

# NAME

CURLINFO_REDIRECT_TIME - get the time for all redirection steps

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_easy_getinfo(CURL *handle, CURLINFO_REDIRECT_TIME,
                           double *timep);
~~~

# DESCRIPTION

Pass a pointer to a double to receive the total time, in seconds, it took for
all redirection steps include name lookup, connect, pretransfer and transfer
before final transaction was started. CURLINFO_REDIRECT_TIME(3) contains
the complete execution time for multiple redirections.

See also the TIMES overview in the curl_easy_getinfo(3) man page.

# %PROTOCOLS%

# EXAMPLE

~~~c
int main(void)
{
  CURL *curl = curl_easy_init();
  if(curl) {
    CURLcode res;
    double redirect;
    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
    res = curl_easy_perform(curl);
    if(CURLE_OK == res) {
      res = curl_easy_getinfo(curl, CURLINFO_REDIRECT_TIME, &redirect);
      if(CURLE_OK == res) {
        printf("Time: %.1f", redirect);
      }
    }
    /* always cleanup */
    curl_easy_cleanup(curl);
  }
}
~~~

# %AVAILABILITY%

# RETURN VALUE

curl_easy_getinfo(3) returns a CURLcode indicating success or error.

CURLE_OK (0) means everything was OK, non-zero means an error occurred, see
libcurl-errors(3).
