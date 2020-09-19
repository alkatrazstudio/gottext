#!/usr/bin/env bash
set -e
cd "$(dirname -- "$(readlink -f -- "$0")")"
cd ..
ROOT_DIR="$(pwd)"
DEPS_ROOT=/deps/dist

pushd /tmp
    mkdir gottext
    pushd gottext
        make -f "$ROOT_DIR/Makefile" clean
        make PHPCPP_ROOT="$DEPS_ROOT" STANDALONE=1 -f "$ROOT_DIR/Makefile"
    popd
popd

# copy the output artifacts into a separate folder
rm -rf dist
mkdir dist
pushd dist
    cp -a /deps/dist/. ./
    cp -a /tmp/gottext/dist/*.so ./
popd
