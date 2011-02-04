find_package(SvnDev)

find_path(SVNCPP_INCLUDE_DIR context_listener.hpp /usr/include/svncpp)
find_library(SVNCPP_LIBRARY svncpp)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SvnCpp DEFAULT_MSG SVNCPP_LIBRARY SVNCPP_INCLUDE_DIR)
