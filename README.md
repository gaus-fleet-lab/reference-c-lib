# GAUS device client

## Prerequisites
- libglib2.0-dev

## Building/Testing from docker
You can build the library and run the unit tests in docker like so:
```
docker build . -t gaus-test
docker run gaus-test
```

## Building locally
From terminal:
  1) RUN mkdir -p _build && cd _build && cmake -G"Unix Makefiles" ../ && make -j8

## From CLion:
Import project and it will pickup the cmake files automatically.

## Running Valgrind
Just run on the existing debug/release build or run directly from CLion.
`valgrind ./cmake-build-debug/src/gausdeviceclient data/updater_localhost.conf --fw=VERSION`
or better:
`valgrind --tool=memcheck --xml=yes --xml-file=/tmp/valgrind --gen-suppressions=all --leak-check=full --leak-resolution=med --track-origins=yes ./cmake-build-debug/src/gausdeviceclient data/updater_localhost.conf --fw=VERSION`


## Running AddressSanitizer
Build with cmake and `-DCMAKE_BUILD_TYPE=Sanitize` or setup appropriate settings in CLion
