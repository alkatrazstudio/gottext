#!/usr/bin/env bash

#
# This script needs to be run in a "php" Docker container
#
set -ex
cd "$(dirname -- "$(readlink -f -- "$0")")"
cd ..


build/bootstrap.sh
build/build.sh
make install
make test_installed
build/archive.sh
make uninstall
make clean


function do_build {
    make PHPCPP_ROOT=/deps/dist "$@"
    make PHPCPP_ROOT=/deps/dist test
    make "$@" clean
}


export DEBIAN_FRONTEND=noninteractive
apt-get install -yq libboost-thread-dev > /dev/null
do_build THREAD_SAFE=1

do_build NATIVE_FILE=1

apt-get install -yq libboost-regex-dev > /dev/null
do_build BOOST_REGEX=1

do_build THREAD_SAFE=1 NATIVE_FILE=1 DEBUG=1 BOOST_REGEX=1
