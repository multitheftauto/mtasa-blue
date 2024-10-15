#  OPUS_FOUND - system has opus
#  OPUS_INCLUDE_DIRS - the opus include directory
#  OPUS_LIBRARIES - The libraries needed to use opus

find_path(OPUS_INCLUDE_DIRS
	NAMES opus/opus.h
	PATH_SUFFIXES include
)
if(OPUS_INCLUDE_DIRS)
	set(HAVE_OPUS_OPUS_H 1)
endif()

if(OPUS_USE_STATIC_LIBS)
	find_library(OPUS_LIBRARIES NAMES "libopus.a")
else()
	find_library(OPUS_LIBRARIES NAMES opus)
endif()

if(OPUS_LIBRARIES)
	if(OPUS_USE_STATIC_LIBS)
		find_library(LIBM NAMES "libm.a" "libm.tbd")
	else()
		find_library(LIBM NAMES m)
	endif()
	if(LIBM)
		list(APPEND OPUS_LIBRARIES ${LIBM})
	endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Opus
	DEFAULT_MSG
	OPUS_INCLUDE_DIRS OPUS_LIBRARIES HAVE_OPUS_OPUS_H
)

mark_as_advanced(OPUS_INCLUDE_DIRS OPUS_LIBRARIES HAVE_OPUS_OPUS_H)
