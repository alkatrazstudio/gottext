#!/usr/bin/env python3

import random

min_string_len = 10
max_string_len = 50
entries_cnt = 100000

en = ' qwerty uiop asdf ghjkl zxcv bnm ,. '
ru = ' ёйцукен гшщзхъ фыва пролджэ ячсмить бю ,. '


def rand_str(alphabet):
    return ''.join(random.choice(alphabet) for a in range(0, random.randint(min_string_len, max_string_len))).strip()


def rand_str_plural(alphabet):
    s = rand_str(alphabet)
    p = random.randrange(0, len(s))
    return (s[:p] + ' %d ' + s[p:]).strip()


def entry_simple():
    s = rand_str(en)
    po = (
        'msgid "{}"\n'
        'msgstr "{}"'
    ).format(s, rand_str(ru))
    php = '\t__("{}");'.format(s)
    return po, php


def entry_plural():
    s1 = rand_str_plural(en)
    s2 = rand_str_plural(en)

    po = (
        '#, php-format\n'
        'msgid "{}"\n'
        'msgid_plural "{}"\n'
        'msgstr[0] "{}"\n'
        'msgstr[1] "{}"\n'
        'msgstr[2] "{}"'
    ).format(
        s1,
        s2,
        rand_str_plural(ru),
        rand_str_plural(ru),
        rand_str_plural(ru)
    )
    php = '\t_n("{}", "{}", {});'.format(s1, s2, random.randrange(0, 100))
    return po, php


with open('./ru_RU.po', 'w') as po_file, open('./words.php', 'w') as php_file:
    po_file.write(
        'msgid ""\n'
        'msgstr ""\n'
        '"Project-Id-Version: \\n"\n'
        '"POT-Creation-Date: 2016-01-25 20:56+0300\\n"\n'
        '"PO-Revision-Date: 2016-01-26 01:00+0300\\n"\n'
        '"Last-Translator: \\n"\n'
        '"Language-Team: \\n"\n'
        '"Language: ru_RU\\n"\n'
        '"MIME-Version: 1.0\\n"\n'
        '"Content-Type: text/plain; charset=UTF-8\\n"\n'
        '"Content-Transfer-Encoding: 8bit\\n"\n'
        '"Plural-Forms: nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && n"\n'
        '"%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);\\n"\n'
        '\n'
    )

    php_file.write(
        '<?php\n'
        'function doTest(){\n'
    )

    for a in range(0, entries_cnt):
        if random.getrandbits(1):
            po, php = entry_simple()
        else:
            po, php = entry_plural()
        po_file.write('#: words.php:{}\n'.format(a+2))
        po_file.write(po)
        po_file.write('\n\n')
        php_file.write(php)
        php_file.write('\n')

    php_file.write('}\n')
