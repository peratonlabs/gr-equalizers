INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_EQUALIZERS equalizers)

FIND_PATH(
    EQUALIZERS_INCLUDE_DIRS
    NAMES equalizers/api.h
    HINTS $ENV{EQUALIZERS_DIR}/include
        ${PC_EQUALIZERS_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    EQUALIZERS_LIBRARIES
    NAMES gnuradio-equalizers
    HINTS $ENV{EQUALIZERS_DIR}/lib
        ${PC_EQUALIZERS_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/equalizersTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(EQUALIZERS DEFAULT_MSG EQUALIZERS_LIBRARIES EQUALIZERS_INCLUDE_DIRS)
MARK_AS_ADVANCED(EQUALIZERS_LIBRARIES EQUALIZERS_INCLUDE_DIRS)
