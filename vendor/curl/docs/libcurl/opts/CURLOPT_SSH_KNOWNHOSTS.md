---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLOPT_SSH_KNOWNHOSTS
Section: 3
Source: libcurl
See-also:
  - CURLOPT_SSH_AUTH_TYPES (3)
  - CURLOPT_SSH_HOST_PUBLIC_KEY_MD5 (3)
  - CURLOPT_SSH_HOST_PUBLIC_KEY_SHA256 (3)
Protocol:
  - SFTP
  - SCP
Added-in: 7.19.6
---

# NAME

CURLOPT_SSH_KNOWNHOSTS - filename holding the SSH known hosts

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_easy_setopt(CURL *handle, CURLOPT_SSH_KNOWNHOSTS, char *fname);
~~~

# DESCRIPTION

Pass a pointer to a null-terminated string holding the filename of the
known_host file to use. The known_hosts file should use the OpenSSH file
format. If this file is specified, libcurl only accepts connections with hosts
that are known and present in that file, with a matching public key. Use
CURLOPT_SSH_KEYFUNCTION(3) to alter the default behavior on host and key
matches and mismatches.

We strongly suggest users doing SCP or SFTP transfers to set this option to
make sure that the network communication is done with the intended server and
not an impostor.

The application does not have to keep the string around after setting this
option.

Using this option multiple times makes the last set string override the
previous ones. Set it to NULL to disable its use again.

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
    curl_easy_setopt(curl, CURLOPT_URL, "sftp://example.com/file");
    curl_easy_setopt(curl, CURLOPT_SSH_KNOWNHOSTS,
                     "/home/clarkkent/.ssh/known_hosts");
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
