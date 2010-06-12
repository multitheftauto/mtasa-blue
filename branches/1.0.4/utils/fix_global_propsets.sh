#!/usr/bin/env sh
# Ascent 2007, Aim

# WARNING:
#  Running this script will wipe all previous svn:ignore propsets

if [ ! -f configure.ac ] ; then
  echo "You must run this script in the root of the trunk."
  exit 1
fi

echo "Updating eol-styles"

NEWLINE='
'
IFS=$NEWLINE

for i in `find . \
  -name "*.h" -o \
  -name "*.cpp" -o \
  -name "*.c" -o \
  -name "*.ini" -o \
  -name "*.txt" -o \
  -name "*.conf" -o \
  -name "configure.ac" -o \
  -name "*.txt" -o \
  -name "*.sh" -o \
  -name "*.sql" -o \
  -name "*.mk" -o \
  -name "Makefile.*" -o \
  -name "*.conf" -o \
  -name "*.xml" -o \
  -name "*.pl" -o \
  -name "*.php" -o \
  -name "*.patch" -o \
  -name "*.html"| grep -v \.svn/`; do
        EOLSTYLE=$(svn pg svn:eol-style $i)
        if [ $? -eq 0 ] && [ -z "$EOLSTYLE" ] ; then
		# first remove the returns just incase. (fucking windows)
		tr -d '\r' < $i > ${i}.tmp 
		mv ${i}.tmp $i
		svn ps svn:eol-style native $i 
        fi
done

