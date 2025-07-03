---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLINFO_TOTAL_TIME
Section: 3
Source: libcurl
See-also:
  - CURLINFO_TOTAL_TIME_T (3)
  - CURLOPT_TIMEOUT (3)
  - curl_easy_getinfo (3)
  - curl_easy_setopt (3)
Protocol:
  - All
Added-in: 7.4.1
---

# NAME

CURLINFO_TOTAL_TIME - get total time of previous transfer

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_easy_getinfo(CURL *handle, CURLINFO_TOTAL_TIME, double *timep);
~~~

# DESCRIPTION

Pass a pointer to a double to receive the total time in seconds for the
previous transfer, including name resolving, TCP connect etc. The double
represents the time in seconds, including fractions.

When a redirect is followed, the time from each request is added together.

See also the TIMES overview in the curl_easy_getinfo(3) man page.

# %PROTOCOLS%

# EXAMPLE

~~~c
int main(void)
{
  CURL *curl = curl_easy_init();
  if(curl) {
    CURLcode res;
    double total;
    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com/");
    res = curl_easy_perform(curl);
    if(CURLE_OK == res) {
      res = curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total);
      if(CURLE_OK == res) {
        printf("Time: %.1f", total);
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
