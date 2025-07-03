---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLOPT_DISALLOW_USERNAME_IN_URL
Section: 3
Source: libcurl
See-also:
  - CURLOPT_PROTOCOLS_STR (3)
  - CURLOPT_URL (3)
  - curl_url_set (3)
  - libcurl-security (3)
Protocol:
  - All
Added-in: 7.61.0
---

# NAME

CURLOPT_DISALLOW_USERNAME_IN_URL - disallow specifying username in the URL

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_easy_setopt(CURL *handle, CURLOPT_DISALLOW_USERNAME_IN_URL,
                          long disallow);
~~~

# DESCRIPTION

A long parameter set to 1 tells the library to not allow URLs that include a
username.

This is the equivalent to the *CURLU_DISALLOW_USER* flag for the
curl_url_set(3) function.

# DEFAULT

0 (disabled)

# %PROTOCOLS%

# EXAMPLE

~~~c
int main(void)
{
  CURL *curl = curl_easy_init();
  if(curl) {

    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
    curl_easy_setopt(curl, CURLOPT_DISALLOW_USERNAME_IN_URL, 1L);

    curl_easy_perform(curl);
  }
}
~~~

# %AVAILABILITY%

# RETURN VALUE

curl_easy_setopt(3) returns a CURLcode indicating success or error.

CURLE_OK (0) means everything was OK, non-zero means an error occurred, see
libcurl-errors(3).

curl_easy_perform(3) returns CURLE_LOGIN_DENIED if this option is
enabled and a URL containing a username is specified.
