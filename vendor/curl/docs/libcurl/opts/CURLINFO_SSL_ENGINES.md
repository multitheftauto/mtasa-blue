---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLINFO_SSL_ENGINES
Section: 3
Source: libcurl
See-also:
  - CURLOPT_SSLENGINE (3)
  - curl_easy_getinfo (3)
  - curl_easy_setopt (3)
Protocol:
  - TLS
TLS-backend:
  - OpenSSL
Added-in: 7.12.3
---

# NAME

CURLINFO_SSL_ENGINES - get an slist of OpenSSL crypto-engines

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_easy_getinfo(CURL *handle, CURLINFO_SSL_ENGINES,
                           struct curl_slist **engine_list);
~~~

# DESCRIPTION

Pass the address of a 'struct curl_slist *' to receive a linked-list of
OpenSSL crypto-engines supported. Note that engines are normally implemented
in separate dynamic libraries. Hence not all the returned engines may be
available at runtime. **NOTE:** you must call curl_slist_free_all(3)
on the list pointer once you are done with it, as libcurl does not free this
data for you.

# %PROTOCOLS%

# EXAMPLE

~~~c
int main(void)
{
  CURL *curl = curl_easy_init();
  if(curl) {
    CURLcode res;
    struct curl_slist *engines;
    res = curl_easy_getinfo(curl, CURLINFO_SSL_ENGINES, &engines);
    if((res == CURLE_OK) && engines) {
      /* we have a list, free it when done using it */
      curl_slist_free_all(engines);
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
