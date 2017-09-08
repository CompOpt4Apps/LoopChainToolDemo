#!/usr/bin/env bash

echo "Creating docker container"
sudo docker build -t LoopChainToolDemo -f ./Dockerfile .
