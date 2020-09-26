<?php
/**
 * GotText - a translation engine with gettext-like features.
 *
 * @package GotText
 * @author Alexey Parfenov aka ZXED <zxed@alkatrazstudio.net>
 * @copyright 2016, Alkatraz Studio <a href="https://alkatrazstudio.net" target="_blank" rel="noreferrer">https://alkatrazstudio.net</a>
 * @license http://www.gnu.org/licenses/gpl.html GNU General Public License version 3 or later
 */

/**
 * GotText - a translation engine with gettext-like features.
 * <a href="https://github.com/alkatrazstudio/gottext" target="_blank" rel="noreferrer">https://github.com/alkatrazstudio/gottext</a>
 */
class GotText {

    /**
     * Constructs GotText object.
     *
     * Loads a file with translations.
     * The file can be any file that can be opened using __fopen()__.
     *
     * Throws __Exception__ on error.
     *
     * All loaded files are cached in memory,
     * so the next time the file is requested it's loaded from memory, not from disk.
     * The memory cache is per-process,
     * e.g. all scripts that were loaded by the same PHP process
     * will share the same memory cache.
     *
     * The memory cache is a key-value storage
     * where a key is a __filename__ passed to a GotText constructor.
     * To load a file from cache the GotText must be created with the same exact filename string
     * that was used to load a file for the first time. It means that if you load two files
     * `./tr/ru_RU.mo` and `./tr/../tr/ru_RU.mo`,
     * then GotText will load the same file from disk twice and create two
     * different cache entries in memory.
     *
     * GotText will never automatically reload a file from disk
     * even if the file is changed or even deleted.
     * To reload a file use {@see reload()}.
     * To unload a file from memory use {@see unload()}.
     * To see all previously loaded files use {@see getFilenames()}.
     * All these functions still only apply to a current PHP process.
     *
     * GotText can also load translations from raw binary string data.
     * Pass the data as a second parameter.
     * In this case you may specify any arbitrary string for a filename,
     * which can be later used to refer to that data, i.e. in {@see get()}.
     *
     * Note that GotText will ignore any plural form rules found in MO file.
     * All plural form rules are hardcoded into GotText.
     * The only information GotText will retrieve and use from MO file header
     * is a locale code.
     *
     * @param string $filename A file with translations to load in gettext MO format.
     * If not specified then a dummy GotText will be returned.
     * See {@see isDummy()} to read more about dummy objects.
     * @param string $data A binary data in gettext MO format.
     * If this parameter is specified,
     * then __filename__ can be any arbitrary string.
     * The translations are always reloaded from this data
     * when this parameter is specified.
     *
     * @return void
     *
     * @example
     * ```php
     * <?php
     * // loading from file
     * try{
     *     $gotText = new GotText("./ru_RU.mo");
     * }catch(Exception $e){
     *     error_log($e->getMessage());
     *     $gotText = new GotText();
     * }
     * echo $gotText->_("Hello"), "\n";
     *
     * // loading from string data
     * try{
     *     $data = file_get_contents("./ja_JP.mo")
     *     $gotText2 = new GotText("Japanese", $data);
     * }catch(Exception $e){
     *     error_log($e->getMessage());
     *     $gotText2 = new GotText();
     * }
     * echo $gotText2->_("Hello"), "\n";
     *
     * // loading the previous translations from "Japanese" string data
     * $gotText3 = GotText::get("Japanese");
     * if(!$gotText3) // if the translations can not be loaded
     *     $gotText3 = new GotText(); // then don't translate anything
     * echo $gotText3->_("Hello"), "\n";
     *
     * // loading from URL
     * try{
     *     $gotText4 = new GotText("http://example.com/sv_SE.mo");
     * }catch(Exception $e){
     *     error_log($e->getMessage());
     *     $gotText4 = new GotText();
     * }
     * echo $gotText4->_("Hello"), "\n";
     * ```
     */
    public function __construct($filename = null, $data = null){}

    /**
     * Translates a string.
     *
     * Similar to gettext(msgid).
     *
     * @param string $msgid A string to translate.
     *
     * @return string The translation of __msgid__, or __msgid__ if no translation found.
     *
     * @example
     * ```php
     * <?php
     * $gotText = new GotText("./ru_RU.mo");
     * echo $gotText->_("Hello"); // "Привет"
     * ```
     */
    public function _($msgid){}

    /**
     * Translates a string and chooses a plural form.
     *
     * Similar to ngettext(msgid, msgid_plural, n).
     *
     * @param string $msgid A string to translate.
     * @param string $msgid_plural A plural form of __msgid__ for the default language (English).
     * @param int $n A number to choose a plural form for. Currently only positive integers are handled correctly.
     *
     * @return string The plural translation of __msgid__ for the number __n__.
     * If no translation is found then the function returns __msgid__ if __n__ == 1, or __msgid_plural__ if __n__ != 1.
     *
     * @example
     * ```php
     * <?php
     * $gotText = new GotText("./ru_RU.mo");
     * echo sprintf($gotText->_n("%d book", "%d books", 1), 1); // "1 книга"
     * echo sprintf($gotText->_n("%d book", "%d books", 2), 2); // "2 книги"
     * echo sprintf($gotText->_n("%d book", "%d books", 5), 5); // "5 книг"
     * ```
     */
    public function _n($msgid, $msgid_plural, $n){}

    /**
     * Translates a string using a provided context.
     *
     * Behaves like {@see _()}, but also takes a message context __msgid_ctxt__ into account.
     * Similar to pgettext(msgid_ctxt, msgid).
     *
     * @param string $msgid_ctxt A context to look for __msgid__ in.
     * @param string $msgid A string to translate.
     *
     * @return string The translation of __msgid__ in the provided context __msgid_ctxt__, or __msgid__ if no such translation found.
     *
     * @example
     * ```php
     * <?php
     * $gotText = new GotText("./ru_RU.mo");
     * echo $gotText->_("Title"); // "Название"
     * echo $gotText->_p("Person", "Title"); // "Титул"
     * ```
     */
    public function _p($msgid_ctxt, $msgid){}

    /**
     * Translates a string using a provided context and chooses a plural form.
     *
     * Behaves like {@see _n()}, but also takes a message context __msgid_ctxt__ into account.
     * Similar to npgettext(msgid_ctxt, msgid, msgid_plural, n).
     *
     * @param string $msgid_ctxt A context to look for __msgid__ in.
     * @param string $msgid A string to translate.
     * @param string $msgid_plural A plural form of __msgid__ for the default language (English).
     * @param int $n A number to choose a plural form for. Currently only positive integers are handled correctly.
     *
     * @return string The plural translation of __msgid__ in the provided context __msgid_ctxt__ for the number __n__.
     * If no translation is found then the function returns __msgid__ if __n__ == 1, or __msgid_plural__ if __n__ != 1.
     *
     * @example
     * ```php
     * <?php
     * $gotText = new GotText("./ru_RU.mo");
     * echo sprintf($gotText->_np("Web", "%d site", "%d sites", 5), 5); // "5 сайтов"
     * echo sprintf($gotText->_np("Location", "%d site", "%d sites", 5), 5); // "5 мест"
     * ```
     */
    public function _np($msgid_ctxt, $msgid, $msgid_plural, $n){}

    /**
     * Checks if it's a dummy GotText object.
     *
     * Dummy GotText objects contain no translations and are usually used as stubs
     * when loading a MO file fails.
     * GotText object with a valid file loaded can become a dummy object
     * if the corresponding file is later unloaded from memory via {@see unload()}.
     *
     * Be careful when using functions like {@see getPluralsCount}, {@see pluralFunc} or similar functions.
     * Such functions will always return an empty string or a zero for dummy objects.
     * See the example below.
     *
     * @return bool __TRUE__ if the GotText object is a dummy object.
     *
     * @example
     * ```php
     * <?php
     * try{
     *     $gotText = new GotText("./invalid.lang.file");
     * }catch(Exception $e){
     *     // construct a fallback dummy object
     *     $gotText = new GotText();
     * }
     * // the following will not fail
     * // even if the translation file was not loaded
     * echo $gotText->_("Hello"); // "Hello"
     * var_export($gotText->isDummy()); // true
     * var_export($gotText->getPluralsCount()); // 0
     * var_export($gotText->pluralFunc(3)); // 0
     * var_export($gotText->getFilename()); // ""
     * var_export($gotText->getLocaleCode()); // ""
     *
     * // load a valid file, then unload it
     * $gotText = new GotText("./ru_RU.mo");
     * var_export($gotText->isDummy()); // false
     * echo $gotText->_n("%d book", "%d books", 3); // "%d книги"
     * GotText::unload("./ru_RU.mo");
     * var_export($gotText->isDummy()); // true
     * echo $gotText->_n("%d book", "%d books", 3); // %d books
     * var_export($gotText->getPluralsCount()); // 0
     * var_export($gotText->pluralFunc(3)); // 0
     * var_export($gotText->getFilename()); // "./ru_RU.mo"
     * var_export($gotText->getLocaleCode()); // ""
     * ```
     */
    public function isDummy(){}

    /**
     * Reloads a translation file.
     *
     * Purges the memory cache for the file and loads it from disk updating the memory cache.
     * This function will affect all GotText instances in all scripts that are currently running or will be running in the current PHP process.
     *
     * Throws __Exception__ on error.
     *
     * This function will reload a translation file only for the current PHP process.
     * You may want to use {@see getTimeCached()} in each process
     * to check if the file needs to be reloaded in that process.
     *
     * To reload a binary data use
     * ```php
     * new GotText($filename, $data);
     * ```
     * instead of this function. See {@see __construct()} for more details.
     *
     * @param string $filename A file to reload from disk.
     *
     * @return void
     *
     * @example
     * ```php
     * <?php
     * // let's assume that all scripts below are loaded from the same PHP process
     *
     * <?php // script test.php loads the file
     * $gotText = new GotText("./ru_RU.mo");
     * echo $gotText->_("Hello"); // "Привет"
     *
     * // let's assume that the file ./ru_RU.mo has been changed on disk
     * // the translation of "Hello" was changed to "Здравствуйте"
     * // still every consecutive invocation of test.php will yeild the same result "Привет"
     * // it's because the file is now loaded from memory, not from disk
     *
     * <?php // script reset.php reloads the file from disk
     * GotText::reload("./ru_RU.mo");
     *
     * <?php // script test.php loads the file
     * // still from memory, but now with the new contents
     * $gotText = new GotText("./ru_RU.mo");
     * echo $gotText->_("Hello"); // "Здравствуйте"
     * ```
     */
    public static function reload($filename){}

    /**
     * Unloads a translation file from memory.
     *
     * Almost the same as {@see reload()} but does not load the contents after cleaning the memory cache.
     * Next time GotText loads the file it will load it from disk, not memory. All consecutive loads will be again from memory.
     *
     * If the file was not loaded in the first place,
     * then this function does nothing.
     *
     * If PHP runs scripts simultaneously in several threads in one process,
     * then this function may unload all translations from a given file
     * in the middle of some other scripts,
     * which will cause these scripts to output untranslated messages
     * until some thread loads this file again.
     *
     * Therefore the use of this function in multi-threaded implementations is discouraged.
     *
     * The memory is not entirely cleared when calling this function.
     * Instead, the GotText object is replaced by a dummy GotText object with no data
     * (see {@see isDummy()}).
     * It means that each unloaded translation file will still occupy a small amount of memory
     * (less that 500 bytes)
     *
     * This function will unload a translation file only from the current PHP process.
     * You may want to use {@see getTimeCached()} in each process
     * to check if the file needs to be unloaded in that process.
     *
     * @param string $filename A file to unload from memory.
     *
     * @return void
     *
     * @example
     * ```php
     * <?php
     * // let's assume that all scripts below are loaded from the same PHP process
     * // and it's a multi-threaded PHP implementation
     *
     * <?php // script test.php starts
     * $gotText = new GotText("./ru_RU.mo");
     * echo $gotText->_("Hello"); // "Привет"
     *
     * // at this point of time, some other script invokes
     * // GotText::unload("./ru_RU.mo")
     *
     * // script test.php is still running, but all translations are now unloaded
     * echo $gotText->_("Hello"); // "Hello"
     * ```
     */
    public static function unload($filename){}

    /**
     * Creates GotText object if the file is already cached in memory.
     *
     * Checks if the file with the specified filename is in the memory cache.
     * If the translations are found in memory,
     * then a new GotText object is created and returned.
     * If there's no such file loaded in memory,
     * then the function returns __FALSE__.
     *
     * This function will also return __FALSE__
     * if the file is currently unloaded by {@see unload()}.
     *
     * @param string $filename A filename that refers to a desired translation file.
     *
     * @return GotText A GotText object if the file is found in memory.
     * __FALSE__ if the file was not loaded before or
     * if it's currently unloaded.
     *
     * @example
     * ```php
     * <?php
     * $langId = "Russian";
     * $gotText = GotText::get($langId);
     * if(!$gotText)
     * {
     *     $data = file_get_contents("./ru_RU.mo");
     *     $gotText = new GotText($langId, $data);
     * }
     * ```
     */
    public static function get($filename){}

    /**
     * Returns internal dictionary data.
     *
     * Builds and returns an associative array containing all dictionaries for the currently loaded translation resource.
     *
     * @return array A dictionary data.
     *
     * @example
     * ```php
     * <?php
     * $gotText = new GotText("./ru_RU.mo");
     * var_export($gotText->getStrings());
     *
     * // the output will be something like this
     * array (
     *   'singular' =>
     *   array (
     *     'Title' => 'Название',
     *     '' => 'Project-Id-Version:
     * POT-Creation-Date: 2016-01-25 20:56+0300
     * PO-Revision-Date: 2016-01-26 01:00+0300
     * Last-Translator:
     * Language-Team:
     * Language: ru_RU
     * MIME-Version: 1.0
     * Content-Type: text/plain; charset=UTF-8
     * Content-Transfer-Encoding: 8bit
     * X-Generator: Poedit 1.8.4
     * X-Poedit-Basepath: .
     * Plural-Forms: nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);
     * X-Poedit-KeywordsList: __;_n:1,2;_p:1c,2;_np:1c,2,3
     * X-Poedit-SearchPath-0: test.php
     * ',
     *   ),
     *   'plural' =>
     *   array (
     *     '%d site' =>
     *     array (
     *       0 => '%d место',
     *       1 => '%d места',
     *       2 => '%d мест',
     *     ),
     *   ),
     *   'singular_context' =>
     *   array (
     *     'Person' =>
     *     array (
     *       'Title' => 'Титул',
     *     ),
     *   ),
     *   'plural_context' =>
     *   array (
     *     'Web' =>
     *     array (
     *       '%d site' =>
     *       array (
     *         0 => '%d сайт',
     *         1 => '%d сайта',
     *         2 => '%d сайтов',
     *       ),
     *     ),
     *   ),
     * )
     * ```
     */
    public function getStrings(){}

    /**
     * Return a list of filenames of all previously loaded files.
     *
     * This list will also include filenames of all files that are currently unloaded by {@see unload()}.
     * To check if the file is actually loaded into memory use {@see get()}.
     *
     * @return array An array of filenames. The returned filenames will be exactly the same strings that were passed to {@see __construct()}.
     *
     * @example
     * ```php
     * <?php
     * // let's assume that all scripts below are loaded from the same PHP process
     *
     * <?php // script test1.php loads the first file
     * $gotText = new GotText("./ru_RU.mo");
     *
     * <?php // script test2.php loads the second file
     * $gotText = new GotText("Swedish", file_get_contents("./sv_SE.mo"));
     * GotText::unload("Swedish");
     *
     * <?php // script test3.php loads no translations
     * $gotText = new GotText();
     *
     * <?php // script result.php shows filenames of all currently loaded files
     * $filenames = GotText::getFilenames();
     * var_export($filenames);
     *
     * // the output will be
     * array (
     *   0 => './ru_RU.mo',
     *   1 => 'Swedish',
     * )
     * ```
     */
    public static function getFilenames(){}

    /**
     * Retrieves the plural form index for a given number.
     *
     * Returns a zero-based index of a plural form for a given number __n__.
     *
     * This function will always return zero for a dummy GotText object (see {@see isDummy()}).
     *
     * @param int $n A number. Currently only positive integers are handled correctly.
     *
     * @return int A plural form index.
     *
     * @example
     * ```php
     * <?php
     * $gotText = new GotText("./ru_RU.mo");
     * $pluralForms = array("%d книга", "%d книги", "%d книг");
     * $number = 22;
     * $pluralFormIndex = $gotText->pluralFunc($number); // 1
     * $pluralForm = $pluralForms[$pluralFormIndex]; // "%d книги"
     * echo sprintf($pluralForm, $number); // "22 книги"
     *
     * $dummy = new GotText();
     * echo $dummy->pluralFunc(42); // 0
     * ```
     */
    public function pluralFunc($n){}

    /**
     * Returns the time the current file was cached in memory.
     *
     * Returns the timestamp the translations in the current file were last reloaded and put in memory cache.
     *
     * This function will always return zero for a dummy GotText object (see {@see isDummy()}).
     *
     * @return int A UNIX timestamp.
     *
     * @example
     * ```php
     * <?php
     * $gotText = new GotText("./ru_RU.mo");
     * // reload the file if it was changed on disk since the last caching
     * if($gotText->getTimeCached() < filemtime($filename))
     *     GotText::reload($filename);
     *
     * $dummy = new GotText();
     * echo $dummy->getTimeCached(); // 0
     * ```
     */
    public function getTimeCached(){}

    /**
     * Returns the filename associated with the the currently loaded file.
     *
     * The returned filename will be exacly the same string that was passed to {@see __construct()}.
     *
     * @return string The filename of the currently loaded resource.
     *
     * ```php
     * <?php
     * $gotText = new GotText("./ru_RU.mo");
     * echo $gotText->getFilename(); // "./ru_RU.mo"
     * GotText::unload("./ru_RU.mo");
     * echo $gotText->getFilename(); // "./ru_RU.mo"
     *
     * $dummy = new GotText();
     * echo $dummy->getFilename(); // ""
     * ```
     */
    public function getFilename(){}

    /**
     * Returns the locale code.
     *
     * Returns the locale code for the current locale. This value is taken from the __Language__ header of MO file.
     *
     * This function will always return an empty string for a dummy GotText object (see {@see isDummy()}).
     *
     * @return string A locale code.
     *
     * @example
     * ```php
     * <?php
     * $gotText = new GotText("./ru_RU.mo");
     * echo $gotText->getLocale(); // "ru_RU"
     *
     * $dummy = new GotText();
     * echo $dummy->getLocaleCode(); // ""
     * ```
     */
    public function getLocaleCode(){}

    /**
     * Returns the number of plural forms.
     *
     * Returns the number of plural forms for the language/locale in the currently loaded file. The number of plural forms is hardcoded for each language/locale and may not correspond with data found in MO file headers.
     *
     * This function will always return zero for a dummy GotText object (see {@see isDummy()}).
     *
     * @return int A number of plural forms.
     *
     * @example
     * ```php
     * <?php
     * $gotText = new GotText("./ru_RU.mo");
     * echo $gotText->getPluralsCount(); // 3
     *
     * $dummy = new GotText();
     * echo $dummy->getPluralsCount(); // 0
     * ```
     */
    public function getPluralsCount(){}

    /**
     * Returns some information about GotText extension.
     *
     * Returns version information, build timestamp and build options.
     *
     * @return array An associative array with the following fields:
     *
     * * __major__ - major version number (__1__.2.3);
     * * __minor__ - minor version number (1.__2__.3);
     * * __patch__ - patch version number (1.2.__3__);
     * * __timestamp__ - build timestamp;
     * * __thread_safe__ - __TRUE__ if GotText is thread-safe
     *   (__THREAD_SAFE__ build flag);
     * * __native_file__ - __TRUE__ if GotText uses native file functions instead of fopen()
     *   (__NATIVE_FILE__ build flag);
     * * __debug__ - __TRUE__ if this is a debug version of GotText
     *   (__DEBUG__ build flag);
     * * __boost_regex__ - __TRUE__ if Boost.Regex is used instead of built-in GCC regular expression library
     *   (__BOOST_REGEX__ build flag).
     * * __standalone__ - __TRUE__ if GotText is compiled as a standalone library
     *   (__STANDALONE__ build flag).
     *
     * @example
     * ```php
     * <?php
     * var_export(GotText::getInfo());
     *
     * // will output something like this
     * array (
     *   'major' => 1,
     *   'minor' => 0,
     *   'patch' => 2,
     *   'timestamp' => 1456753020,
     *   'thread_safe' => false,
     *   'native_file' => false,
     *   'debug' => false,
     *   'boost_regex' => true,
     *   'standalone' => true,
     * )
     * ```
     */
    public static function getInfo(){}
}
