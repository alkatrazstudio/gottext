<?php
class BenchEngineBase
{
    protected static $gotText;

    static function __($s)
    {
        return self::$gotText->_($s);
    }

    static function _n($s1, $s2, $n)
    {
        return self::$gotText->_n($s1, $s2, $n);
    }

    static function doInit()
    {
        self::$gotText = new GotText(__DIR__."/../locale/ru_RU/LC_MESSAGES/messages.mo");
    }
}
