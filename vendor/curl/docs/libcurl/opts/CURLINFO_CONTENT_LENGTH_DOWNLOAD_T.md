---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLINFO_CONTENT_LENGTH_DOWNLOAD_T
Section: 3
Source: libcurl
See-also:
  - CURLINFO_CONTENT_LENGTH_UPLOAD_T (3)
  - curl_easy_getinfo (3)
  - curl_easy_setopt (3)
Protocol:
  - HTTP
Added-in: 7.55.0
---

# NAME

CURLINFO_CONTENT_LENGTH_DOWNLOAD_T - get content-length of download

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_easy_getinfo(CURL *handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T,
                           curl_off_t *content_length);
~~~

# DESCRIPTION

Pass a pointer to a *curl_off_t* to receive the content-length of the
download. This is the value read from the Content-Length: field. Stores -1 if
the size is not known.

# %PROTOCOLS%

# EXAMPLE

~~~c
int main(void)
{
  CURL *curl = curl_easy_init();
  if(curl) {
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");

    /* Perform the request */
    res = curl_easy_perform(curl);

    if(!res) {
      /* check the size */
      curl_off_t cl;
      res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &cl);
      if(!res) {
        printf("Download size: %" CURL_FORMAT_CURL_OFF_T "\n", cl);
      }
    }
  }
}
~~~

# %AVAILABILITY%

# RETURN VALUE

curl_easy_getinfo(3) returns a CURLcode indicating success or error.

CURLE_OK (0) means everything was OK, non-zero means an error occurred, see
libcurl-errors(3).
