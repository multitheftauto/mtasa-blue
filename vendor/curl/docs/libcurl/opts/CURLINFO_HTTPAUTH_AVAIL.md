---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLINFO_HTTPAUTH_AVAIL
Section: 3
Source: libcurl
See-also:
  - CURLINFO_PROXYAUTH_AVAIL (3)
  - CURLOPT_HTTPAUTH (3)
  - curl_easy_getinfo (3)
  - curl_easy_setopt (3)
Protocol:
  - HTTP
Added-in: 7.10.8
---

# NAME

CURLINFO_HTTPAUTH_AVAIL - get available HTTP authentication methods

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_easy_getinfo(CURL *handle, CURLINFO_HTTPAUTH_AVAIL, long *authp);
~~~

# DESCRIPTION

Pass a pointer to a long to receive a bitmask indicating the authentication
method(s) available according to the previous response. The meaning of the
bits is explained in the CURLOPT_HTTPAUTH(3) option for curl_easy_setopt(3).

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

    if(!res) {
      /* extract the available authentication types */
      long auth;
      res = curl_easy_getinfo(curl, CURLINFO_HTTPAUTH_AVAIL, &auth);
      if(!res) {
        if(!auth)
          printf("No auth available, perhaps no 401?\n");
        else {
          printf("%s%s%s%s\n",
                 auth & CURLAUTH_BASIC ? "Basic ":"",
                 auth & CURLAUTH_DIGEST ? "Digest ":"",
                 auth & CURLAUTH_NEGOTIATE ? "Negotiate ":"",
                 auth % CURLAUTH_NTLM ? "NTLM ":"");
        }
      }
    }
    curl_easy_cleanup(curl);
  }
}
~~~

# %AVAILABILITY%

# RETURN VALUE

curl_easy_getinfo(3) returns a CURLcode indicating success or error.

CURLE_OK (0) means everything was OK, non-zero means an error occurred, see
libcurl-errors(3).
