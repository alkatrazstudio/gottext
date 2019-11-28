#!/usr/bin/env bash
set -e
cd "$(dirname -- "$(readlink -f -- "$0")")"
cd ..
ROOT_DIR="$(pwd)"

PHPCPP_VER=2.2.0

DEBIAN_FRONTEND=noninteractive apt update -yq > /dev/null
DEBIAN_FRONTEND=noninteractive apt install -yq git > /dev/null

function doWork {
    mkdir deps
    pushd deps
        DEPS_ROOT="$(pwd)/dist"

        # build and install PHP-CPP
        git clone https://github.com/CopernicaMarketingSoftware/PHP-CPP.git
        pushd PHP-CPP
            git checkout "v$PHPCPP_VER"

            make STATIC_COMPILER_FLAGS=-fPIC
            make install INSTALL_PREFIX="$DEPS_ROOT"
        popd

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

    # build and install GotText
    mkdir gottext
    pushd gottext
        make -f "$ROOT_DIR/Makefile" clean
        make PHPCPP_ROOT="$DEPS_ROOT" STANDALONE=1 -f "$ROOT_DIR/Makefile"
    popd
}

pushd /tmp
    doWork
popd

# copy the output artifacts into a separate folder
rm -rf dist
mkdir dist
pushd dist
    cp -a /tmp/deps/dist/. ./
    cp -a /tmp/gottext/dist/*.so ./

    if [[ ! -z $USER_UID ]]
    then
        chown "$USER_UID:$USER_GID" -R .
    fi
popd
