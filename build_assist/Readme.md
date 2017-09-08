# Build/Use Assistance Scripts
This project has some difficult dependencies to satisfy.
There are two ways to avoid solving this problem yourself:
1. Use the scripts in the directory `local_build_generator` to automatically
   download and build the dependencies. Depending on your system this may or
   may-not work.
2. Use the scripts in the directory `docker_container` to create a docker
   container that will wrap the LoopChainToolDemo executable. Because the docker
   container is a sanitized and highly controlled environment, there is a very
   high probability that it will successfully build the dependencies and the
   LoopChainToolDemo executable. The trade-off is that running docker in any way
   requires root access, and the tools becomes slightly less accessible.

The Readmes in both of these directories explain their use.
