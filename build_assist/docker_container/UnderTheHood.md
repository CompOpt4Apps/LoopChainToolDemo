# Under The Hood
Here is an 'under the hood' view at how the build and usage process works
in the docker container.

## Building The Container
The build process is described below for the curious.
The script `make_container.sh` wraps the command
`sudo docker build -t LoopChainToolDemo -f ./Dockerfile .`
as well makes a copy of `local_build_generator` in this directory.

This command tells docker to build a container named
`compopt4apps/loopchaintooldemo` using the commands from the docker-file
`./Dockerfile`
The docker-file lists the commands docker needs to run to successfully build
the container.

First docker copies `../local_build_generator` into the container's `/tmp/`
directory as `generator_dir`.
Then it copies the script `./endpoint.sh` into the containers `/usr/bin`
The endpoint script is what the container will execute when run.


After that has been done, docker then runs a very long command that is actually
many commands chained together with the 'and' operator (`&&`).
This is done because each `RUN` command in the docker-file creates a new
intermediate container, which can cause lots of bloat on the host file-system.
To avoid that, we use one long command.

The first set of commands in this chain update and install software using apt.
```
apt update && \
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
apt autoremove -y
```

Next we create a symlink to `g++-5` as `/usr/bin/g++`.
```
ln -s $(which g++-5) /usr/bin/g++
```

Next we `cd` into the local build generator scripts, and generate a local build
directory in `/tmp/`.
```
cd /tmp/generator_dir && \
  ./generate_local_build.sh
```

Next we `cd` into the local build directory, run the build script, and copy the
files from the local build's `install` directory into the container's `/usr/` directory.
This will build the dependencies for the LoopChainToolDemo project.
```
cd /tmp/build_dir && \
  ./build.sh && \
  cp -r /tmp/build_dir/install/* /usr/.
```

Next we `cd` into the container's `/tmp/` and clone the LoopChainToolDemo
repository (which is this repository, to be exact).
```
cd /tmp/ && \
  git clone https://github.com/CompOpt4Apps/LoopChainToolDemo.git
```

Next we `cd` into the repository directory, configure, build, and install the
LoopChainToolDemo executable to the container's `/usr/bin/` directory.
```
cd /tmp/OpenMP_LoopChain_Tool_Demo && \
  ./configure && \
  make -C omplc_tool && \
  cp src/lctool /usr/bin/.
```

Next we forcefully remove most of the packages we installed earlier, since they
are no longer needed to run the LoopChainDemoTool executable.
```
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
apt autoremove -y
```

In the last command in the `RUN` docker command we remove various trash files
that no longer serve a purpose.
```
rm -rf /var/lib/{apt,dpkg,cache,log}/ && \
rm -rf /tmp/*
```

Finally, we set the entrypoint of the docker container to be
`/usr/bin/entrypoint.sh`, where we copied the entrypoint script.
Again, the endpoint script is what the container will execute when run.


## Using The Container
The script `entrypoint.sh` this whole processes.
Because the LoopChainToolDemo executable reads a file from a path given on a
command line, and because it's not very easy/practical to copy files to/from
the docker container, this script (while in the executing docker container
directs its stdin (which is being fed from our `cat input.cpp` command) to a
local file, then runs the LoopChainToolDemo executable on that local file, then
`cat`s the resulting file, which appears on our stdout.
