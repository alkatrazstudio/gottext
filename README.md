GotText - Translation engine for PHP
====================================

GotText is an alternative to [PHP's gettext](https://secure.php.net/manual/book.gettext.php). It can load [MO files](https://www.gnu.org/software/gettext/manual/html_node/MO-Files.html) produced by [msgfmt](https://www.gnu.org/software/gettext/manual/html_node/msgfmt-Invocation.html).
GotText is not a drop-in replacement for gettext,
so read the below notes and [documentation](https://alkatrazstudio.gitlab.io/gottext/api/) before trying to use this extension.

The current version of GotText PHP extension is for PHP 7 and PHP 5 on Linux.

This extension is built using [PHP-CPP](http://www.php-cpp.com/) framework. The [main repository](https://github.com/CopernicaMarketingSoftware/PHP-CPP) of PHP-CPP is for PHP 7 only. If you want to build GotText for PHP 5 you need to use [PHP-CPP-LEGACY](https://github.com/CopernicaMarketingSoftware/PHP-CPP-LEGACY) repository.



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



Advantages over PHP's gettext
-----------------------------

### Straightforward file loading

This is how you load a translation file in gettext:

```php
setlocale(LC_MESSAGES, "ru_RU.utf8");
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


### Speed

GotText is 50-90% faster than gettext in some synthetic tests.


### Extra features

GotText has some extra functionality compared to gettext. For example, it can return all translations as an associative array. See the [documentation](https://alkatrazstudio.gitlab.io/gottext/api/) for a full list of available methods.



Unsupported gettext features
----------------------------

GotText has no concept of domains, categories and codesets. Therefore the following gettext functions has no alternatives in GotText:

* [bind_textdomain_codeset](https://secure.php.net/manual/function.bind-textdomain-codeset.php)
* [bindtextdomain](https://secure.php.net/manual/function.bindtextdomain.php)
* [dcgettext](https://secure.php.net/manual/function.dcgettext.php)
* [dcngettext](https://secure.php.net/manual/function.dcngettext.php)
* [dgettext](https://secure.php.net/manual/function.dgettext.php)
* [dngettext](https://secure.php.net/manual/function.dngettext.php)
* [textdomain](https://secure.php.net/manual/function.textdomain.php)

GotText also does not perform any charset conversions. It will match against and return the exact binary representation of strings that were located in the loaded MO file. It's recommended to create your MO files in UTF-8, and then treat all strings that are passed to or retreived from GotText as UTF-8.



open_basedir support
--------------------

GotText respects [open_basedir](https://secure.php.net/manual/ru/ini.core.php#ini.open-basedir) when it opens files, gettext - does not. By default, GotText uses PHP's [fopen()](https://secure.php.net/manual/function.fopen.php) to read files. Therefore __open_basedir__ restrictions are in order. If you don't want such restrictions, you may build GotText with a flag that instructs GotText to use native functions to read files. See the [build instructions](#installing-from-source) below for more details.



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

* `THREAD_SAFE=1` - build a thread-safe version of GotText. By default, GotText is not thread-safe. If you enable this option then you'll also have to install [Boost.Thread](http://www.boost.org/doc/libs/master/doc/html/thread.html): `sudo apt-get install libboost-thread-dev` for Ubuntu/Debian, `yum install boost-devel` for CentOS, or install an alternative package for your OS.
* `NATIVE_FILE=1` - instruct GotText to use a native API for reading files. By default, GotText reads files using PHP functions (fopen, fread, ...).
* `DEBUG=1` - build a debug version of the extension
* `BOOST_REGEX=1` - use [Boost.Regex](http://www.boost.org/doc/libs/master/libs/regex/doc/html/index.html) instead of std::regex. Use this option when your version of GCC does not support regular expressions (GCC < 4.9.0). If you enable this option then you'll also have to install Boost.Regex: `sudo apt-get install libboost-regex-dev` for Ubuntu/Debian, `yum install boost-devel` for CentOS, or install an alternative package for your OS.
* `PHP_VER=x.y` - use PHP version x.y instead of the auto-detected one. This is for internal development only.
* `PHPCPP_ROOT=<PHP-CPP install directory>` - assume that PHP-CPP root is installed under this diretory. This option adds `$PHPCPP_ROOT/include` to the header search paths, and `$PHPCPP_ROOT/lib` to the library search paths.
* `STANDALONE=1` - include all library dependencies inside GotText binary, so that it can be used without any external libraries (like Boost or PHP-CPP) at runtime. Note that this only work if all library dependencies are built with `-fPIC` compiler flag.

You may combine these options. For example, to install a debug thread-safe version of GotText that uses native file reading functions, run the following:

```bash
make THREAD_SAFE=1 NATIVE_FILE=1 DEBUG=1 && sudo make install
```

Shadow builds are supported. To enable shadow build just run make from a different directory.
For example:

```bash
# make a subfolder to hold builf artifacts
mkdir -p shadow
cd shadow

# point make to the Makefile in the root of the repository
make -f STANDALONE=1 PHPCPP_ROOT=/tmp/PHP-CPP/dist -f ../Makefile
```

If you've already built the extension and want to change some build options, then you should clean the previous build: `make clean`.



Uninstall
---------

To uninstall the extension, issue the following commands:
```bash
sudo phpdismod gottext # for PHP 5 use php5dismod if available
cd <path-to-gottext-repository-root>
sudo make uninstall
cd <path-to-PHP-CPP-repository-root>
sudo make uninstall
```
Then restart your web-server and/or PHP process manager if needed.

At the time of writing, PHP-CPP-LEGACY did not provide any means to uninstall itself via `make`.



Documentation
-------------

You can find the full documentation at https://alkatrazstudio.gitlab.io/gottext/api/.

The GotText docs are built with [apigen](http://www.apigen.org/). At the time of writing, apigen only supported PHP 5. To build or modify a local copy of the documentation follow these steps:

1. On CentOS you need to install [mbstring](https://secure.php.net/manual/book.mbstring.php) extension for PHP: `sudo yum install php-mbstring`.
2. Install __apigen__ if it's not already installed. See http://www.apigen.org/ for instructions.
3. Edit __doc/source/gottext.php__ if needed.
4. Invoke `make doc` to build the documentation into __doc/api__ folder.



Tests
-----

A self-test for GotText can be found in __test/test.php__. Run the following `make` targets to perform this test:

* `make test` - test the built extension in your current build directory (in a `dist` subfolder). The extension should not be enabled for PHP CLI system-wide or else you may expect an undefined behavior. On Ubuntu you can disable GotText for PHP CLI by invoking the following command: `sudo phpdismod -s cli gottext`. You can enable it back with `sudo phpenmod -s cli gottext`. For PHP 5 use `php5dismod`/`php5enmod`. On CentOS you need to comment out the contents of __/etc/php.d/gottext.ini__ to disable the extension. This test works also with the extension built via Docker.
* `make test_installed` - test the installed version of the extension.
