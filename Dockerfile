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
