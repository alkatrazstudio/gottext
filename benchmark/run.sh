#!/usr/bin/env bash
set -e
cd "$(dirname -- "$(readlink -f -- "$0")")"

function rtrim {
    local var="$*"
    var="${var%"${var##*[![:space:]]}"}"
    echo -n "$var"
}

if [[ ! -f words.php ]] || [[ ! -f locale/ru_RU/LC_MESSAGES/messages.mo ]]
then
    python3 gen_words.py
    mkdir -p locale/ru_RU/LC_MESSAGES
    msgfmt ru_RU.po -o locale/ru_RU/LC_MESSAGES/messages.mo
fi

readarray GOTTEXT_RESULTS <<< "$(php bench.php gottext yes)"
readarray GETTEXT_RESULTS <<< "$(php bench.php gettext no)"

printf "%35s%10s\n" GotText gettext
echo "---------------------------------------------"
for i in "${!GOTTEXT_RESULTS[@]}"
do
    printf "%s%10s\n" "$(rtrim "${GOTTEXT_RESULTS[i]}")" "$(rtrim "${GETTEXT_RESULTS[i]}")"
done
