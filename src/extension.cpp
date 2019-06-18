/*************************************************************************}
{ extension.cpp - extension bindings to GotText engine                    }
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

#include <iomanip>
#include <sstream>

#include <phpcpp.h>

#include "gottext.h"

// Specify the following directive to instruct GotText extension
// to use std::ifstream for reading files.
// Otherwise, PHP functions (fopen, fread, ...) will be used.
#ifndef GOTTEXT_EXT_NATIVE_FILE
    #include "phpreadstream.h"
#endif

/*!
 * Implementation that retrieves the timestamp via PHP function time()
 * to be consistent with the calling PHP code.
 * By default it also uses PHP functions to access files
 * instead of the native std::ifstream.
 */
class GotTextCustom : public GotText::GotText {
protected:
#ifndef GOTTEXT_EXT_NATIVE_FILE
    virtual ::GotText::Lang loadFromFile(const std::string& filename)
    {
        try{
            PhpReadStream f(filename);
            f.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
            try{
                return loadFromStream(f, filename);
            }catch(std::ios_base::failure &e){
                throw ::GotText::Exception(::GotText::Exception::ReadError, f);
            }
        }catch(std::ios_base::failure &e){
            throw ::GotText::Exception(::GotText::Exception::ReadError, 0, e.what());
        }
    }
#endif
    virtual time_t getTimestamp() const
    {
        return Php::call("time");
    }
};

/*!
 * PHP extension interface for GotText.
 */
class GotTextExtension : public Php::Base
{
protected:
    GotTextCustom gotText; /*!<
        GotText instance.
        Avoiding multiple inheritance.
    */

public:
    /*!
     * See GotText::_().
     */
    Php::Value _(Php::Parameters &params) const
    {
        // the read lock is inside this function
        return gotText._(params[0]);
    }

    /*!
     * See GotText::_n().
     */
    Php::Value _n(Php::Parameters &params) const
    {
        // the read lock is inside this function
        return gotText._n(params[0], params[1], params[2]);
    }

    /*!
     * See GotText::_p().
     */
    Php::Value _p(Php::Parameters &params) const
    {
        // the read lock is inside this function
        return gotText._p(params[0], params[1]);
    }

    /*!
     * See GotText::_np().
     */
    Php::Value _np(Php::Parameters &params) const
    {
        // the read lock is inside this function
        return gotText._np(params[0], params[1], params[2], params[3]);
    }

    /*!
     * Retrieves a plural form index for a given number.
     */
    Php::Value pluralFunc(Php::Parameters &params) const
    {
        GOTTEXT_READ_LOCK
        return gotText.getLang().pluralInfo.func(params[0]);
    }

    /*!
     * Builds an associative array containing all dictionaries from GotText::getLang().
     */
    Php::Value getStrings() const
    {
        GOTTEXT_READ_LOCK
        const GotText::Lang& thisLang = gotText.getLang();
        Php::Value dicts;
        dicts["singular"] = umapToVal(thisLang.dictOne);
        dicts["plural"] = umapToVal(thisLang.dictNum);
        Php::Value dict(Php::Type::Array);
        for(auto& i : thisLang.dictCtxOne)
            dict[i.first] = umapToVal(i.second);
        dicts["singular_context"] = std::move(dict);
        dict = Php::Value(Php::Type::Array);
        for(auto& i : thisLang.dictCtxNum)
            dict[i.first] = umapToVal(i.second);
        dicts["plural_context"] = std::move(dict);
        return dicts;
    }

    /*!
     * Returns the time the current language translations was last reloaded.
     */
    Php::Value getTimeCached() const
    {
        GOTTEXT_READ_LOCK
        return static_cast<int64_t>(gotText.getLang().time);
    }

    /*!
     * Returns the filename associated with the the current language translations.
     */
    Php::Value getFilename() const
    {
        GOTTEXT_READ_LOCK
        return gotText.getFilename();
    }

    /*!
     * Returns the current locale code.
     * Example locale codes: ru_RU, sv, sah.
     */
    Php::Value getLocaleCode() const
    {
        GOTTEXT_READ_LOCK
        return gotText.getLang().locale;
    }

    /*!
     * Returns the number of plural forms for the current language/locale.
     */
    Php::Value getPluralsCount() const
    {
        GOTTEXT_READ_LOCK
        return static_cast<int>(gotText.getLang().pluralInfo.count);
    }

    /*!
     * Return a list of filenames of loaded files
     * including files that were unloaded later.
     */
    static Php::Value getFilenames()
    {
        GOTTEXT_READ_LOCK
        std::vector<std::string> filenames;
        const GotText::LangStorage& storage = GotText::GotText::getStorage();
        for(auto i : storage)
            filenames.emplace_back(i.first);
        return filenames;
    }

    /*!
     * Reloads a particular translation.
     */
    static void reload(Php::Parameters &params)
    {
        GotTextCustom inst;
        inst.load(params[0], true); // the write lock is inside this function
    }

    /*!
     * See GotText::unload().
     */
    static void unload(Php::Parameters &params)
    {
        // the write lock is inside this function
        GotText::GotText::unload(params[0]);
    }

    /*!
     * Returns a new GotText object if a specified file is cached in memory.
     * Returns false if the file is not found in memory,
     * or if it was unloaded via unload().
     */
    static Php::Value get(Php::Parameters &params)
    {
        GOTTEXT_READ_LOCK
        if(!GotText::GotText::isLoaded(params[0]))
            return false;
        return Php::Object("GotText", params[0]); // nested read locks are ok
    }

    /*!
     * See GotText::isDummy().
     */
    Php::Value isDummy() const
    {
        GOTTEXT_READ_LOCK
        return gotText.isDummy();
    }

    /*!
     * Returns version information.
     */
    static Php::Value getInfo()
    {
        Php::Value info;
        info["major"] = VER_MAJ;
        info["minor"] = VER_MIN;
        info["patch"] = VER_PAT;
        info["timestamp"] = BUILD_TIMESTAMP;
        info["thread_safe"] =
#ifndef GOTTEXT_NO_THREADSAFE
                true;
#else
                false;
#endif
        info["native_file"] =
#ifdef GOTTEXT_EXT_NATIVE_FILE
                true;
#else
                false;
#endif
        info["debug"] =
#ifdef GOTTEXT_EXT_DEBUG
                true;
#else
                false;
#endif
        info["boost_regex"] =
#ifdef GOTTEXT_BOOST_REGEX
                true;
#else
                false;
#endif
        return info;
    }

    /*!
     * PHP object constructor
     */
    void __construct(Php::Parameters &params)
    {
        try{
            switch(params.size())
            {
                case 0:
                    break;

                case 1:
                    gotText.load(params[0]);
                    break;

                default:
                    std::stringstream s(params[1].stringValue(), std::ios_base::in);
                    gotText.load(params[0], s);
            }
        }catch(const GotText::Exception &e){
            throwPhpException(e);
        }
    }

    virtual ~GotTextExtension() {}

protected:
    /*!
     * Returns a hexadecimal string representation of an integer.
     */
    template<typename T>
    std::string hex(T n)
    {
        std::stringstream str;
        str << std::showbase
            << std::setiosflags(std::ios::internal)
            << std::setfill('0')
            << std::setw(sizeof(T)*2)
            << std::hex
            << n;
        return str.str();
    }

    /*!
     * Converts GotText::Exception to a human readable Php::Exception.
     */
    void throwPhpException(const GotText::Exception &e)
    {
        std::string s("GotText ERROR ");
        s.append(std::to_string(e.type));
        s.append(". ");

        switch(e.type)
        {
            case GotText::Exception::NoError:
                s.append("No Error");
                break;

            case GotText::Exception::UnknownMagicNumber:
                s.append("Magic number mismatch. Expected: ");
                s.append(hex(GotText::MO_MAGIC_NUMBER));
                s.append(", got ");
                s.append(hex(e.intParam));
                break;

            case GotText::Exception::UnsupportedVersion:
                s.append("Unsupported version. Expected maximum: ");
                s.append(std::to_string(GotText::MO_MAX_SUPPORTED_VERSION));
                s.append(", got ");
                s.append(std::to_string(e.intParam));
                break;

            case GotText::Exception::NoTranslations:
                s.append("No translations found");
                break;

            case GotText::Exception::NoHeaders:
                s.append("Headers not found");
                break;

            case GotText::Exception::NoLanguageHeader:
                s.append("No valid Language header found. Expected: \"Language: xx_XX\". Got headers: ");
                s.append(e.strParam);
                break;

            case GotText::Exception::TooManySourceForms:
                s.append("There are more than two forms for this source string: ");
                s.append(e.strParam);
                s.append(". Found source forms: ");
                s.append(std::to_string(e.intParam));
                break;

            case GotText::Exception::TooManyNonPluralTranslations:
                s.append("There are more than one translation for this non-plural string: ");
                s.append(e.strParam);
                s.append(". Found plural forms: ");
                s.append(std::to_string(e.intParam));
                break;

            case GotText::Exception::InvalidPluralFormsCount:
                s.append("Invalid plural forms count for this string: ");
                s.append(e.strParam);
                s.append(". Found plural forms: ");
                s.append(std::to_string(e.intParam));
                break;

            case GotText::Exception::ReadError:
                s.append("Read error");
                if(e.strParam.size())
                {
                    s.append(". ");
                    s.append(e.strParam);
                }
                break;

            default:
                s.append("Unknown error.");
                break;
        }

        s.append(". Stream pos: ");
        s.append(std::to_string(e.filePos));
        if(e.errNo)
        {
            s.append(". Last errno: ");
            s.append(std::to_string(e.errNo));
            s.append(" (");
            s.append(strerror(e.errNo));
            s.append(").");
        }
        else
        {
            s.append(".");
        }

        throw Php::Exception(s);
    }

    /*!
     * Helper function that converts unordered_map to associated PHP array.
     */
    template<typename T>
    Php::Value umapToVal(const std::unordered_map<std::string, T>& map) const
    {
        Php::Value v(Php::Type::Array);
        for(auto& i : map)
            v[i.first] = Php::Value(i.second);
        return v;
    }
};

/*!
 * The only exported function, which returns PHP bindings to C++ code.
 */
extern "C" PHPCPP_EXPORT void *get_module()
{
    static Php::Extension extension("gottext", VERSION_STR);

    Php::Class<GotTextExtension> gotTextClass("GotText");
    gotTextClass.method<&GotTextExtension::getInfo>("getInfo");
    gotTextClass.method<&GotTextExtension::__construct>("__construct", {
        Php::ByVal("filename", Php::Type::String, false),
        Php::ByVal("data", Php::Type::String, false)
    });
    gotTextClass.method<&GotTextExtension::reload>("reload", {
        Php::ByVal("filename", Php::Type::String, true)
    });
    gotTextClass.method<&GotTextExtension::unload>("unload", {
        Php::ByVal("filename", Php::Type::String, true)
    });
    gotTextClass.method<&GotTextExtension::get>("get", {
        Php::ByVal("filename", Php::Type::String, true)
    });
    gotTextClass.method<&GotTextExtension::isDummy>("isDummy");
    gotTextClass.method<&GotTextExtension::_>("_", {
        Php::ByVal("msgid", Php::Type::String, true)
    });
    gotTextClass.method<&GotTextExtension::_n>("_n", {
        Php::ByVal("msgid", Php::Type::String, true),
        Php::ByVal("msgid_plural", Php::Type::String, true),
        Php::ByVal("n", Php::Type::Numeric, true)
    });
    gotTextClass.method<&GotTextExtension::_p>("_p", {
        Php::ByVal("msgid_ctxt", Php::Type::String, true),
        Php::ByVal("msgid", Php::Type::String, true)
    });
    gotTextClass.method<&GotTextExtension::_np>("_np", {
        Php::ByVal("msgid_ctxt", Php::Type::String, true),
        Php::ByVal("msgid", Php::Type::String, true),
        Php::ByVal("msgid_plural", Php::Type::String, true),
        Php::ByVal("n", Php::Type::Numeric, true)
    });
    gotTextClass.method<&GotTextExtension::getTimeCached>("getTimeCached");
    gotTextClass.method<&GotTextExtension::getFilename>("getFilename");
    gotTextClass.method<&GotTextExtension::getLocaleCode>("getLocaleCode");
    gotTextClass.method<&GotTextExtension::getPluralsCount>("getPluralsCount");
    gotTextClass.method<&GotTextExtension::getStrings>("getStrings");
    gotTextClass.method<&GotTextExtension::getFilenames>("getFilenames");
    gotTextClass.method<&GotTextExtension::pluralFunc>("pluralFunc", {
        Php::ByVal("n", Php::Type::Numeric, true)
    });

    extension.add(std::move(gotTextClass));

    return extension;
}
