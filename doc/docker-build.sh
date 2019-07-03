#!/usr/bin/env bash
set -e
THIS_DIR="$(dirname -- "$(readlink -f -- "$0")")"
cd "$THIS_DIR/.."
ROOT_DIR="$(pwd)"

docker run \
    --rm \
    -v"$ROOT_DIR":/gottext \
    -w /gottext \
    -e "USER_UID=$(id -u)" \
    -e "USER_GID=$(id -g)" \
    php:5.6 bash /gottext/doc/build.sh
