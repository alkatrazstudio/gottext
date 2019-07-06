#!/usr/bin/env bash
set -e
THIS_DIR="$(dirname -- "$(readlink -f -- "$0")")"
cd "$THIS_DIR/.."
ROOT_DIR="$(pwd)"

PHP_VER="$1"

if [[ -z $PHP_VER ]]
then
    PHP_VER="$(php-config --version | grep -Po '^\d+\.\d+')"
fi

docker run \
    --rm \
    -v"$ROOT_DIR":/gottext \
    -w /gottext \
    php:"$PHP_VER" make test
