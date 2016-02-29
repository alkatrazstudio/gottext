<?php
ini_set("display_errors", 1);
ini_set("display_startup_errors", 1);
error_reporting(E_ALL);
date_default_timezone_set("UTC");

assert_options(ASSERT_CALLBACK,
    function($script, $line, $message, $custom = "") {
        echo "Line ", $line;
        if($custom)
            echo " (", $custom, ")";
        echo ": ", $message, "\n";
        exit(1);
    }
);

assert("chdir(__DIR__)");

assert('copy("./ru_RU.mo.1", "./ru_RU.mo")');

assert('$gotText = new GotText("./ru_RU.mo")');

assert('$gotText->_("Title") === "Название"');
assert('$gotText->_n("%d site", "%d sites", 1) === "%d место"');
assert('$gotText->_n("%d site", "%d sites", 2) === "%d места"');
assert('$gotText->_n("%d site", "%d sites", 5) === "%d мест"');
assert('$gotText->_p("Person", "Title") === "Титул"');
assert('$gotText->_np("Web", "%d site", "%d sites", 21) === "%d сайт"');
assert('$gotText->_np("Web", "%d site", "%d sites", 22) === "%d сайта"');
assert('$gotText->_np("Web", "%d site", "%d sites", 11) === "%d сайтов"');

assert('$gotText->isDummy() === false');
assert('$gotText->getPluralsCount() === 3');
assert('$gotText->pluralFunc(121) === 0');
assert('$gotText->pluralFunc(122) === 1');
assert('$gotText->pluralFunc(111) === 2');
assert('$gotText->getFilename() === "./ru_RU.mo"');
assert('$gotText->getLocaleCode() === "ru_RU"');

sleep(2);

assert('$gotTextData = new GotText("ru_RU", file_get_contents("./ru_RU.mo"))');
assert('$gotTextData->getStrings() === $gotText->getStrings()');

assert('$gotText2 = new GotText("./ru_RU.mo")');
assert('$timeFirstCached = $gotText->getTimeCached()');
assert('$timeFirstCached === $gotText2->getTimeCached()');
assert('$gotTextData->getTimeCached() > $timeFirstCached');

assert('GotText::getFilenames() === array("./ru_RU.mo", "ru_RU")');

assert('$gotTextEmpty = new GotText()');
assert('$gotTextEmpty->isDummy() === true');
assert('$gotTextEmpty->getPluralsCount() === 0');
assert('$gotTextEmpty->pluralFunc(3) === 0');
assert('$gotTextEmpty->getFilename() === ""');
assert('$gotTextEmpty->getLocaleCode() === ""');
assert('$gotTextEmpty->getTimeCached() === 0');

assert('$gotTextData->isDummy() === false');

assert('GotText::getFilenames() === array("./ru_RU.mo", "ru_RU")');
GotText::unload("ru_RU");
assert('GotText::getFilenames() === array("./ru_RU.mo", "ru_RU")');

assert('$gotTextData->isDummy() === true');
assert('$gotTextData->getPluralsCount() === 0');
assert('$gotTextData->pluralFunc(3) === 0');
assert('$gotTextData->getFilename() === "ru_RU"');
assert('$gotTextData->getLocaleCode() === ""');
assert('$gotTextData->getTimeCached() === 0');

assert('$gotTextData->getStrings() === $gotTextEmpty->getStrings()');
assert('!GotText::get("ru_RU")');
assert('GotText::get("./ru_RU.mo")->getStrings() === $gotText->getStrings()');

assert('copy("./ru_RU.mo.2", "./ru_RU.mo")');

assert('$gotTextNew = new GotText("./ru_RU.mo")');
assert('$gotTextNew->getStrings() === $gotText->getStrings()');
assert('$gotText->_("Hello") === "Привет"');
GotText::reload("./ru_RU.mo");
assert('$gotTextNew->getStrings() === $gotText->getStrings()');
assert('$gotTextNew->getStrings() !== $gotTextData->getStrings()');
assert('$gotText->_("Hello") === "Здравствуйте"');
assert('$gotText->getTimeCached() > $timeFirstCached');

assert('unlink("./ru_RU.mo")');

try{
    $gotTextInvalid = new GotText("./ru_RU.mo.invalid");
    $msg = "";
}catch(Exception $e){
    $msg = $e->getMessage();
}
assert('strlen($msg)', "invalid file");

try{
    @$gotTextInvalid = new GotText("./ru_RU.mo.nonexistent");
    $msg = "";
}catch(Exception $e){
    $msg = $e->getMessage();
}
assert('strlen($msg)', "nonexistent file");

assert('$info = GotText::getInfo()');
assert('is_array($info)');
foreach(array("major", "minor", "patch", "timestamp") as $key)
{
    assert('isset($info["'.$key.'"])');
    assert('is_int($info["'.$key.'"])');
    assert('$info["'.$key.'"] >= 0');
}
assert('$info["timestamp"] == strtotime(date("Y-m-d H:i:s", $info["timestamp"]))');
foreach(array("thread_safe", "native_file", "debug", "boost_regex") as $key)
{
    assert('isset($info["'.$key.'"])');
    assert('is_bool($info["'.$key.'"])');
}

echo "All tests were passed correctly.\n";
