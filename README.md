GotText - Translation engine for PHP
====================================

GotText is an alternative to [PHP's gettext](https://www.php.net/manual/book.gettext.php). It can load [MO files](https://www.gnu.org/software/gettext/manual/html_node/MO-Files.html) produced by [msgfmt](https://www.gnu.org/software/gettext/manual/html_node/msgfmt-Invocation.html).
GotText is not a drop-in replacement for gettext,
so read the below notes and [documentation](https://alkatrazstudio.gitlab.io/gottext/api/) before trying to use this extension.

The current version of GotText PHP extension is for PHP 7 on Linux.
The officially supported PHP versions are 7.2 and 7.3.

This extension is built using [PHP-CPP](https://www.php-cpp.com) framework.



Table of contents
-----------------
* [Advantages over PHP's gettext](#advantages-over-phps-gettext)
* [Unsupported gettext features](#unsupported-gettext-features)
* [open_basedir support](#open_basedir-support)
* [Example usage](#example-usage)
* [Installing from source](#installing-from-source)
* [Uninstall](#uninstall)
* [Documentation](#documentation)
* [Tests](#tests)
* [Benchmark](#benchmark)



Advantages over PHP's gettext
-----------------------------

### Straightforward file loading

This is how you load a translation file in gettext:

```php
setlocale(LC_MESSAGES, "ru_RU.UTF-8");
bindtextdomain("messages", "./locale");
textdomain("messages");
bind_textdomain_codeset("messages", "UTF-8");
```

And this is how it's done in GotText:

```php
$gotText = new GotText("./any_path/ru_RU.mo");
```

Both examples do not include any error checking, though.

Note, that with GotText you can load a translation file from any location
that can be opened using [fopen()](https://secure.php.net/manual/function.fopen.php).
You can even try to load MO files from remote URLs,
though it's not guaranteed to work.
GotText can also load translations from an arbitrary data string.

See the [documentation](https://alkatrazstudio.gitlab.io/gottext/api/) for more details.


### In-memory translations can be easily reloaded

It's nearly impossible to force gettext to reload the updated translation file from disk
without resorting to some dirty hacks.
GotText also caches all translations in memory like gettext,
but GotText offers a separate function to reload the translations from disk:

```php
GotText::reload("./ru_RU.mo");
```


### Thread-safety

GotText can be configured to be thread-safe.
It will cause 20% slowdown, but allows using this extension,
for example, in Apache with mpm_worker_module.

See the [build instructions](#installing-from-source) below for more details.


### Context support

GotText supports alternatives for the following [context-handling functions](https://www.gnu.org/software/gettext/manual/html_node/Contexts.html): `pgettext` and `npgettext`.
PHP's gettext does not support these functions out of the box.


### Extra features

GotText has some extra functionality compared to gettext. For example, it can return all translations as an associative array. See the [documentation](https://alkatrazstudio.gitlab.io/gottext/api/) for a full list of available methods.


### No locales

GotText doesn't care about locales. This, however, can be either beneficial or detrimental to your project depending on what you need.
All GotText does is take an untranslated string as an input and return the corresponding translation from MO file.
Gettext, on the other hand, makes use of system locales:

* It needs for a corresponding locale with a valid codeset to be present on your system.

* You need to switch your entire application (either thread or process) to the desired locale if you want to translate any string to that locale.
For example, it makes a process of simultaneous translation to different languages rather awkward.
Also, changing a locale for an entire process may have unwanted side-effects, since it may affect a code you don't control.

GotText requires none of that. It only needs an MO file. This is a non-intrusive approach. GotText doesn't affect any system settings, and system settings don't affect GotText.

However, if the rest of your application is also using system locales then you might need to consider all pros and cons of GotText not supporting locales.


### Speed

GotText is 50% faster than gettext in some synthetic tests.
See the [benchmark section](#benchmark) below for more details.



Unsupported gettext features
----------------------------

GotText has no concept of domains, categories and codesets. Therefore the following gettext functions has no alternatives in GotText:

* [bind_textdomain_codeset](https://php.net/manual/function.bind-textdomain-codeset.php)
* [bindtextdomain](https://php.net/manual/function.bindtextdomain.php)
* [dcgettext](https://php.net/manual/function.dcgettext.php)
* [dcngettext](https://php.net/manual/function.dcngettext.php)
* [dgettext](https://php.net/manual/function.dgettext.php)
* [dngettext](https://php.net/manual/function.dngettext.php)
* [textdomain](https://php.net/manual/function.textdomain.php)

GotText also does not perform any charset conversions. It will match against and return the exact binary representation of strings that were located in the loaded MO file. It's recommended to create your MO files in UTF-8, and then treat all strings that are passed to or retreived from GotText as UTF-8.



open_basedir support
--------------------

GotText respects [open_basedir](https://php.net/manual/ru/ini.core.php#ini.open-basedir) when it opens files, gettext - does not. By default, GotText uses PHP's [fopen()](https://php.net/manual/function.fopen.php) to read files. Therefore __open_basedir__ restrictions are in order. If you don't want such restrictions, you may build GotText with a flag that instructs GotText to use native functions to read files. See the [build instructions](#installing-from-source) below for more details.



Example usage
-------------

```php
<?php
try{
    // load the MO file;
    // it will be loaded from memory on consecutive loads
    $filename = "./ru_RU.mo";
    $gotText = new GotText($filename);

    // if the file was changed after it was cached in memory
    // then reload the translations from the disk
    if($gotText->getTimeCached() < filemtime($filename))
        GotText::reload($filename);
}catch(Exception $e){
    error_log($e->getMessage());

    // construct a dummy GotText object without translations
    // if the real file cannot be loaded
    $gotText = new GotText();
}

// a helper wrapper
function __($s)
{
    global $gotText;
    return $gotText->_($s);
}

echo __("Hello, World!");
```



Installing from source
----------------------

The recommended method is building via Docker.
The script `build/docker-build.sh` does all the job of building a statically-linked version of GotText (no extra dependencies).
This script tries to automatically detect the installed version of PHP on the host system.
You can also pass a specific PHP version number, which is actually a tag for a `php` Docker image.
The list of possible tags can be found here: https://hub.docker.com/_/php.

After running `docker-build.sh` the extension will be located at `dist/gottext.so`.
You can then install it via `make install`.

Usage example of `docker-build.sh` for building GotText for PHP 7.2:

```bash
# get the source code of GotText
git clone https://gitlab.com/alkatrazstudio/gottext.git
cd gottext

# start the Docker build (the version number can be omitted)
build/docker-build.sh 7.2

# install the extension
sudo make install

# enable the extension
sudo phpenmod gottext
```

After enabling the extension you may need to restart your web server and/or PHP process manager (e.g. PHP-FPM).

The actual build process that happens inside Docker container is described in `build/build.sh`.

When invoking `make` to build GotText you may specify the following options:

* `THREAD_SAFE=1` - build a thread-safe version of GotText. By default, GotText is not thread-safe. If you enable this option then you'll also have to install [Boost.Thread](http://www.boost.org/doc/libs/master/doc/html/thread.html): `sudo apt install libboost-thread-dev` for Ubuntu/Debian, `yum install boost-devel` for CentOS, or install an alternative package for your OS.
* `NATIVE_FILE=1` - instruct GotText to use a native API for reading files. By default, GotText reads files using PHP functions (fopen, fread, ...).
* `DEBUG=1` - build a debug version of the extension
* `BOOST_REGEX=1` - use [Boost.Regex](http://www.boost.org/doc/libs/master/libs/regex/doc/html/index.html) instead of std::regex. Use this option when your version of GCC does not support regular expressions (GCC < 4.9.0). If you enable this option then you'll also have to install Boost.Regex: `sudo apt install libboost-regex-dev` for Ubuntu/Debian, `yum install boost-devel` for CentOS, or install an alternative package for your OS.
* `PHP_VER=x.y` - use PHP version x.y instead of the auto-detected one. This is for internal development only.
* `PHPCPP_ROOT=<PHP-CPP install directory>` - assume that PHP-CPP root is installed under this diretory. This option adds `$PHPCPP_ROOT/include` to the header search paths, and `$PHPCPP_ROOT/lib` to the library search paths.
* `STANDALONE=1` - include all library dependencies inside GotText binary, so that it can be used without any external libraries (like Boost or PHP-CPP) at runtime. Note that this only work if all library dependencies are built with `-fPIC` compiler flag.
* `INI_DIR=<extension configuration files directory>` - specify a directory where `gottext.ini` file needs to be put. You can also use `PHP_INI_DIR` environment variable to specifiy this folder. `INI_DIR` options takes precedence. This path is automatically deducted for Ubuntu and CentOS distributions and for `php` Docker images.

You may combine these options. For example, to install a debug thread-safe version of GotText that uses native file reading functions, run the following:

```bash
make THREAD_SAFE=1 NATIVE_FILE=1 DEBUG=1 && sudo make install
```

Shadow builds are supported. To enable shadow build just run make from a different directory.
For example:

```bash
# make a subfolder to hold build artifacts
mkdir -p shadow
cd shadow

# point make to the Makefile in the root of the repository
make STANDALONE=1 PHPCPP_ROOT=/tmp/PHP-CPP/dist -f ../Makefile

# install the extension from the same shadow directory
make INI_DIR=/foo/bar -f ../Makefile install
```

If you've already built the extension and want to change some build options, then you should clean the previous build: `make clean`.



Uninstall
---------

To uninstall the extension, issue the following commands:
```bash
sudo phpdismod gottext
cd <path-to-gottext-repository-root>
sudo make uninstall
cd <path-to-PHP-CPP-repository-root>
sudo make uninstall
```
Then restart your web-server and/or PHP process manager if needed.



Documentation
-------------

You can find the full documentation at https://alkatrazstudio.gitlab.io/gottext/api/.

The GotText docs are built with [ApiGen](https://github.com/ApiGen/ApiGen) v4. This version only supports PHP 5. The recommended way to build the documentation is to use `doc/docker-build.sh` script which uses Docker images to run ApiGen. After running this script the resulting documentation will be located in `doc/dist`. The actual build instructions are described in `doc/docker-build.sh`.

Documentation is based on `doc/source/gottext.php` file. You can also use this file in your IDE to enable auto-completition, etc.



Tests
-----

A self-test for GotText can be found in __test/test.php__. Run the following `make` targets to perform this test:

* `make test` - test the built extension in your current build directory (in a `dist` subfolder). The extension should not be enabled for PHP CLI system-wide or else you may expect an undefined behavior. On Ubuntu you can disable GotText for PHP CLI by invoking the following command: `sudo phpdismod -s cli gottext`. You can enable it back with `sudo phpenmod -s cli gottext`. This test works also with the extension built via Docker.
* `make test_installed` - test the installed version of the extension.

You can also run this test inside a Docker container. Run the script `test/docker-test.sh` to start the test.
This script will try to detect your currently installed PHP version and run a test against the appropriate Docker image.
However, you can specify a different PHP version number, the same way as in [build instructions](#installing-from-source) for a `build/docker-build.sh` script.

Example of building the GotText extension for PHP 7.3 and then performing a test:

```bash
build/docker-build.sh 7.3
test/docker-test.sh 7.3
```



Benchmark
---------

In the `benchmark` directory there is a test that lets you compare the performance of GotText vs gettext.
This test translates 100K randomly generated strings, including strings with pluralization, using both engines and then prints out the timings breakdown.

To be able to run the test you need both GotText and gettext PHP extensions installed on your machine.
Also, Python 3 and gettext CLI tools (msgfmt) are required.
To install everything above on Ubuntu (except GotText itself) you may run `sudo apt install php-cli php-gettext python3 gettext`.

To start the benchmark, run `benchmark/run.sh` script. The results will be something like this:

                                GotText   gettext
    ---------------------------------------------
     cold initialization (ms) -   88         0
       re-initialization (ms) -    0         0
        translate 1 pass (ms) -   58       198
             miss 1 pass (ms) -   41       145
    translate 100 passes (ms) -   56        88
         miss 100 passes (ms) -   40       142

The numbers are timings in milliseconds. Description of timings:

* __cold initialization__ - first time initialization. GotText parses MO file when its instance is constructed, but gettext delays loading the file until the actual translation is needed. However, both GotText and gettext only need to load MO file one time, so this timing should not affect the overall performance of the application, unless it spawns a new PHP process on each request.

* __re-initialization__ - initializing the engine again. This only makes sense for GotText. It shows how much time is needed to construct a GotText object that refers to an already loaded MO file. This number should be very small or zero, because GotText does not actually do anything with the file after it was loaded and parsed the first time.

* __translate 1 pass__ - first time translating 100K strings in a row. gettext timing here also includes the time needed to load and parse the MO file. So, for both engines, "cold translation" time is "cold initialization" time plus "translate 1 pass" time.

* __miss 1 pass__ - here, "miss" means the translation attempt that did not succeeded, i.e. the translation was not found for a requested string. One pass of "misses" equals to 100K failed translation attempts in a row. This metric may be important if you expect a lot of strings to be untranslated.

* __translate 100 passes__ - the same as "translate 1 pass" but do it 100 times and the timing is the average timing for these attempts. This is the metric that matters the most. It shows the speed of translation when all caches are prepared.

* __miss 100 passes__ - the same as "translate 100 passes" but when all translation attempts fail.
