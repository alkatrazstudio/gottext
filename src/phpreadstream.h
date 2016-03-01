/*************************************************************************}
{ phpreadstream.h - file reader that uses PHP functions to read files     }
{                                                                         }
{ This file is a part of the project                                      }
{   GotText - translation engine with gettext-like features               }
{                                                                         }
{ (c) Alexey Parfenov, 2016                                               }
{                                                                         }
{ e-mail: zxed@alkatrazstudio.net                                         }
{                                                                         }
{ This library is free software; you can redistribute it and/or           }
{ modify it under the terms of the GNU General Public License             }
{ as published by the Free Software Foundation; either version 3 of       }
{ the License, or (at your option) any later version.                     }
{                                                                         }
{ This library is distributed in the hope that it will be useful,         }
{ but WITHOUT ANY WARRANTY; without even the implied warranty of          }
{ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU        }
{ General Public License for more details.                                }
{                                                                         }
{ You may read GNU General Public License at:                             }
{   http://www.gnu.org/copyleft/gpl.html                                  }
{*************************************************************************/

#pragma once
#include <istream>
#include <phpcpp.h>

/*!
 * Stream buffer that uses PHP functions to read from a file.
 * This class is made specifically for GotText PHP extension.
 */
class PhpReadBuffer : public std::streambuf
{
public:
    PhpReadBuffer() = delete;
    explicit PhpReadBuffer(const std::string& filename);
    virtual ~PhpReadBuffer();

protected:
    Php::Value f; /*!< file handle returned by PHP's fopen(). */
    std::string buffer; /*!< read buffer */
    int64_t realPos; /*!< actual read position in *f* */

    /*!
     * Converts the last PHP error/warning into Exception.
     */
    void throwLastError();

    /*!
     * Override for std::streambuf::underflow().
     * Gets the current character without changing the current position.
     */
    int_type underflow();

    /*!
     * Override for std::streambuf::seekpos().
     * Sets the absolute stream position.
     */
    std::streampos seekpos(std::streampos pos, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out);

    /*!
     * Override for std::streambuf::seekoff().
     * Alters the stream position in a specific way.
     */
    std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way,
                           std::ios_base::openmode which = std::ios_base::in | std::ios_base::out);
};

/*!
 * Stream for reading files using PhpReadBuffer.
 */
class PhpReadStream : public std::istream
{
public:
    PhpReadStream() = delete;
    explicit PhpReadStream(const std::string& filename);
    virtual ~PhpReadStream(){}

private:
    PhpReadBuffer buffer; /*!< the underlying buffer */
};
