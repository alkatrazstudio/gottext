/*************************************************************************}
{ gottext.h - the engine                                                  }
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

#include <map>
#include <unordered_map>
#include <vector>

#include "plural.h"
#include "exception.h"

// Specify the following directive to disable thread-safety.
#ifndef GOTTEXT_NO_THREADSAFE
    #include <boost/thread/locks.hpp>
    #include <boost/thread/shared_mutex.hpp>

    #define GOTTEXT_READ_LOCK boost::shared_lock<boost::shared_mutex> _gottext_lock(::GotText::GotText::_gottext_mutex);
    #define GOTTEXT_WRITE_LOCK boost::unique_lock<boost::shared_mutex> _gottext_lock(::GotText::GotText::_gottext_mutex);
    #define GOTTEXT_READ_UPGRADE_LOCK boost::upgrade_lock<boost::shared_mutex> _gottext_lock(::GotText::GotText::_gottext_mutex);
    #define GOTTEXT_UPGRADE_LOCK_TO_WRITE boost::upgrade_to_unique_lock<boost::shared_mutex> _gottext_writeLock(_gottext_lock);
#else
    #define GOTTEXT_READ_LOCK
    #define GOTTEXT_WRITE_LOCK
    #define GOTTEXT_READ_UPGRADE_LOCK
    #define GOTTEXT_UPGRADE_LOCK_TO_WRITE
#endif

namespace GotText {
    static const uint32_t MO_MAX_SUPPORTED_VERSION = 0; /*!< Maximum supported *.mo files version. */
    static const uint32_t MO_MAGIC_NUMBER = 0x950412de; /*!< Magic number for *.mo files. */

    typedef std::unordered_map<std::string, std::string> DictOne;
    typedef std::unordered_map<std::string, std::vector<std::string>> DictNum;
    typedef std::unordered_map<std::string, DictOne> DictCtxOne;
    typedef std::unordered_map<std::string, DictNum> DictCtxNum;

    /*!
     * Translations and other info for a single language/locale.
     */
    struct Lang {
        time_t time = 0; /*!<
            A timestamp provided by GotText::getTimestamp().
            The default GotText implementation does not use this value internally
            after assigning a value to it.
        */
        std::string locale; /*!<
            Locale, i.e. ru_RU, sv, sah.
            The default GotText implementation does not use this value internally
            after assigning a value to it.
        */
        Plural::Info pluralInfo; /*!< see Plural::Info. */
        DictOne dictOne; /*!< A dictionary for GotText::_(). */
        DictNum dictNum; /*!< A dictionary for GotText::_n(). */
        DictCtxOne dictCtxOne; /*!< A dictionary for GotText::_p(). */
        DictCtxNum dictCtxNum; /*!< A dictionary for GotText::_np(). */

        void swap(Lang &&other); /*!< Swap two translation objects. */

        /*!
         * Returns true if it's a dummy/invalid Lang object.
         * The object is a dummy object if it contains no translation data.
         * All the fields of such objects are initialized to default values.
         */
        inline bool isDummy() const {return !pluralInfo.isValid();}
    };

    typedef std::map<std::string, Lang> LangStorage;

    /*!
     * Core class providing all base functionality:
     * loading and parsing files,
     * translating the strings,
     * managing the global storage.
     * All functions of this class are thread-safe unless otherwise stated.
     * However, if GOTTEXT_NO_THREADSAFE directive is specified then
     * there's no thread-safety at all.
     */
    class GotText
    {
    protected:
        LangStorage::iterator lang; /*!<
            Iterator to the currently loaded language.
            MUST always be valid, i.e. MUST point to a dummy object if the translation is not loaded.
        */

    public:
#ifndef GOTTEXT_NO_THREADSAFE
        static boost::shared_mutex _gottext_mutex; /*!<
            Helper mutex for GOTTEXT_*_LOCK macroses.
            DO NOT use directly.
        */
#endif

        explicit GotText();
        virtual ~GotText(){}
        GotText(const GotText& that);

        /*!
         * Load translations from a resource identified by *filename*.
         * If some translations is already loaded from this *filename*
         * then the translations will be reloaded only if *forceReload* == true.
         * Throws Exception on error.
         * If the translations are (re)loaded successfully
         * then they will be stored in the global storage.
         */
        void load(const std::string &filename, bool forceReload = false);
        void load(const std::string &filename, std::istream &stream);

        /*!
         * All GotText objects contain a global storage iterator
         * that contains a reference to a current language.
         * To prevent unnecessary validation and thread locks
         * this iterator must always remain valid.
         * Therefore the Lang objects cannot be completely removed from storage,
         * because it will invalidate all global storage iterators.
         * Instead, this function replaces the existing translations (Lang object)
         * that was loaded from the resource identified by *filename*
         * with a dummy translation object.
         * All memory occupied by that old translations is released.
         * However, the dummy object and the filename string will still be in memory.
         * If there are no translations associated with *filename*
         * then the function does nothing.
         */
        static void unload(const std::string &filename);

        /*!
         * Returns the currently loaded translations.
         * If no translation is loaded then the function returns the dummy translation object.
         * The returned object is dummy if its pluralInfo.count == 0.
         */
        inline const Lang& getLang() const {return (*lang).second;}

        /*!
         * Returns all current languages and their corresponsing translations
         * stored in the global storage
         * including unloaded (dummy) translations.
         */
        static const LangStorage& getStorage();

        /*!
         * Returns a filename of the currenly loaded file.
         * Returns an empty string if no translation is loaded.
         */
        inline std::string getFilename() const {return (*lang).first;}

        /*!
         * Returns a translation of *msgid*.
         * If no translation found then return *msgid* itself .
         * Similar to gettext(msgid).
         */
        std::string _(const std::string &msgid) const;

        /*!
         * Translates *msgid* and chooses a plural form.
         * If no translation is found then returns *msgid* if n == 1, or *msgid_plural* if n != 1.
         * Similar to ngettext(msgid, msgid_plural, n).
         */
        std::string _n(const std::string &msgid, const std::string &msgid_plural, int n) const;

        /*!
         * Behaves like _(), but also takes message context *msgid_ctxt* into account.
         * Similar to pgettext(msgid_ctxt, msgid).
         */
        std::string _p(const std::string &msgid_ctxt, const std::string &msgid) const;

        /*!
         * Behaves like _n(), but also takes message context *msgid_ctxt* into account.
         * Similar to npgettext(msgid_ctxt, msgid, msgid_plural, n).
         */
        std::string _np(const std::string &msgid_ctxt, const std::string &msgid, const std::string &msgid_plural, int n) const;

        /*!
         * Returns true if the file/stream with a specified *filename* is loaded.
         * Returns false if the file/stream was never loaded or
         * if it's currently unloaded.
         *
         * This function is NOT thread-safe! Use GOTTEXT_READ_LOCK.
         */
        static bool isLoaded(const std::string &filename);

        /*!
         * Returns true if it's a dummy/invalid GotText object.
         * The object is a dummy object if it contains no translations data.
         * See Lang::isDummy().
         *
         * This function is NOT thread-safe! Use GOTTEXT_READ_LOCK.
         */
        inline bool isDummy() const {return getLang().isDummy();}

    protected:
        /*!
         * Returns a timestamp, which will be stored in getLang().time.
         * Override to provide consistent timestamps for underlying implementation.
         * The default implementation uses std::chrono.
         * GotText itself does not rely on this value in any way.
         */
        virtual time_t getTimestamp() const;

        /*!
         * Loads a file from a specified location.
         * Throws Exception on error.
         * On success, updates global storage
         * and points *lang* to the loaded Lang struct.
         */
        void loadFile(const std::string& filename);

        /*!
         * Loads a file from a specified stream.
         * Throws Exception on error.
         * On success, updates the global storage
         * and points *lang* to the loaded Lang struct.
         */
        void loadStream(std::istream &s, const std::string& filename);

        /*!
         * Sets translations and updates the global storage.
         */
        void setLang(const std::string& filename, Lang &&other);

        /*!
         * Loads translations from a file.
         * If thread-safety is enabled, then this function is already thread-safe.
         * Therefore you MUST NOT use any GOTTEXT_*_LOCK in it.
         * This function SHOULD raise Exception on any error including read errors.
         */
        virtual Lang loadFromFile(const std::string& filename);

        /*!
         * Loads translations from a stream.
         * The stream is already opened and its position is at the start of the data.
         * If thread-safety is enabled, then this function is already thread-safe.
         * Therefore you MUST NOT use any GOTTEXT_*_LOCK in it.
         * This function SHOULD raise Exception on any error that is not a read error.
         * This function SHOULD NOT set the *time* field.
         */
        virtual Lang loadFromStream(std::istream& f, const std::string& filename);
    };

}
