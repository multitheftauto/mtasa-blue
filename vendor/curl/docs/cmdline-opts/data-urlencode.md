---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Long: data-urlencode
Arg: <data>
Help: HTTP POST data URL encoded
Protocols: HTTP
Added: 7.18.0
Category: http post upload
Multi: append
See-also:
  - data
  - data-raw
Example:
  - --data-urlencode name=val $URL
  - --data-urlencode =encodethis $URL
  - --data-urlencode name@file $URL
  - --data-urlencode @fileonly $URL
---

# `--data-urlencode`

Post data, similar to the other --data options with the exception that this
performs URL-encoding.

To be CGI-compliant, the \<data\> part should begin with a *name* followed by
a separator and a content specification. The \<data\> part can be passed to
curl using one of the following syntaxes:

## content
URL-encode the content and pass that on. Just be careful so that the content
does not contain any `=` or `@` symbols, as that makes the syntax match one of
the other cases below.

## =content
URL-encode the content and pass that on. The preceding `=` symbol is not
included in the data.

## name=content
URL-encode the content part and pass that on. Note that the name part is
expected to be URL-encoded already.

## @filename
load data from the given file (including any newlines), URL-encode that data
and pass it on in the POST. Using `@-` makes curl read the data from stdin.

## name@filename
load data from the given file (including any newlines), URL-encode that data
and pass it on in the POST. The name part gets an equal sign appended,
resulting in *name=urlencoded-file-content*. Note that the name is expected to
be URL-encoded already.
