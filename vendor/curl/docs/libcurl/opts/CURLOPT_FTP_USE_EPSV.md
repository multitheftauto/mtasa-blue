---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLOPT_FTP_USE_EPSV
Section: 3
Source: libcurl
Protocol:
  - FTP
See-also:
  - CURLOPT_FTPPORT (3)
  - CURLOPT_FTP_USE_EPRT (3)
Added-in: 7.9.2
---

# NAME

CURLOPT_FTP_USE_EPSV - use EPSV for FTP

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_easy_setopt(CURL *handle, CURLOPT_FTP_USE_EPSV, long epsv);
~~~

# DESCRIPTION

Pass *epsv* as a long. If the value is 1, it tells curl to use the EPSV
command when doing passive FTP downloads (which it does by default). Using
EPSV means that libcurl first attempts to use the EPSV command before using
PASV. If you pass zero to this option, it does not use EPSV, only plain PASV.

The EPSV command is a slightly newer addition to the FTP protocol than PASV
and is the preferred command to use since it enables IPv6 to be used. Old FTP
servers might not support it, which is why libcurl has a fallback mechanism.
Sometimes that fallback is not enough and then this option might come handy.

If the server is an IPv6 host, this option has no effect.

# DEFAULT

1

# %PROTOCOLS%

# EXAMPLE

~~~c
int main(void)
{
  CURL *curl = curl_easy_init();
  if(curl) {
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL,
                     "ftp://example.com/old-server/file.txt");

    /* let's shut off this modern feature */
    curl_easy_setopt(curl, CURLOPT_FTP_USE_EPSV, 0L);

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
