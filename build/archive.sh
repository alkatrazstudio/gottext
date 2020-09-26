#!/usr/bin/env bash
set -e
cd "$(dirname -- "$(readlink -f -- "$0")")"
cd ..
ROOT_DIR="$(pwd)"

VERSION="$(<VERSION)"
PHP_VER="$(php-config --version | sed -r 's/^([0-9]+)\.([0-9]+).*/\1\2/')"
ARCHIVE_NAME="gottext-php$PHP_VER-v$VERSION"
TMP_DIR="$ARCHIVE_NAME"
ARCHIVE_FILENAME="$ROOT_DIR/dist/$ARCHIVE_NAME.tar.xz"

cd /tmp
rm -rf "$TMP_DIR"
mkdir -p "$TMP_DIR"

cp "$ROOT_DIR/dist/gottext.so" "$TMP_DIR/"
cp "$ROOT_DIR/src/gottext.ini" "$TMP_DIR/"

rm -f "$ARCHIVE_FILENAME"
XZ_OPT=-e9 tar -cJf "$ARCHIVE_FILENAME" "$TMP_DIR"
