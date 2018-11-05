cmake_minimum_required(VERSION 3.6)
project(gaus VERSION 1.0.0 LANGUAGES C)
add_compile_definitions(VERSION="-dirty")

find_package(PkgConfig)

#Debug settings:
set(CMAKE_C_FLAGS_DEBUG "-g -O0 ${GPROF} -fstack-protector-all")

#Release settings:
set(CMAKE_C_FLAGS_RELEASE "-O2 -fPIE -fstack-protector-all")
set(CMAKE_CXX_FLAGS_RELEASE "-D_FORTIFY_SOURCE=2")

#Address Sanitizer settings
if (CMAKE_BUILD_TYPE STREQUAL "Sanitize")
  message(STATUS "Address santitizer enabled")
  set(CMAKE_C_FLAGS_DEBUG "-g -fsanitize=address -O1 -fno-omit-frame-pointer")
endif ()


# Enable testing, but we want to show the output from google test when executing from console, so we
# add our own target. Execute with for instance `make check`.
enable_testing()
add_custom_target(check-old COMMAND GTEST_COLOR=1 ${CMAKE_CTEST_COMMAND} --verbose)
add_custom_target(check COMMAND GTEST_COLOR=1 ${CMAKE_CTEST_COMMAND} --verbose)

# Retrieve the standard GNU installation paths
include(GNUInstallDirs)
set(INSTALL_CMAKE_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/Gaus)

# Import the "download_project" function for downloading dependencies during the configure phase.
include(contrib/cmake-downloadProject/DownloadProject.cmake)


# Download googletest dependency during CMake configure time
download_project(
    PROJ googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.8.0
    GIT_SHALLOW true
    ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

# Add googletest directly to our build. This defines the gtest and gtest_main targets.
add_subdirectory(${googletest_SOURCE_DIR}
                 ${googletest_BINARY_DIR}
                 EXCLUDE_FROM_ALL)

# Download jansson dependency during CMake configure time
download_project(
    PROJ jansson
    GIT_REPOSITORY https://github.com/akheron/jansson.git
    GIT_TAG v2.11
    GIT_SHALLOW true
    ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

set(JANSSON_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(JANSSON_WITHOUT_TESTS ON CACHE BOOL "" FORCE)
set(JANSSON_EXAMPLES OFF CACHE BOOL "" FORCE)

# Add jansson directly to our build. This defines the jansson targets.
add_subdirectory(${jansson_SOURCE_DIR}
                 ${jansson_BINARY_DIR}
                 EXCLUDE_FROM_ALL)
include_directories(${jansson_BINARY_DIR}/include)

# Download libcurl dependency during CMake configure time
download_project(
    PROJ curl
    URL https://curl.haxx.se/download/curl-7.61.1.zip
    ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)
set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(ENABLE_MANUAL OFF CACHE BOOL "" FORCE)

# Add curl directly to our build. This defines the curl targets.
add_subdirectory(${curl_SOURCE_DIR}
                 ${curl_BINARY_DIR}
                 EXCLUDE_FROM_ALL)

include_directories(${curl_BINARY_DIR}/include)

# Prevent googletest from overriding the parent project's compiler/linker
# when building with Microsoft Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Prevent strict aliasing due to libev.
# For further discussion: http://lists.schmorp.de/pipermail/libev/2015q2/002540.html
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wextra -Wall -Wformat -Wformat-security -Wstrict-prototypes")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Werror -Wextra -Wall -Wformat -Wformat-security -Wstrict-prototypes")

add_subdirectory(src)
add_subdirectory(test)
