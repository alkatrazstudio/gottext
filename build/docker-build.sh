#!/usr/bin/env bash
set -e
cd "$(dirname -- "$(readlink -f -- "$0")")"
ROOT_DIR="$(pwd)/.."

PHP_VER="$1"

if [[ -z $PHP_VER ]]
then
    PHP_VER="$(php-config --version | grep -Po '^\d+\.\d+')"
fi

IMAGE_NAME="gottext-build-$PHP_VER"

sudo docker build -t "$IMAGE_NAME" -f Dockerfile --build-arg PHP_VER="$PHP_VER" .

sudo docker run \
    --rm \
    -v"$ROOT_DIR":/gottext \
    -w /gottext \
    -u "$(id -u):$(id -g)" \
    "$IMAGE_NAME" bash /gottext/build/build.sh
