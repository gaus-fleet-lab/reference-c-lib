add_subdirectory(libgaus)

# Copy public headers during `make install`
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
