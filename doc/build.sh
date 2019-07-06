#!/usr/bin/env bash
set -e
cd "$(dirname -- "$(readlink -f -- "$0")")"
ROOT_DIR="$(pwd)"

APIGEN_URL=https://github.com/ApiGen/ApiGen/releases/download/v4.1.2/apigen.phar

# install ApiGen
curl -s -o /usr/bin/apigen -L -O "$APIGEN_URL"
chmod u+x /usr/bin/apigen

# generate docs
DIST_DIR=/tmp/dist
apigen --quiet generate --source source --destination "$DIST_DIR"

# format docs
VER_STR="$(< ../VERSION)"
php ./modify.php "$DIST_DIR/class-GotText.html" "$VER_STR"
echo "
#right {
    margin: 0;
    display: flex;
    justify-content: center;
}

#rightInner {
    min-width: 1000px;
}

.method-name {
    color: darkcyan
}

#footer {
    background: none;
}
" >> "$DIST_DIR/resources/style.css"

# copy the resulting docs to a separate folder
rm -rf dist
mkdir dist
pushd dist &> /dev/null
    cp "$DIST_DIR/class-GotText.html" ./index.html
    cp "$DIST_DIR/resources/style.css" ./
    if [[ ! -z $USER_UID ]]
    then
        chown "$USER_UID:$USER_GID" -R .
    fi
popd &> /dev/null
