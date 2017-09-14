#!/usr/bin/env bash

if [ -e ./local_build_generator ]
then
  rm -rf ./local_build_generator
fi

cp -r ../local_build_generator .

echo "Creating docker container"
sudo docker build -t compopt4apps/loopchaintooldemo_short -f ./Dockerfile .
