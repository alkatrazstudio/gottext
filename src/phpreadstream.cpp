/*************************************************************************}
{ phpreadstream.cpp - file reader that uses PHP functions to read files   }
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

#include "phpreadstream.h"

static const int PHP_READ_BUFFER_LEN = 8192;

PhpReadBuffer::PhpReadBuffer(const std::string &filename) :
    std::streambuf(),
    f(Php::call("fopen", filename, "rb")),
    realPos(0)
{
    if(f.isBool())
        throwLastError();
    char* p = &buffer[0];
    setg(p, p, p);
}

PhpReadBuffer::~PhpReadBuffer()
{
    if(!f.isBool())
        Php::call("fclose", f);
}

void PhpReadBuffer::throwLastError()
{
    Php::Value msg = Php::call("error_get_last")["message"];
    throw std::ios_base::failure(msg.stringValue());
}

std::streambuf::int_type PhpReadBuffer::underflow()
{
    if(gptr() < egptr())
        return traits_type::to_int_type(*gptr());

    Php::Value v = Php::call("fread", f, PHP_READ_BUFFER_LEN);
    if(f.isBool())
        return traits_type::eof();
    std::string s = v.stringValue();
    if(!s.size())
        return traits_type::eof();
    realPos += s.size();
    std::swap(s, buffer);

    char* p = &buffer[0];
    setg(p, p, p + buffer.size());

    return traits_type::to_int_type(*gptr());
}

std::streampos PhpReadBuffer::seekpos(std::streampos pos, std::ios_base::openmode which)
{
    if(!(which & std::ios_base::in))
        return pos_type(off_type(-1));

    int64_t bufOffset = static_cast<int64_t>(pos) - (realPos - buffer.size());
    if(bufOffset < 0 || static_cast<uint64_t>(bufOffset) > buffer.size())
    {
        Php::Value result = Php::call("fseek", f, pos);
        if(result == -1)
            return pos_type(off_type(-1));
        realPos = pos;
        setg(eback(), eback(), eback());
    }
    else
    {
        setg(eback(), eback()+bufOffset, egptr());
    }

    return pos;
}

std::streampos PhpReadBuffer::seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which)
{
    if(!(which & std::ios_base::in))
        return std::streampos(std::streamoff(-1));

    int64_t posBefore = realPos - (egptr() - gptr());
    int64_t offset = static_cast<int64_t>(off);
    int64_t posAfter;
    switch(way)
    {
        case std::ios_base::beg:
            posAfter = offset;
            break;

        case std::ios_base::cur:
            posAfter = posBefore + offset;
            break;

        default:
            return std::streampos(std::streamoff(-1));
    }

    return seekpos(posAfter, which);
}

PhpReadStream::PhpReadStream(const std::string &filename) :
    std::istream(new PhpReadBuffer(filename))
{
}

PhpReadStream::~PhpReadStream()
{
    delete rdbuf();
}
