#!/bin/bash

docker system prune -a
pushd ../Content/
rm -rf ./fdf
git clone --depth 1 --single-branch --branch master https://source.developers.google.com/p/fraud-detection-framework/r/fdf
popd
cd ..
docker image build -f ubuntu_build/Dockerfile -t fdf_0.1_build .
