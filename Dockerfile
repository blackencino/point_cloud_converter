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

RUN mkdir /pcc && \
    mkdir /pcc_build

COPY src /pcc/src
COPY third_party /pcc/third_party
COPY CMakeLists.txt /pcc
COPY conanfile.txt /pcc 

RUN cd /pcc_build && \
  cmake -DCMAKE_BUILD_TYPE=Release -GNinja /pcc && \
  ninja

CMD [ "/bin/bash" ]
