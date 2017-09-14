# Under The Hood
Here is an 'under the hood' view at how the local build process works.

## Builing the Dependencies
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
which has been configured to install to the `install` directory, and then adds
it's sentinel file to the sentinels directory.
