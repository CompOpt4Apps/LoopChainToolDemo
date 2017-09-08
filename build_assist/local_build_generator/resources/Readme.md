# Local Build
This directory contains the scripts needed to create a local build for the
dependencies of the LoopChainToolDemo project.
These dependencies (and their dependencies) are:
+ Rose @ 0.9.9.0 from https://github.com/rose-compiler/rose
  - Boost @ 1.61 from http://www.boost.org/users/history/version_1_61_0.html
+ LoopChainIR @ head from https://github.com/CompOpt4Apps/LoopChainIR
  - OR-Tools @ 6.2 from https://github.com/google/or-tools
  - Integer Set Library (ISL) @ 0.18 from http://isl.gforge.inria.fr/

These sources are already downloaded by the local build generator script, and
live under the `sources` directory.

## Builing the Dependencies
All the dependencies can be build using the `./build.sh` script.
Setup the build environment and build the build all the dependencies, which
will be subsequently installed to the `install` directory.

### Under the Hood
The build script does two things:
1. Sources the paths script in prepend mode (`./paths --prepend`) to create an
   environment where the dependee libraries it is building are the ones that
   will be found by the depender library that it will later build
   (e.g. LoopChainIR depends on ISL, and we want this build of LoopChainIR to
   use this build of ISL). This avoids some potential errors where dependee
   libraries that may already exist on the system, but are the incorrect
   version.
2. Run the makefile as `make all` with the specified number of jobs
   (by default, runs as many as there are hardware threads).

In the makefile, there is a rule for each dependency.
However, because it is hard to determine if a library has been built or not
from the installation directory alone, the rules are based around sentinel files
that are added/checked for in the `sentinels` directory.
A dependency only depends (in the makefile sense) on these sentinel files.
When a build is performed, the source directory is copied from the `sources`
directory into the `build` directory, and the build is performed.
How the build is done library dependent.
In some cases, such as rose, patches may be applied.
These patches live in the `patches` directory.
When a build terminates, any directories or files  that build creates under the
`build` directory are removed, regardless of whether the build terminated in
success or failure.
When a build succeeds, runs whatever `make install` equivalent for that library,
which has been configured to isntall to the `install` directory, and then adds
it's sentinel file to the sentinels directory. 

## Installing / Pathing the Libraries
This install directory can be moved somewhere more useful, such as to a local
location that is already pointed to by one of the various path environment
variables, or to a more global location, such as `/usr/`.
**Or**, the script `./paths.sh` can be used to append/overwrite/prepend the
path environment variables so that they point (or additionally point) to the
proper directories under the `install` directory.
The `paths.sh` script must be sourced (with `source ./paths.sh [options]` or
`. ./paths.sh [options]`) to work, and will fail if it detects that it has not
been sourced.
To see all options of the paths script, run or source the command `./paths.sh -h`.
