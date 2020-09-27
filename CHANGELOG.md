GotText changelog
=================

v2.0.3 (September 27, 2020)
---------------------------

#### Internal changes
- Filesize and speed improvements
- IDE stubs added to the archive



v2.0.2 (September 26, 2020)
---------------------------

#### Internal changes
- Using GitHub Actions for CI



v2.0.1 (September 19, 2020)
---------------------------

#### New features
- Support for PHP 7.4.



v2.0.0 (July 6, 2019)
---------------------

#### Breaking changes
- PHP 5 support is dropped.

#### New features
- `standalone` field in `getInfo()`.

#### Improvements
- Updates for plural functions from [here](https://github.com/translate/l10n-guide/commits/master/docs/l10n/pluralforms.rst).
- Visually reworked documentation.
- Stability optimizations.
- Small memory consumption optimizations.

#### Bugfixes
- Fixed errors and inconsistencies in plural functions.

#### Internal changes
- New versioning system.
- Standalone build.
- Ability to specify PHP-CPP directory for a build.
- Support for shadow builds.
- New doc build system
- Docker builds and tests.
- Benchmark test.
- Code modernization.
- Tests modernization.



v1.1 (June 26, 2016)
--------------------

#### New features
- PHP 7 support.

#### Bugfixes
- Empty sub-dictionaries returned by getStrings() are now empty arrays instead of NULL.



v1.0 (February 29, 2016)
------------------------

#### New features
- First public release.
