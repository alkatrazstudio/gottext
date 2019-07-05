<?php
class BenchEngineBase
{
    static function __($s)
    {
        return gettext($s);
    }

    static function _n($s1, $s2, $n)
    {
        return ngettext($s1, $s2, $n);
    }

    static function doInit()
    {
        setlocale(LC_MESSAGES, "ru_RU.UTF-8");
        bindtextdomain("messages", __DIR__."/../locale");
        textdomain("messages");
        bind_textdomain_codeset("messages", "UTF-8");
    }
}
