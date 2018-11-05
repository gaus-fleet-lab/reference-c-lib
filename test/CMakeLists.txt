#The MIT License (MIT)
#
#Copyright 2018, Sony Mobile Communications Inc.
#
#Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
