find_path(IDEALITY_INCLUDE_DIR dideality.h ${CMAKE_SOURCE_DIR}/ext/ideality/lib)
find_library(IDEALITY_LIBRARY ideality PATH ${CMAKE_SOURCE_DIR}/ext/ideality/lib) 

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Ideality DEFAULT_MSG IDEALITY_LIBRARY IDEALITY_INCLUDE_DIR)

mark_as_advanced(IDEALITY_INCLUDE_DIR)
mark_as_advanced(IDEALITY_LIBRARY)
