#!/usr/bin/env bash

build_dir="../build_dir"

cp -r resources/ $build_dir

for dir in build install sentinels source;
do
  mkdir $build_dir/$dir
done

python3 -u ./get_source.py --source_json sources.json --destination $build_dir/source
