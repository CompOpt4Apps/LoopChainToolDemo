#!/usr/bin/env bash

# Setup environment variables
source ./paths.sh --prepend

# Build dependencies
make JOBS=$(lscpu | grep "^CPU(s):" | awk '{ print $2 }') all
