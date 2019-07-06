GotText changelog
=================

v2.0.0 (July 6, 2019)
---------------------

#### Breaking changes
- PHP 5 support is dropped. [d5d5d3bc]

#### New features
- `standalone` field in `getInfo()`. [c35c0003]

#### Improvements
- Updates for plural functions from [here](https://github.com/translate/l10n-guide/commits/master/docs/l10n/pluralforms.rst). [4f539a3b]
- Visually reworked documentation. [d49b482e]
- Stability optimizations. [fcd3b86d]
- Small memory consumption optimizations. [02f04114]

#### Bugfixes
- Fixed errors and inconsistencies in plural functions. [4f539a3b]

#### Internal changes
- New versioning system. [f82d6947]
- Standalone build. [ec604598]
- Ability to specify PHP-CPP directory for a build. [b8b50982]
- Support for shadow builds. [b55a85e4, 8a9a906f, 26363175]
- New doc build system [d49b482e]
- Docker builds and tests. [bd6979a3, 94bcfecd, 387a4d65, d49b482e]
- Benchmark test. [37b9ffb6]
- Code modernization. [2f73772c]
- Tests modernization. [38723963]



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
