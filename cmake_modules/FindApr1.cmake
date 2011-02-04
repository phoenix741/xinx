find_package(PkgConfig)
pkg_check_modules(PC_APR1 apr-1 QUIET)
SET(APR1_DEFINITIONS ${PC_APR1_CFLAGS_OTHER})

find_path(APR1_INCLUDE_DIR NAMES apr.h
   HINTS
   ${PC_APR1_INCLUDEDIR}
   ${PC_APR1_INCLUDE_DIRS}
   )

find_library(APR1_LIBRARY NAMES apr-1
   HINTS
   ${PC_APR1_LIBDIR}
   ${PC_APR1_LIBRARY_DIRS}
   )

find_library(APRUTIL1_LIBRARY NAMES aprutil-1
   HINTS
   ${PC_APR1_LIBDIR}
   ${PC_APR1_LIBRARY_DIRS}
   )

set(APR1_LIBRARIES ${APR1_LIBRARY} ${APRUTIL1_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Apr1 DEFAULT_MSG APR1_LIBRARIES APR1_INCLUDE_DIR)

mark_as_advanced(APRUTIL1_LIBRARIES)
