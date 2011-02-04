find_path(QCODEEDIT_INCLUDE_DIR qce-config.h ${CMAKE_SOURCE_DIR}/ext/qcodeedit/lib)
find_library(QCODEEDIT_LIBRARY qcodeedit PATH ${CMAKE_SOURCE_DIR}/ext/qcodeedit) 

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QCodeEdit DEFAULT_MSG QCODEEDIT_LIBRARY QCODEEDIT_INCLUDE_DIR)

if (QCODEEDIT_FOUND)
  set(QCODEEDIT_INCLUDE_DIR ${QCODEEDIT_INCLUDE_DIR} ${QCODEEDIT_INCLUDE_DIR}/document ${QCODEEDIT_INCLUDE_DIR}/language ${QCODEEDIT_INCLUDE_DIR}/qnfa ${QCODEEDIT_INCLUDE_DIR}/widgets)
endif (QCODEEDIT_FOUND)


mark_as_advanced(QCODEEDIT_INCLUDE_DIR)
mark_as_advanced(QCODEEDIT_LIBRARY)
