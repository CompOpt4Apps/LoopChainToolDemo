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
