# We just try to figure out where hash<> is defined.  It's in some file
# that ends in hash_fun.h...
#
# Ideally we'd use AC_CACHE_CHECK, but that only lets us store one value
# at a time, and we need to store two (filename and namespace).
# prints messages itself, so we have to do the message-printing ourselves
# via AC_MSG_CHECKING + AC_MSG_RESULT.  (TODO(csilvers): can we cache?)
#
# stl_hash_fun.h: old gcc's (gc2.95?)
# ext/hash_fun.h: newer gcc's (gcc4)
# stl/_hash_fun.h: STLport

AC_DEFUN([AC_CXX_STL_HASH_FUN],
  [AC_REQUIRE([AC_CXX_NAMESPACES])
   AC_MSG_CHECKING(how to include hash_fun directly)
   AC_LANG_SAVE
   AC_LANG_CPLUSPLUS
   ac_cv_cxx_stl_hash_fun=""
   for location in ext/hash_fun.h ext/stl_hash_fun.h \
                   hash_fun.h stl_hash_fun.h \
                   stl/_hash_fun.h; do
     for namespace in __gnu_cxx "" std stdext; do
       if test -z "$ac_cv_cxx_stl_hash_fun"; then
         AC_TRY_COMPILE([#include <$location>],
                        [int x = ${namespace}::hash<int>()(5)],
                        [ac_cv_cxx_stl_hash_fun="<$location>";
                         ac_cv_cxx_hash_namespace="$namespace";])
       fi
     done
   done
   AC_LANG_RESTORE
   AC_DEFINE_UNQUOTED(HASH_FUN_H,$ac_cv_cxx_stl_hash_fun,
                      [the location of <hash_fun.h>/<stl_hash_fun.h>])
   AC_DEFINE_UNQUOTED(HASH_NAMESPACE,$ac_cv_cxx_hash_namespace,
                      [the namespace of hash_map/hash_set])
   AC_MSG_RESULT([$ac_cv_cxx_stl_hash_fun])
])
