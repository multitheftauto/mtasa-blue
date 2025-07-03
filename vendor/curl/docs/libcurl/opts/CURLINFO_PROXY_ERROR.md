---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLINFO_PROXY_ERROR
Section: 3
Source: libcurl
See-also:
  - CURLINFO_RESPONSE_CODE (3)
  - curl_easy_getinfo (3)
  - curl_easy_setopt (3)
  - libcurl-errors (3)
Protocol:
  - All
Added-in: 7.73.0
---

# NAME

CURLINFO_PROXY_ERROR - get the detailed (SOCKS) proxy error

# SYNOPSIS

~~~c
#include <curl/curl.h>

typedef enum {
  CURLPX_OK,
  CURLPX_BAD_ADDRESS_TYPE,
  CURLPX_BAD_VERSION,
  CURLPX_CLOSED,
  CURLPX_GSSAPI,
  CURLPX_GSSAPI_PERMSG,
  CURLPX_GSSAPI_PROTECTION,
  CURLPX_IDENTD,
  CURLPX_IDENTD_DIFFER,
  CURLPX_LONG_HOSTNAME,
  CURLPX_LONG_PASSWD,
  CURLPX_LONG_USER,
  CURLPX_NO_AUTH,
  CURLPX_RECV_ADDRESS,
  CURLPX_RECV_AUTH,
  CURLPX_RECV_CONNECT,
  CURLPX_RECV_REQACK,
  CURLPX_REPLY_ADDRESS_TYPE_NOT_SUPPORTED,
  CURLPX_REPLY_COMMAND_NOT_SUPPORTED,
  CURLPX_REPLY_CONNECTION_REFUSED,
  CURLPX_REPLY_GENERAL_SERVER_FAILURE,
  CURLPX_REPLY_HOST_UNREACHABLE,
  CURLPX_REPLY_NETWORK_UNREACHABLE,
  CURLPX_REPLY_NOT_ALLOWED,
  CURLPX_REPLY_TTL_EXPIRED,
  CURLPX_REPLY_UNASSIGNED,
  CURLPX_REQUEST_FAILED,
  CURLPX_RESOLVE_HOST,
  CURLPX_SEND_AUTH,
  CURLPX_SEND_CONNECT,
  CURLPX_SEND_REQUEST,
  CURLPX_UNKNOWN_FAIL,
  CURLPX_UNKNOWN_MODE,
  CURLPX_USER_REJECTED,
  CURLPX_LAST /* never use */
} CURLproxycode;

CURLcode curl_easy_getinfo(CURL *handle, CURLINFO_PROXY_ERROR, long *detail);
~~~

# DESCRIPTION

Pass a pointer to a long to receive a detailed error code when the most recent
transfer returned a **CURLE_PROXY** error. That error code matches the
**CURLproxycode** set.

The error code is zero (**CURLPX_OK**) if no response code was available.

# %PROTOCOLS%

# EXAMPLE

~~~c
int main(void)
{
  CURL *curl = curl_easy_init();
  if(curl) {
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");

    curl_easy_setopt(curl, CURLOPT_PROXY, "socks5://127.0.0.1");
    res = curl_easy_perform(curl);
    if(res == CURLE_PROXY) {
      long proxycode;
      res = curl_easy_getinfo(curl, CURLINFO_PROXY_ERROR, &proxycode);
      if(!res && proxycode)
        printf("The detailed proxy error: %ld\n", proxycode);
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
