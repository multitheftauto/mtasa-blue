---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLOPT_TELNETOPTIONS
Section: 3
Source: libcurl
See-also:
  - CURLOPT_HTTPHEADER (3)
  - CURLOPT_QUOTE (3)
Protocol:
  - TELNET
Added-in: 7.7
---

# NAME

CURLOPT_TELNETOPTIONS - set of telnet options

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_easy_setopt(CURL *handle, CURLOPT_TELNETOPTIONS,
                          struct curl_slist *cmds);
~~~

# DESCRIPTION

Provide a pointer to a curl_slist with variables to pass to the telnet
negotiations. The variables should be in the format \<option=value\>. libcurl
supports the options **TTYPE**, **XDISPLOC** and **NEW_ENV**. See the TELNET
standard for details.

Using this option multiple times makes the last set list override the previous
ones. Set it to NULL to disable its use again.

libcurl does not copy the list, it needs to be kept around until after the
transfer has completed.

# DEFAULT

NULL

# %PROTOCOLS%

# EXAMPLE

~~~c
int main(void)
{
  CURL *curl = curl_easy_init();
  if(curl) {
    CURLcode res;
    struct curl_slist *options;
    options = curl_slist_append(NULL, "TTTYPE=vt100");
    options = curl_slist_append(options, "USER=foobar");
    curl_easy_setopt(curl, CURLOPT_URL, "telnet://example.com/");
    curl_easy_setopt(curl, CURLOPT_TELNETOPTIONS, options);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_slist_free_all(options);
  }
}
~~~

# %AVAILABILITY%

# RETURN VALUE

curl_easy_setopt(3) returns a CURLcode indicating success or error.

CURLE_OK (0) means everything was OK, non-zero means an error occurred, see
libcurl-errors(3).
