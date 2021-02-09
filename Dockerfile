FROM ubuntu:20.04

RUN apt-get update && \
    apt-get install -y software-properties-common

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    cmake \
    curl \
    flex bison \
    git \
    m4 \
    nano \
    ninja-build \
    python3 \
    python3-pip \
    sudo \
    wget

RUN pip3 install conan

RUN mkdir /pcc_source && \
    mkdir /pcc_build && \
    mkdir /pcc

RUN echo "blah3"

COPY src /pcc_source/src
COPY third_party /pcc_source/third_party
COPY CMakeLists.txt /pcc_source
COPY conanfile.txt /pcc_source

RUN cd /pcc_build && \
  cmake -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/pcc \
        -GNinja /pcc_source && \
  ninja install

RUN rm -rf /pcc_build && \
    rm -rf /pcc_source

RUN conan remove "*" -s -b -f

CMD [ "/pcc/bin/e57_to_pts" ]
