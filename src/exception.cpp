/*************************************************************************}
{ exception.cpp - specialized exceptions                                  }
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

#include "exception.h"

namespace GotText {

    Exception::Exception(const Exception &other):
        std::exception(),
        type(other.type),
        errNo(other.errNo),
        filePos(other.filePos),
        intParam(other.intParam),
        strParam(other.strParam)
    {
    }

    Exception::Exception(Exception &&other):
        std::exception(),
        type(other.type),
        errNo(other.errNo),
        filePos(other.filePos),
        intParam(other.intParam),
        strParam(std::move(other.strParam))
    {
    }

    Exception::Exception(
            Type type,
            size_t filePos,
            const char* strParam,
            uint32_t intParam):
        std::exception(),
        type(type),
        errNo(errno),
        filePos(filePos),
        intParam(intParam),
        strParam(strParam ? strParam : "")
    {
    }

    Exception::Exception(
            Exception::Type type,
            std::istream &f,
            const char *strParam,
            uint32_t intParam
            ):
        std::exception(),
        type(type),
        errNo(errno),
        intParam(intParam),
        strParam(strParam ? strParam : "")
    {
        f.clear();
        filePos = f.tellg();
    }

    Exception::Exception(
            Exception::Type type,
            size_t filePos,
            const std::string &strParam,
            uint32_t intParam
            ):
        std::exception(),
        type(type),
        errNo(errno),
        filePos(filePos),
        intParam(intParam),
        strParam(strParam)
    {
    }

    Exception::Exception(
            Exception::Type type,
            std::istream &f,
            const std::string &strParam,
            uint32_t intParam):
        std::exception(),
        type(type),
        errNo(errno),
        intParam(intParam),
        strParam(strParam)
    {
        f.clear();
        filePos = f.tellg();
    }

    Exception::Exception(
            Exception::Type type,
            size_t filePos,
            std::string &&strParam,
            uint32_t intParam
            ):
        std::exception(),
        type(type),
        errNo(errno),
        filePos(filePos),
        intParam(intParam),
        strParam(std::move(strParam))
    {
    }

    Exception::Exception(
            Exception::Type type,
            std::istream &f,
            std::string &&strParam,
            uint32_t intParam):
        std::exception(),
        type(type),
        errNo(errno),
        intParam(intParam),
        strParam(std::move(strParam))
    {
        f.clear();
        filePos = f.tellg();
    }

}
