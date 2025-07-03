---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Title: CURLINFO_RTSP_SERVER_CSEQ
Section: 3
Source: libcurl
See-also:
  - CURLINFO_RTSP_CSEQ_RECV (3)
  - curl_easy_getinfo (3)
  - curl_easy_setopt (3)
Protocol:
  - RTSP
Added-in: 7.20.0
---

# NAME

CURLINFO_RTSP_SERVER_CSEQ - get the next RTSP server CSeq

# SYNOPSIS

~~~c
#include <curl/curl.h>

CURLcode curl_easy_getinfo(CURL *handle, CURLINFO_RTSP_SERVER_CSEQ,
                           long *cseq);
~~~

# DESCRIPTION

Pass a pointer to a long to receive the next CSeq that is expected to be used
by the application.

Listening for server initiated requests is not implemented.

Applications wishing to resume an RTSP session on another connection should
retrieve this info before closing the active connection.

# %PROTOCOLS%

# EXAMPLE

~~~c
int main(void)
{
  CURL *curl = curl_easy_init();
  if(curl) {
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, "rtsp://rtsp.example.com");
    res = curl_easy_perform(curl);
    if(res == CURLE_OK) {
      long cseq;
      curl_easy_getinfo(curl, CURLINFO_RTSP_SERVER_CSEQ, &cseq);
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
