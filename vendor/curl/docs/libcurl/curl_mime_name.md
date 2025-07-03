---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: curl_mime_name
Section: 3
Source: libcurl
See-also:
  - curl_mime_addpart (3)
  - curl_mime_data (3)
  - curl_mime_type (3)
Protocol:
  - HTTP
  - IMAP
  - SMTP
Added-in: 7.56.0
---

# NAME

curl_mime_name - set a mime part's name

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_mime_name(curl_mimepart *part, const char *name);
~~~

# DESCRIPTION

curl_mime_name(3) sets a mime part's name. This is the way HTTP form
fields are named.

*part* is the part's handle to assign a name to.

*name* points to the null-terminated name string.

The name string is copied into the part, thus the associated storage may
safely be released or reused after call. Setting a part's name multiple times
is valid: only the value set by the last call is retained. It is possible to
reset the name of a part by setting *name* to NULL.

# %PROTOCOLS%

# EXAMPLE

~~~c
int main(void)
{
  curl_mime *mime;
  curl_mimepart *part;

  CURL *curl = curl_easy_init();
  if(curl) {
    /* create a mime handle */
    mime = curl_mime_init(curl);

    /* add a part */
    part = curl_mime_addpart(mime);

    /* give the part a name */
    curl_mime_name(part, "shoe_size");
  }
}
~~~

# %AVAILABILITY%

# RETURN VALUE

This function returns a CURLcode indicating success or error.

CURLE_OK (0) means everything was OK, non-zero means an error occurred, see
libcurl-errors(3). If CURLOPT_ERRORBUFFER(3) was set with curl_easy_setopt(3)
there can be an error message stored in the error buffer when non-zero is
returned.
