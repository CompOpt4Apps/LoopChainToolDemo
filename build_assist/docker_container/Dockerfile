FROM ubuntu:16.04

COPY ./local_build_generator /tmp/generator_dir

COPY ./entrypoint.sh /usr/bin/entrypoint.sh

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install \
      mercurial git subversion wget \
      csh \
      tar unzip \
      make autoconf automake cmake libtool libtool-bin \
      perl python3 gcc-5 g++-5 default-jdk default-jre \
      flex bison \
      libarchive-dev libbz2-dev libgmp-dev libgmp10 libgmpxx4ldbl zlib1g zlib1g-dev \
      lsb-release ghostscript \
      -y && \
    apt-get autoremove -y && \
    ln -s $(which g++-5) /usr/bin/g++ && \
    cd /tmp/generator_dir && \
      ./generate_local_build.sh && \
    cd /tmp/build_dir && \
      ./build.sh && \
      cp -r /tmp/build_dir/install/* /usr/. && \
    cd /tmp/ && \
      git clone https://github.com/CompOpt4Apps/LoopChainToolDemo.git && \
    cd /tmp/LoopChainToolDemo && \
      ./configure && \
      make -C src && \
      cp src/lctool /usr/bin/. && \
    apt-get remove --purge \
      mercurial git subversion wget \
      csh \
      unzip \
      make autoconf automake cmake libtool libtool-bin \
      perl python3 default-jdk default-jre \
      flex bison \
      libarchive-dev libbz2-dev zlib1g-dev \
      lsb-release ghostscript \
      -y && \
    apt-get clean autoclean && \
    apt-get autoremove -y && \
    rm -rf /var/lib/{apt,dpkg,cache,log}/ && \
    rm -rf /tmp/* && \
    chmod +x /usr/bin/entrypoint.sh

ENTRYPOINT ["/usr/bin/entrypoint.sh"]
