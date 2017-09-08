FROM ubuntu:16.04

COPY ../local_build_generator /tmp/generator_dir

COPY ./entrypoint.sh /usr/bin/entrypoint.sh

RUN apt update && \
    apt upgrade -y && \
    apt install \
      mercurial git subversion wget \
      csh \
      tar unzip \
      make autoconf automake cmake libtool libtool-bin \
      perl python3 gcc-5 g++-5 default-jdk default-jre \
      flex bison \
      libarchive-dev libbz2-dev libgmp-dev libgmp10 libgmpxx4ldbl zlib1g zlib1g-dev \
      lsb-release ghostscript \
      -y && \
    apt autoremove -y && \
    ln -s $(which g++-5) /usr/bin/g++ && \
    cd /tmp/generator_dir && \
      ./generate_local_build.sh && \
    cd /tmp/build_dir && \
      ./build.sh && \
      cp -r /tmp/build_dir/install/* /usr/. && \
    cd /tmp/ && \
      git clone https://github.com/CompOpt4Apps/LoopChainToolDemo.git && \
    cd /tmp/OpenMP_LoopChain_Tool_Demo && \
      ./configure && \
      make -C omplc_tool && \
      cp src/lctool /usr/bin/. && \
    apt remove --purge \
      mercurial git subversion wget \
      csh \
      unzip \
      make autoconf automake cmake libtool libtool-bin \
      perl python3 default-jdk default-jre \
      flex bison \
      libarchive-dev libbz2-dev zlib1g-dev \
      lsb-release ghostscript \
      -y && \
    apt clean autoclean && \
    apt autoremove -y && \
    rm -rf /var/lib/{apt,dpkg,cache,log}/ && \
    rm -rf /tmp/*

ENTRYPOINT ["/usr/bin/entrypoint.sh"]
