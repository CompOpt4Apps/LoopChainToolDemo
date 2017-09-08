# Local Build
To ease the acquisition and build process for the libraries that the
LoopChainToolDemo depends on, the local build tool automatically downloads the
sources for these libraries and produces the scripts necessary to build them
on the Ubuntu 16.04 system.

This is used to build and install the dependencies locally, and is used in the
docker container build process.

## Creating a Local Build Directory
Running `./generate_local_build.sh` will produce a directory in the parent
directory named `build_dir`.
In it will be all the sources and scripts necessary to build on the supported
system.
See the Readme in the `resources` directory or the generated `build_dir`
directory for more information and instructions.

### Under the Hood
The `generate_local_build.sh` script does two main things:
1. Sets up the directory structure and copies scripts
2. Downloads the dependency sources

### Creating the Directories and Scripts
The `resources` directory contains all the pre-written scripts, patches, and the
Readme.
This directory is copied to `../build_dir` to create it.
Directories without any contents (`build`, `install`, `sentinels`, `source`) are
all created under the `../build_dir` directory.

### Downloading Sources
The python script `get_source.py` handles the downloading process.
Sources are specified in `sources.json`.
Each source is a JSON object with the required keys:
+ "name" is the name of the library. It is also the name of the library's root
source directory, so it should not have spaces or magic characters.
+ "method": method by which the source is downloaded. Two methods are supported:
  - "git" : use this method if sources are cloned via git.
    Additional tags:
    + "tag" : optional key that can be used with the "git" method to specify a
      tag to checkout after cloning.
  - "wget" : Use this method if sources are simply downloaded.
    + "arcive_info" : mandatory key that must be used with the "wget" method
      that provides the script with additional information. Has the keys:
      - "name" : actual name of the resulting archive.
      - "type" : Can be "tar" if a `*.tar.*` file or "zip" if a `*.zip` file.
      - "root_dir" : Name of root directory resulting from extracting the tar
        file. This does require that an archive *have* a root directory.

The downloader script has several options:
+ -h, --help            show this help message and exit
+ -s --source_json SOURCE_JSON : Specify a JSON file with the source
  information. Default = "sources.json"
+ -p --processes PROCESSES : Run the download script with PROCESS number of
  concurrent processes. Default = 1/2 * hardware threads
+ -d --destination DESTINATION : Specify where resulting sources are moved to.
  Default = ./local_build/source
+ -o --only-these-packages ONLY_THESE_PACKAGES [ONLY_THESE_PACKAGES ...] :
  Specify a subset of packages from the sources JSON list to download and
  unpack.
+ -l, --list-packages : list all the packages in the specified JSON source file.
