#! /bin/false
#
# Copyright (C) 2003 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#

# Find a way to echo strings without interpreting backslash.
if test "X`(echo '\t') 2>/dev/null`" = 'X\t'; then
  echo='echo'
else
  if test "X`(printf '%s\n' '\t') 2>/dev/null`" = 'X\t'; then
    echo='printf %s\n'
  else
    echo_func () {
      cat <<EOT
$*
EOT
    }
    echo='echo_func'
  fi
fi

# eval_gettext MSGID
# looks up the translation of MSGID and substitutes shell variables in the
# result.
eval_gettext () {
  gettext "$1" | (export PATH `envsubst --variables "$1"`; envsubst "$1")
}

# eval_ngettext MSGID MSGID-PLURAL COUNT
# looks up the translation of MSGID / MSGID-PLURAL for COUNT and substitutes
# shell variables in the result.
eval_ngettext () {
  ngettext "$1" "$2" "$3" | (export PATH `envsubst --variables "$1 $2"`; envsubst "$1 $2")
}

# Note: This use of envsubst is much safer than using the shell built-in 'eval'
# would be.
# 1) The security problem with Chinese translations that happen to use a
#    character such as \xe0\x60 is avoided.
# 2) The security problem with malevolent translators who put in command lists
#    like "$(...)" or "`...`" is avoided.
# 3) The translations can only refer to shell variables that are already
#    mentioned in MSGID or MSGID-PLURAL.
#
# Note: "export PATH" above is a dummy; this is for the case when
# `envsubst --variables ...` returns nothing.
#
# Note: In eval_ngettext above, "$1 $2" means a string whose variables set is
# the union of the variables set of "$1" and "$2".
#
# Note: The minimal use of backquote above ensures that trailing newlines are
# not dropped, not from the gettext invocation and not from the value of any
# shell variable.
#
# Note: Field splitting on the `envsubst --variables ...` result is desired,
# since envsubst outputs the variables, separated by newlines. Pathname
# wildcard expansion or tilde expansion has no effect here, since the words
# output by "envsubst --variables ..." consist solely of alphanumeric
# characters and underscore.
