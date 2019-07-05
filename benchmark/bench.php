#!/usr/bin/env php
<?php
$engine = $argv[1] ?? "";
$showTitle = $argv[2] ?? "yes";
switch($engine)
{
    case "gettext":
    case "gottext":
        break;

    default:
        die("Unknown engine");
}
require __DIR__."/words.php";
require __DIR__."/engines/$engine.php";

class BenchEngine extends BenchEngineBase
{
    protected static $add = "";
    protected static $showTitle = true;

    static function setMiss($miss)
    {
        self::$add = $miss ? "_" : "";
    }

    static function setShowTitle($show)
    {
        self::$showTitle = $show != "no";
    }

    static function __($s)
    {
        return parent::__($s.self::$add);
    }

    static function _n($s1, $s2, $n)
    {
        return parent::_n($s1.self::$add, $s2.self::$add, $n);
    }

    static function printVal($title, $num, $unit)
    {
        if(self::$showTitle)
            echo sprintf("%25s", $title." ($unit)"), " - ", sprintf("%4d", $num);
        else
            echo $num;
        echo "\n";
    }

    static function benchmark($f, $title, $n = 1)
    {
        $start = microtime(true);
        for($a=0; $a<$n; $a++)
            $f();
        $finish = microtime(true);
        $diff = round(($finish - $start)/$n*1000);
        self::printVal($title, $diff, "ms");
    }
}

function __($s)
{
    return BenchEngine::__($s);
}

function _n($s1, $s2, $n)
{
    return BenchEngine::_n($s1, $s2, $n);
}

function doInit()
{
    BenchEngine::doInit();
}



//
// BENCHMARK
//

BenchEngine::setShowTitle($showTitle);
BenchEngine::benchmark("doInit", "cold initialization");
BenchEngine::benchmark("doInit", "re-initialization");
BenchEngine::benchmark("doTest", "translate 1 pass");
BenchEngine::setMiss(true);
BenchEngine::benchmark("doTest", "miss 1 pass");
BenchEngine::setMiss(false);
BenchEngine::benchmark("doTest", "translate 100 passes", 100);
BenchEngine::setMiss(true);
BenchEngine::benchmark("doTest", "miss 100 passes", 100);
