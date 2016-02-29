/*************************************************************************}
{ exception.h - specialized exceptions                                    }
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

#include <exception>
#include <fstream>

namespace GotText {

    /*!
     * Specialized exception class.
     */
    class Exception : public std::exception
    {
    public:
        /*!
         * Exception type.
         * When reimplementing Exception class,
         * try to provide information in Exception::intParam and Exception::strParam
         * that is similar to information provided by the default implementation.
         */
        enum Type : int {
            UnknownError = -1, /*!< Unknown error. Should not happen. */
            NoError = 0, /*!< No error. Not used in the default implementation. */

            UnknownMagicNumber, /*!<
                Unknown magic number found while trying to identify the file format.
                Exception::intParam will contain that number.
            */
            UnsupportedVersion, /*!<
                The version of the file is not supported by current implementation.
                Exception::intParam will contain that version.
            */
            NoTranslations, /*!< No translations found in file. */
            NoHeaders, /*!< Headers were expected but not found. */
            NoLanguageHeader, /*!<
                Header with language info was expected but not found.
                Exception::strParam will contain all headers.
            */
            TooManySourceForms, /*!<
                More than 2 plural forms for a source string.
                Exception::intParam will contain the number of plural forms found.
                Exception::strParam will contain the source string.
            */
            TooManyNonPluralTranslations, /*!<
                More than 1 translation form for a non-plural source string.
                Exception::intParam will contain the number of plural forms found.
                Exception::strParam will contain the source string.
            */
            InvalidPluralFormsCount, /*!<
                Number of plural forms does not match the locale.
                Exception::intParam will contain the number of plural forms found.
                Exception::strParam will contain the source string.
            */
            ReadError, /*!< Stream read error. */

            CustomError = 100 /*!< An anchor for custom error types. DO NOT use as an actual exception type. */
        };

        Type type; /*!< Type of the exception. */
        int errNo; /*!<
            The value of errno at the time this exception is thrown.
            May not provide any meaningful info if there wasn't any system error involved.
        */
        size_t filePos; /*!<
            The last reached position of the stream, if appllicable.
            Note that if the exception was thrown because the stream jumped to an invalid position,
            the *filePos* will contain that invalid position,
            not the last valid position.
        */
        uint32_t intParam; /*!< An integer. The meaning may vary depending on *type*. */
        std::string strParam; /*!< A string. The meaning may vary depending on *type*. */

    public:
        Exception() = delete;
        Exception operator =(const GotText::Exception& other) = delete;
        Exception(const GotText::Exception& other);
        Exception(GotText::Exception&& other);

        explicit Exception(Type type, size_t filePos = 0, const char *strParam = nullptr, uint32_t intParam = 0);
        explicit Exception(Type type, std::istream& f, const char *strParam = nullptr, uint32_t intParam = 0);
        explicit Exception(Type type, size_t filePos, const std::string& strParam, uint32_t intParam = 0);
        explicit Exception(Type type, std::istream& f, const std::string& strParam, uint32_t intParam = 0);
        explicit Exception(Type type, size_t filePos, std::string&& strParam, uint32_t intParam = 0);
        explicit Exception(Type type, std::istream& f, std::string&& strParam, uint32_t intParam = 0);

        virtual ~Exception() {}
    };

}
