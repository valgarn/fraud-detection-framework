#!/bin/bash

docker system prune -a
cd ../
docker image build -f ubuntu_prod/Dockerfile -t fdf_0.1_prod .
