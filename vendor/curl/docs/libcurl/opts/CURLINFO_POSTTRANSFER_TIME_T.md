---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLINFO_POSTTRANSFER_TIME_T
Section: 3
Source: libcurl
See-also:
  - CURLINFO_PRETRANSFER_TIME_T (3)
  - CURLOPT_TIMEOUT (3)
  - curl_easy_getinfo (3)
  - curl_easy_setopt (3)
Protocol:
  - All
Added-in: 8.10.0
---

# NAME

CURLINFO_POSTTRANSFER_TIME_T - get the time until the last byte is sent

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_easy_getinfo(CURL *handle, CURLINFO_POSTTRANSFER_TIME_T,
                           curl_off_t *timep);
~~~

# DESCRIPTION

Pass a pointer to a curl_off_t to receive the time, in microseconds,
it took from the start until the last byte is sent by libcurl.

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
    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
    res = curl_easy_perform(curl);
    if(CURLE_OK == res) {
      curl_off_t posttransfer;
      res = curl_easy_getinfo(curl, CURLINFO_POSTTRANSFER_TIME_T,
                              &posttransfer);
      if(CURLE_OK == res) {
        printf("Request sent after: %" CURL_FORMAT_CURL_OFF_T ".%06ld us",
               posttransfer / 1000000, (long)(posttransfer % 1000000));
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
