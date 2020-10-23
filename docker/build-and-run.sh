#!/usr/bin/env bash

BASE_DIR="$(pwd)"

docker build -t local/obs-node-docker:latest -f "$BASE_DIR/docker/Dockerfile" "$BASE_DIR/docker"
docker run --rm -ti --privileged -v "$(pwd)":/obs-node local/obs-node-docker:latest bash