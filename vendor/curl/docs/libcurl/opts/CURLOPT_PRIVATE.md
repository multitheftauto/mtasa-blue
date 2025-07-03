---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLOPT_PRIVATE
Section: 3
Source: libcurl
See-also:
  - CURLINFO_PRIVATE (3)
  - CURLOPT_STDERR (3)
  - CURLOPT_VERBOSE (3)
Protocol:
  - All
Added-in: 7.10.3
---

# NAME

CURLOPT_PRIVATE - store a private pointer

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_easy_setopt(CURL *handle, CURLOPT_PRIVATE, void *pointer);
~~~

# DESCRIPTION

Pass a void * as parameter, pointing to data that should be associated with
this curl handle. The pointer can subsequently be retrieved using
curl_easy_getinfo(3) with the CURLINFO_PRIVATE(3) option. libcurl itself
never does anything with this data.

# DEFAULT

NULL

# %PROTOCOLS%

# EXAMPLE

~~~c
struct private {
  void *custom;
};

int main(void)
{
  CURL *curl = curl_easy_init();
  struct private secrets;
  if(curl) {
    struct private *extracted;
    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");

    /* store a pointer to our private struct */
    curl_easy_setopt(curl, CURLOPT_PRIVATE, &secrets);

    curl_easy_perform(curl);

    /* we can extract the private pointer again too */
    curl_easy_getinfo(curl, CURLINFO_PRIVATE, &extracted);
  }
}
~~~

# %AVAILABILITY%

# RETURN VALUE

curl_easy_setopt(3) returns a CURLcode indicating success or error.

CURLE_OK (0) means everything was OK, non-zero means an error occurred, see
libcurl-errors(3).
