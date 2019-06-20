/*************************************************************************}
{ plural.h - international plural forms information                       }
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

#include <string>

namespace GotText {
namespace Plural {

    using Func = int(int n);

    /*!
     * Plural information.
     */
    struct Info {
        Func* func; /*!<
            A function to retrieve a plural form index
            by a number.
        */

        size_t count {}; /*!< Number of plural forms */

        Info();
        Info(Func func, size_t count);

        /*!
         * Returns true if the plural information is valid/initialized.
         * Uninitialized Info object's *func* will always return zero
         * and *count* will also be zero.
         */
        inline bool isValid() const {return count != 0;}
    };

    /*!
     * Returns a plural form index for a default (English) language.
     */
    inline constexpr int origFunc(int n)
    {
        return n == 1 ? 0 : 1;
    }

    /*!
     * Returns a plural info for a *locale*.
     * Example locales: ru_RU, sv, sah.
     * See the source file plural.cpp for a full list of supported locales.
     */
    const Info &getInfo(const std::string& locale);

}}
