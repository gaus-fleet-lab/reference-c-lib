enable_language(CXX)
# Build our unittests executable.
#
# Test files are suffixed with "_test" to make sure all file names
# are unique in our project (makes it easier to find the correct file if you are working in a big
# project).
add_executable(unittests
               #test files:
               curl_mock.cpp curl_mock.h
               init_test.cpp
               register_test.cpp
               authenticate_test.cpp
               check_for_updates_test.cpp
               report_test.cpp
               unittest.cpp
               )

target_link_libraries(unittests Gaus::libgaus gtest)

# Execute "unittests" as part of "cmake" tests
add_test(NAME unittests COMMAND unittests)

target_compile_features(unittests PUBLIC cxx_std_11)


# Rebuild unittests before executing tests when starting with "check" target.
add_dependencies(check unittests)
