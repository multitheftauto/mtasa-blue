---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLOPT_LOGIN_OPTIONS
Section: 3
Source: libcurl
See-also:
  - CURLOPT_PASSWORD (3)
  - CURLOPT_USERNAME (3)
Protocol:
  - IMAP
  - LDAP
  - POP3
  - SMTP
Added-in: 7.34.0
---

# NAME

CURLOPT_LOGIN_OPTIONS - login options

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_easy_setopt(CURL *handle, CURLOPT_LOGIN_OPTIONS, char *options);
~~~

# DESCRIPTION

Pass a char pointer as parameter, which should be pointing to the
null-terminated *options* string to use for the transfer.

For more information about the login options please see RFC 2384, RFC 5092 and
the IETF draft **draft-earhart-url-smtp-00.txt**.

CURLOPT_LOGIN_OPTIONS(3) can be used to set protocol specific login options,
such as the preferred authentication mechanism via "AUTH=NTLM" or "AUTH=*",
and should be used in conjunction with the CURLOPT_USERNAME(3) option.

Since 8.2.0, IMAP supports the login option "AUTH=+LOGIN". With this option,
curl uses the plain (not SASL) LOGIN IMAP command even if the server
advertises SASL authentication. Care should be taken in using this option, as
it sends your password in plain text. This does not work if the IMAP server
disables the plain LOGIN (e.g. to prevent password snooping).

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
    curl_easy_setopt(curl, CURLOPT_URL, "smtp://example.com/");
    curl_easy_setopt(curl, CURLOPT_LOGIN_OPTIONS, "AUTH=*");
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }
}
~~~

# HISTORY

Support for OpenLDAP added in 7.82.0.

# %AVAILABILITY%

# RETURN VALUE

curl_easy_setopt(3) returns a CURLcode indicating success or error.

CURLE_OK (0) means everything was OK, non-zero means an error occurred, see
libcurl-errors(3).
