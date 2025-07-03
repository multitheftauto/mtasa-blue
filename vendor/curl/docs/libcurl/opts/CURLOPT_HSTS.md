---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLOPT_HSTS
Section: 3
Source: libcurl
Protocol:
  - HTTP
See-also:
  - CURLOPT_ALTSVC (3)
  - CURLOPT_HSTS_CTRL (3)
  - CURLOPT_RESOLVE (3)
Added-in: 7.74.0
---

# NAME

CURLOPT_HSTS - HSTS cache filename

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_easy_setopt(CURL *handle, CURLOPT_HSTS, char *filename);
~~~

# DESCRIPTION

Make the *filename* point to a filename to load an existing HSTS cache
from, and to store the cache in when the easy handle is closed. Setting a file
name with this option also enables HSTS for this handle (the equivalent of
setting *CURLHSTS_ENABLE* with CURLOPT_HSTS_CTRL(3)).

If the given file does not exist or contains no HSTS entries at startup, the
HSTS cache simply starts empty. Setting the filename to NULL allows HSTS
without reading from or writing to any file. NULL also makes libcurl clear the
list of files to read HSTS data from, if any such were previously set.

If this option is set multiple times, libcurl loads cache entries from each
given file but only stores the last used name for later writing.

# FILE FORMAT

The HSTS cache is saved to and loaded from a text file with one entry per
physical line. Each line in the file has the following format:

    [host] [stamp]

[host] is the domain name for the entry and the name is dot-prefixed if it is
an entry valid for all subdomains to the name as well or only for the exact
name.

[stamp] is the time (in UTC) when the entry expires and it uses the format
"YYYYMMDD HH:MM:SS".

Lines starting with "#" are treated as comments and are ignored. There is
currently no length or size limit.

# DEFAULT

NULL, no filename

# SECURITY CONCERNS

libcurl cannot fully protect against attacks where an attacker has write
access to the same directory where it is directed to save files. This is
particularly sensitive if you save files using elevated privileges.

# %PROTOCOLS%

# EXAMPLE

~~~c
int main(void)
{
  CURL *curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_HSTS, "/home/user/.hsts-cache");
    curl_easy_perform(curl);
  }
}
~~~

# %AVAILABILITY%

# RETURN VALUE

curl_easy_setopt(3) returns a CURLcode indicating success or error.

CURLE_OK (0) means everything was OK, non-zero means an error occurred, see
libcurl-errors(3).
