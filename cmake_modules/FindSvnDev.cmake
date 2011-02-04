find_package(Apr1)

find_path(SVNDEV_INCLUDE_DIR svn_auth.h /usr/include/subversion-1)
find_library(SVNCLIENT_LIBRARY NAMES svn_client-1)
find_library(SVNWC_LIBRARY NAMES svn_wc-1)
find_library(SVNRA_LIBRARY NAMES svn_ra-1)
find_library(SVNDELTA_LIBRARY NAMES svn_delta-1)
find_library(SVNSUBR_LIBRARY NAMES svn_subr-1)


set(SVNDEV_LIBRARIES ${SVNCLIENT_LIBRARY} ${SVNWC_LIBRARY} ${SVNRA_LIBRARY} ${SVNDELTA_LIBRARY} ${SVNSUBR_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SvnDev DEFAULT_MSG SVNDEV_LIBRARIES SVNDEV_INCLUDE_DIR)

mark_as_advanced(SVNDEV_LIBRARIES)
