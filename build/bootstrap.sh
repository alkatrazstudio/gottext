#!/usr/bin/env bash
set -e
cd "$(dirname -- "$(readlink -f -- "$0")")"
cd ..
ROOT_DIR="$(pwd)"

PHPCPP_VER=v2.2.0

export DEBIAN_FRONTEND=noninteractive
apt-get update -yq > /dev/null
apt-get install -yq git > /dev/null
apt-get clean -q > /dev/null

rm -rf /deps
mkdir /deps
DEPS_ROOT=/deps/dist

pushd /tmp
    # build and install PHP-CPP
    git clone https://github.com/CopernicaMarketingSoftware/PHP-CPP.git
    pushd PHP-CPP
        git checkout "$PHPCPP_VER"

        make STATIC_COMPILER_FLAGS=-fPIC
        make install INSTALL_PREFIX="$DEPS_ROOT"
    popd
    rm -rf PHP-CPP

    # make PHP-CPP symlinks portable
    pushd "$DEPS_ROOT/lib"
        SOFILE_FULL="$(find -regextype posix-egrep -regex '.*/libphpcpp\.so\.[0-9]+\.[0-9]+\.[0-9]+')"
        SOFILE_MINOR="$(find -regextype posix-egrep -regex '.*/libphpcpp\.so\.[0-9]+\.[0-9]+')"
        AFILE_FULL="$(find -regextype posix-egrep -regex '.*/libphpcpp\.a\.[0-9]+\.[0-9]+\.[0-9]+')"

        ln -sf "$SOFILE_FULL" "$SOFILE_MINOR"
        ln -sf "$SOFILE_FULL" libphpcpp.so
        ln -sf "$AFILE_FULL" libphpcpp.a
    popd
popd
