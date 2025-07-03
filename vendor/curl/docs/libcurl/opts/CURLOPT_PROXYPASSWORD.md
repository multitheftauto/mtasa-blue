---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLOPT_PROXYPASSWORD
Section: 3
Source: libcurl
See-also:
  - CURLOPT_HTTPAUTH (3)
  - CURLOPT_PASSWORD (3)
  - CURLOPT_PROXYAUTH (3)
  - CURLOPT_PROXYUSERNAME (3)
Protocol:
  - All
Added-in: 7.19.1
---

# NAME

CURLOPT_PROXYPASSWORD - password to use with proxy authentication

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_easy_setopt(CURL *handle, CURLOPT_PROXYPASSWORD, char *pwd);
~~~

# DESCRIPTION

Pass a char pointer as parameter, which should be pointing to the
null-terminated password to use for authentication with the proxy.

The CURLOPT_PROXYPASSWORD(3) option should be used in conjunction with the
CURLOPT_PROXYUSERNAME(3) option.

The application does not have to keep the string around after setting this
option.

Using this option multiple times makes the last set string override the
previous ones. Set it to NULL to disable its use again.

# DEFAULT

blank

# %PROTOCOLS%

# EXAMPLE

~~~c
int main(void)
{
  CURL *curl = curl_easy_init();
  if(curl) {
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com/foo.bin");
    curl_easy_setopt(curl, CURLOPT_PROXY, "http://localhost:8080");
    curl_easy_setopt(curl, CURLOPT_PROXYUSERNAME, "mrsmith");
    curl_easy_setopt(curl, CURLOPT_PROXYPASSWORD, "qwerty");
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
