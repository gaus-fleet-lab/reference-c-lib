# GAUS device client

[![Build Status](https://codebuild.eu-west-1.amazonaws.com/badges?uuid=eyJlbmNyeXB0ZWREYXRhIjoiSEZiVnYyMEVKdTFaTU9rRnJabDZYNE5oZnBKU08wbVlXYUZpendvQkdDbDhNOXAzaHgwUERmaVpMemFJcmtUdFk5TXFaTDZ5UnNTaUwvMkF3ZlVEK1VFPSIsIml2UGFyYW1ldGVyU3BlYyI6IndEZ280NTczcEZmMXhUNzgiLCJtYXRlcmlhbFNldFNlcmlhbCI6MX0%3D&branch=master)](https://eu-west-1.console.aws.amazon.com/codesuite/codebuild/projects/c-cpp-demo/details)

A reference library to be used to communicate with [Gaus Fleet Lab](https://gaus.incubation.io/), the automated,
scalable, and secure way to manage a connected device fleet.

## Building/Testing from docker
You can build the library and run the unit tests in docker like so:
```
docker build . -t gaus-test
docker run gaus-test
```

## Building locally
From terminal:
  1) RUN mkdir -p _build && cd _build && cmake -G"Unix Makefiles" ../ && make -j8

## Working from CLion:
Import project and it will pickup the cmake files automatically.

## Running Valgrind
Just run on the existing debug/release build or run directly from CLion.
`valgrind ./cmake-build-debug/src/gausdeviceclient data/updater_localhost.conf --fw=VERSION`
or better:
`valgrind --tool=memcheck --xml=yes --xml-file=/tmp/valgrind --gen-suppressions=all --leak-check=full --leak-resolution=med --track-origins=yes ./cmake-build-debug/src/gausdeviceclient data/updater_localhost.conf --fw=VERSION`


## Running AddressSanitizer
Build with cmake and `-DCMAKE_BUILD_TYPE=Sanitize` or setup appropriate settings in CLion
