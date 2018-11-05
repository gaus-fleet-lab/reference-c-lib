#The MIT License (MIT)
 #
 #Copyright 2018, Sony Mobile Communications Inc.
 #
 #Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 #
 #The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 #
 #THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
FROM gcc:7.2.0 as build
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    git unzip
RUN mkdir -p /cmake/ \
    && cd /cmake/ \
    && wget -nc https://cmake.org/files/v3.12/cmake-3.12.4-Linux-x86_64.sh \
    && chmod +x cmake-3.12.4-Linux-x86_64.sh \
    && mkdir cmake \
    && ./cmake-3.12.4-Linux-x86_64.sh --skip-license
RUN cd /cmake \
    && ln -s /cmake/bin/* /usr/local/bin/

# Create the actual image to use to build project
#
# Create target directories
RUN mkdir -p /cmake/bin \
    && mkdir -p /cmake/share \
    && mkdir -p /opencv/opencv-3.4.0 \
    && mkdir -p /glm

FROM gcc:7.2.0
RUN apt-get update \
    && apt-get install -y --no-install-recommends\
    valgrind
# Setup cmake from builder
COPY --from=build /cmake/bin /cmake/bin
COPY --from=build /cmake/share /cmake/share
RUN cd /cmake \
    && ln -s /cmake/bin/* /usr/local/bin/


# Copy and build our actual project
# Layers below will only be built if our project changes
RUN mkdir -p /gaus
WORKDIR /gaus
COPY . .

RUN mkdir -p _build/release && cd _build/release && cmake -G"Unix Makefiles" -D"CMAKE_BUILD_TYPE=release" ../../ && make -j8
RUN mkdir -p _build/debug && cd _build/debug && cmake -G"Unix Makefiles" -D"CMAKE_BUILD_TYPE=debug" ../../ && make -j8
RUN mkdir -p _build/debug && cd _build/debug && cmake -G"Unix Makefiles" -D"CMAKE_BUILD_TYPE=Sanitize" ../../ && make -j8

RUN valgrind --error-exitcode=1 --tool=memcheck --leak-check=full --leak-resolution=med --track-origins=yes _build/release/test/unittests
CMD make -C _build/release -j8 check
