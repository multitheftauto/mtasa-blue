#!/usr/bin/env sh
# Summit? 2008, Aim

# WARNING:
#  Running this script will wipe all previous svn:ignore propsets

if [ ! -f configure.ac ] ; then
  echo "You must run this script in the root of the trunk."
  exit 1
fi

echo "Updating svn:ignore"
for i in ./ $(svn -R list | grep /\$); do
        if [ -f $i/.svn.ignore ] ; then
                echo "Merging ${i}.svn.ignore with global"
                cat ${i}.svn.ignore .svn.ignore.global | sort  | uniq > ${i}.svn.ignore
								svn propget svn:ignore $i | diff -Bq - ${i}.svn.ignore
								if [ $? -ne 0 ] ; then 
									echo Propset differs, updating...
                	svn propset svn:ignore -F ${i}.svn.ignore $i
									echo
								fi
        else
								svn propget svn:ignore $i | diff -Bq - .svn.ignore.global
								if [ $? -ne 0 ] ; then
									echo Propset differs with global, updating propset
                	svn propset svn:ignore -F .svn.ignore.global $i
									echo
								fi
        fi
done

