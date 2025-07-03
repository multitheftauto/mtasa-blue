---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLOPT_USERAGENT
Section: 3
Source: libcurl
See-also:
  - CURLOPT_CUSTOMREQUEST (3)
  - CURLOPT_HTTPHEADER (3)
  - CURLOPT_REFERER (3)
  - CURLOPT_REQUEST_TARGET (3)
Protocol:
  - HTTP
Added-in: 7.1
---

# NAME

CURLOPT_USERAGENT - HTTP user-agent header

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_easy_setopt(CURL *handle, CURLOPT_USERAGENT, char *ua);
~~~

# DESCRIPTION

Pass a pointer to a null-terminated string as parameter. It is used to set the
User-Agent: header field in the HTTP request sent to the remote server. You
can also set any custom header with CURLOPT_HTTPHEADER(3).

The application does not have to keep the string around after setting this
option.

Using this option multiple times makes the last set string override the
previous ones. Set it to NULL to disable its use again.

# DEFAULT

NULL, no User-Agent: header is used.

# %PROTOCOLS%

# EXAMPLE

~~~c
int main(void)
{
  CURL *curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");

    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Dark Secret Ninja/1.0");

    curl_easy_perform(curl);
  }
}
~~~

# %AVAILABILITY%

# RETURN VALUE

curl_easy_setopt(3) returns a CURLcode indicating success or error.

CURLE_OK (0) means everything was OK, non-zero means an error occurred, see
libcurl-errors(3).
