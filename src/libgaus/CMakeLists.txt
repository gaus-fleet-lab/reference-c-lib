add_library(libgaus
            ../include/gaus/gaus_client_report_types.h
            ../include/gaus/gaus_client.h
            ../include/gaus/gaus_client_types.h
            curl_wrapper.c curl_wrapper.h
            gaus.c
            gaus_register.c
            gaus_authenticate.c
            gaus_check_for_updates.c
            gaus_report.c
            request.c request.h
            log.c log.h
            gaus_json_helpers.c gaus_json_helpers.h
            )

# CMake automatically prefixes our target name with "lib" for libraries, i.e. the built target
# will by default be called "liblibgaus.a". We adjust the library name so it becomes
# "libgaus.a" for cosmetic reasons.
set_target_properties(libgaus PROPERTIES PREFIX "")

# This adds the current source directory as an include directory for consumers of our library.
# If we have generated headers we would also add CMAKE_CURRENT_BINARY_DIR.
target_include_directories(libgaus PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/../include
                           $<INSTALL_INTERFACE:include>
                           )

target_link_libraries(libgaus libcurl jansson)

# Add a target in our namespace
add_library(Gaus::libgaus ALIAS libgaus)

# Install libgaus.so target
install(TARGETS libgaus
        EXPORT libgaus-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

# Write an GausConfig.cmake file that can be used by `find_package` when some dependent
# project wants to use our library.
# `find_package` requires the file to have `Config.cmake` suffix. If we need to do more than just
# exporting some variables (e.g. do `find_package` on our other dependencies) then we could write
# to `GausTargets.cmake` instead and generate `GausConfig.cmake` with
# `configure_package_config_file` and include `GausTargets.cmake` from
# `GausConfig.cmake`.
install(
    EXPORT libgaus-targets
    FILE GausConfig.cmake
    NAMESPACE Gaus::
    DESTINATION ${INSTALL_CMAKE_CONFIGDIR}
)
