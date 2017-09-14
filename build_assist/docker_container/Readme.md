# LoopChainToolDemo Docker Container
To eliminate the difficulty introduced by different OS' environments, the
scripts in this directory will create a docker container that will always
successfully build this project and its dependencies.

Before using, please have some familiarity with what docker containers are,
and some of the docker commands, such as `docker images` and `docker run`.
You will need to have root access (such as through `sudo`) in order to run
these scripts and use the resulting container.

## Building the Container
Running `sudo ./make_container.sh` (or running `./make_container.sh` as root)
will perform all the actions required to build the container.

## Using the Container
To transform a source file, it should be `cat`-ed to the docker run command,
where its output will be printed to stdout, and can be captured.
This can be done as follows:
```
cat path/to/source/file.cpp | sudo docker run -i compopt4apps/loopchaintooldemo > path/to/output/file.cpp
```

Use your system's particular method for executing commands as root if `sudo` is
not used.
