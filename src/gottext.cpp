/*************************************************************************}
{ gottext.cpp - the engine                                                }
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

#include "gottext.h"

#include <fstream>
#include <cstdint>
#include <algorithm>

#include <cstdio>
#include <chrono>

// Specify the following directive to use boost::regex instead of std::regex.
#ifdef GOTTEXT_BOOST_REGEX
    #include <boost/regex.hpp>
#else
    #include <regex>
#endif

#include <phpcpp.h>

namespace GotText {

#ifndef GOTTEXT_NO_THREADSAFE
    boost::shared_mutex GotText::_gottext_mutex;
#endif

    struct StrIndex {
        int index;
        uint32_t offset;
        uint32_t len;

        StrIndex(int index, uint32_t offset, uint32_t len):
            index(index),
            offset(offset),
            len(len){
        }

        bool operator <(const StrIndex& other) const {
            return offset < other.offset;
        }
    };
    using StrIndexArr = std::vector<StrIndex>;

    struct StrData {
        int index;
        std::vector<std::string> strings;

        StrData(int index, std::vector<std::string> &&strings):
            index(index),
            strings(std::move(strings)){
        }

        bool operator <(const StrData& other) const {
            return index < other.index;
        }
    };
    using StrDataArr = std::vector<StrData>;

    static LangStorage emptyLangStorage = {{"", Lang()}};
    static LangStorage langStorage;


    std::string GotText::_(
            const std::string& msgid
            ) const
    {
        GOTTEXT_READ_LOCK
        auto i = getLang().dictOne.find(msgid);
        if(i == getLang().dictOne.end())
            return msgid;
        return (*i).second;
    }

    std::string GotText::_n(
            const std::string& msgid,
            const std::string& msgid_plural,
            int n
            ) const
    {
        GOTTEXT_READ_LOCK
        auto i = getLang().dictNum.find(msgid);
        if(i == getLang().dictNum.end())
            return Plural::origFunc(n) ? msgid_plural : msgid;
        return (*i).second[getLang().pluralInfo.func(n)];
    }

    std::string GotText::_p(
            const std::string& msgid_ctxt,
            const std::string& msgid
            ) const
    {
        GOTTEXT_READ_LOCK
        auto ic = getLang().dictCtxOne.find(msgid_ctxt);
        if(ic == getLang().dictCtxOne.end())
            return msgid;
        auto i = (*ic).second.find(msgid);
        if(i == (*ic).second.end())
            return msgid;
        return (*i).second;
    }

    std::string GotText::_np(
            const std::string& msgid_ctxt,
            const std::string& msgid,
            const std::string& msgid_plural,
            int n
            ) const
    {
        GOTTEXT_READ_LOCK
        auto ic = getLang().dictCtxNum.find(msgid_ctxt);
        if(ic == getLang().dictCtxNum.end())
            return Plural::origFunc(n) ? msgid_plural : msgid;
        auto i = (*ic).second.find(msgid);
        if(i == (*ic).second.end())
            return Plural::origFunc(n) ? msgid_plural : msgid;
        return (*i).second[getLang().pluralInfo.func(n)];
    }

    time_t GotText::getTimestamp() const
    {
        return std::chrono::seconds(std::time(nullptr)).count();
    }

    GotText::GotText():
        lang(emptyLangStorage.begin()) // this allows not to check for iterator validity every request
    {
    }

    GotText::GotText(const GotText &that)
    {
        GOTTEXT_WRITE_LOCK
        lang = that.lang;
    }

    void GotText::load(const std::string& filename, bool forceReload)
    {
        if(forceReload)
        {
            GOTTEXT_WRITE_LOCK
            loadFile(filename);
        }
        else
        {
            GOTTEXT_READ_UPGRADE_LOCK
            auto i = langStorage.find(filename);
            if(i == langStorage.end() || (*i).second.isDummy())
            {
                GOTTEXT_UPGRADE_LOCK_TO_WRITE
#ifndef GOTTEXT_NO_THREADSAFE
                // check again to mitigate a race condition from the previous comparison
                LangStorage::iterator i = langStorage.find(filename);
                if(i == langStorage.end() || (*i).second.isDummy())
                {
                    loadFile(filename);
                    return;
                }
#else
                loadFile(filename);
                return;
#endif
            }
            lang = i;
        }
    }

    void GotText::load(const std::string &filename, std::istream &stream)
    {
        GOTTEXT_WRITE_LOCK
        loadStream(stream, filename);
    }

    void GotText::unload(const std::string &filename)
    {
        GOTTEXT_WRITE_LOCK
        auto i = langStorage.find(filename);
        if(i == langStorage.end())
            return;
        (*i).second.swap(Lang());
    }

    bool GotText::isLoaded(const std::string& filename)
    {
        auto i = langStorage.find(filename);
        return i != langStorage.end() && !(*i).second.isDummy();
    }

    const LangStorage &GotText::getStorage()
    {
        return langStorage;
    }

    static uint32_t readInt(std::istream& f)
    {
        uint32_t result = 0;
        for(size_t a=0; a<sizeof(result); a++)
            result += f.get() << (a*8);
        return result;
    }

    static const StrIndexArr readStrTable(std::istream& f, uint32_t offset, uint32_t nStrings)
    {
        StrIndexArr indexArr;
        f.seekg(offset);
        for(uint32_t a=0; a<nStrings; a++)
            indexArr.emplace_back(a, readInt(f), readInt(f));
        return indexArr;
    }

    static std::vector<std::string> readStrings(const StrIndex& strIndex, std::istream& f)
    {
        f.seekg(strIndex.offset);
        std::vector<std::string> strings;
        uint32_t p = strIndex.offset;
        uint32_t end = p + strIndex.len + 1;
        do
        {
            std::string s;
            std::getline(f, s, '\0');
            p = p + s.size() + 1;
            strings.emplace_back(std::move(s));
        }while(p != end);
        return strings;
    }

    static const StrDataArr readStrings(StrIndexArr& indexArr, std::istream& f)
    {
        std::sort(indexArr.begin(), indexArr.end());
        StrDataArr dataArr;
        for(StrIndex& strIndex : indexArr)
            dataArr.emplace_back(strIndex.index, readStrings(strIndex, f));
        std::sort(dataArr.begin(), dataArr.end());
        return dataArr;
    }

    void GotText::setLang(const std::string& filename, Lang &&other)
    {
        lang = langStorage.find(filename);
        if(lang == langStorage.end())
        {
            lang = langStorage.emplace(
                filename,
                std::move(other)
            ).first;
        }
        else
        {
            (*lang).second.swap(std::move(other));
        }
        (*lang).second.time = getTimestamp();
    }

    void GotText::loadFile(const std::string& filename)
    {
        errno = 0;
        setLang(filename, loadFromFile(filename));
    }

    void GotText::loadStream(std::istream &s, const std::string& filename)
    {
        errno = 0;
        setLang(filename, loadFromStream(s, filename));
    }

    Lang GotText::loadFromFile(const std::string& filename)
    {
        std::ifstream f(filename, std::ifstream::binary);
        if(!f)
            throw Exception(Exception::ReadError);
        f.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
        try{
            return loadFromStream(f, filename);
        }catch(std::ios_base::failure &e){
            throw Exception(Exception::ReadError, f);
        }
    }

    Lang GotText::loadFromStream(std::istream &f, const std::string& /*filename*/)
    {
        uint32_t magicNum = readInt(f);
        if(magicNum != MO_MAGIC_NUMBER)
            throw Exception(Exception::UnknownMagicNumber, f, nullptr, magicNum);
        uint32_t version = readInt(f);
        if(version > MO_MAX_SUPPORTED_VERSION)
            throw Exception(Exception::UnsupportedVersion, f, nullptr, version);
        uint32_t nStrings = readInt(f);
        if(!nStrings)
            throw Exception(Exception::NoTranslations, f);

        uint32_t offsetOrig = readInt(f);
        uint32_t offsetTr = readInt(f);
        StrIndexArr indexArrOrig;
        StrIndexArr indexArrTr;

        if(offsetTr > offsetOrig)
        {
            indexArrOrig = readStrTable(f, offsetOrig, nStrings);
            indexArrTr = readStrTable(f, offsetTr, nStrings);
        }
        else
        {
            indexArrTr = readStrTable(f, offsetTr, nStrings);
            indexArrOrig = readStrTable(f, offsetOrig, nStrings);
        }

        StrDataArr dataArrOrig = readStrings(indexArrOrig, f);
        StrDataArr dataArrTr = readStrings(indexArrTr, f);

        Lang thisLang;
        thisLang.dictOne.reserve(nStrings);
        thisLang.dictNum.reserve(nStrings);

        auto trp = dataArrTr.begin();
        for(const StrData& orig : dataArrOrig)
        {
            StrData& tr = *trp;
            if(orig.strings[0].empty())
            {
#ifdef GOTTEXT_BOOST_REGEX
                using namespace boost;
#else
                using namespace std;
#endif
                smatch m;
                regex rx("\\nLanguage\\:\\s(\\w+)");
                if(!regex_search(tr.strings[0], m, rx))
                    throw Exception(Exception::NoLanguageHeader, f, std::move(tr.strings[0]));
                thisLang.pluralInfo = Plural::getInfo(m[1]);
                if(thisLang.pluralInfo.isValid())
                    thisLang.locale = m[1];
                break;
            }
            ++trp;
        }
        if(!thisLang.pluralInfo.isValid())
            throw Exception(Exception::NoHeaders, f);

        trp = dataArrTr.begin();
        for(StrData& orig : dataArrOrig)
        {
            StrData& tr = *trp;
            if(orig.strings.size() > 2)
                throw Exception(Exception::TooManySourceForms, f, std::move(tr.strings[0]), tr.strings.size());
            if(orig.strings.size() == 1)
            {
                if(tr.strings.size() != 1)
                    throw Exception(Exception::TooManyNonPluralTranslations, f, std::move(tr.strings[0]), tr.strings.size());

                std::string& s = orig.strings[0];
                size_t p = s.find('\4');

                if(p == std::string::npos)
                {
                    thisLang.dictOne.emplace(
                        std::move(orig.strings[0]),
                        std::move(tr.strings[0])
                    );
                }
                else
                {
                    std::string ctx = s.substr(0, p);
                    s.erase(0, p+1);
                    thisLang.dictCtxOne[std::move(ctx)][std::move(s)] = std::move(tr.strings[0]);
                }
            }
            else
            {
                if(tr.strings.size() != thisLang.pluralInfo.count)
                    throw Exception(Exception::InvalidPluralFormsCount, f, std::move(tr.strings[0]), tr.strings.size());

                std::string& s = orig.strings[0];
                size_t p = s.find('\4');

                if(p == std::string::npos)
                {
                    thisLang.dictNum.emplace(
                        std::move(orig.strings[0]),
                        std::move(tr.strings)
                    );
                }
                else
                {
                    std::string ctx = s.substr(0, p);
                    s.erase(0, p+1);
                    thisLang.dictCtxNum[std::move(ctx)][std::move(s)] = std::move(tr.strings);
                }
            }
            ++trp;
        }

        return thisLang;
    }

    void Lang::swap(Lang &&other)
    {
        std::swap(time, other.time);
        std::swap(locale, other.locale);
        std::swap(pluralInfo, other.pluralInfo);
        std::swap(dictOne, other.dictOne);
        std::swap(dictNum, other.dictNum);
        std::swap(dictCtxOne, other.dictCtxOne);
        std::swap(dictCtxNum, other.dictCtxNum);
    }
}
