/*************************************************************************}
{ plural.cpp - international plural forms information                     }
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

#include "plural.h"

//  data source: https://localization-guide.readthedocs.io/en/latest/l10n/pluralforms.html
// last updated: Jun 20, 2019
//    changelog: https://github.com/translate/l10n-guide/commits/master/docs/l10n/pluralforms.rst

namespace GotText {
namespace Plural {

#define INFO_DECL(suffix, cnt, expr) \
    static int func_##suffix(int n){ \
        return (expr); \
    } \
    static const Info& info_##suffix(){ \
        static Info info(func_##suffix, cnt); \
        return info; \
    }

    INFO_DECL(err,      0,   0);
    INFO_DECL(1,        1,   0);
    INFO_DECL(2_ne1,    2,   n!=1 ? 1 : 0);
    INFO_DECL(2_gt1,    2,   n>1 ? 1 : 0);
    INFO_DECL(slavic,   3,   (n%10==1 && n%100!=11) ? 0 : (n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20)) ? 1 : 2);
    INFO_DECL(ar,       6,   n==0 ? 0 : n==1 ? 1 : n==2 ? 2 : (n%100>=3 && n%100<=10) ? 3 : n%100>=11 ? 4 : 5);
    INFO_DECL(cs,       3,   n==1 ? 0 : (n>=2 && n<=4) ? 1 : 2);
    INFO_DECL(csb_pl,   3,   n==1 ? 0 : (n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20)) ? 1 : 2);
    INFO_DECL(cy,       4,   n==1 ? 0 : n==2 ? 1 : (n!=8 && n!=11) ? 2 : 3);
    INFO_DECL(ga,       5,   n==1 ? 0 : n==2 ? 1 : (n>2 && n<7) ? 2 : (n>6 && n<11) ? 3 : 4);
    INFO_DECL(gd,       4,   (n==1 || n==11) ? 0 : (n==2 || n==12) ? 1 : (n>2 && n<20) ? 2 : 3);
    INFO_DECL(is,       2,   (n%10!=1 || n%100==11) ? 1 : 0);
    INFO_DECL(jv,       2,   n!=0 ? 1 : 0);
    INFO_DECL(kw,       4,   n==1 ? 0 : n==2 ? 1 : n==3 ? 2 : 3);
    INFO_DECL(lt,       3,   (n%10==1 && n%100!=11) ? 0 : (n%10>=2 && (n%100<10 || n%100>=20)) ? 1 : 2);
    INFO_DECL(lv,       3,   (n%10==1 && n%100!=11) ? 0 : n!=0 ? 1 : 2);
    INFO_DECL(mk,       2,   n==1 || n%10==1 ? 0 : 1);
    INFO_DECL(mnk,      3,   n==0 ? 0 : n==1 ? 1 : 2);
    INFO_DECL(mt,       4,   n==1 ? 0 : (n==0 || (n%100>1 && n%100<11)) ? 1 : (n%100>10 && n%100<20) ? 2 : 3);
    INFO_DECL(ro,       3,   n==1 ? 0 : (n==0 || (n%100>0 && n%100<20)) ? 1 : 2);
    INFO_DECL(sk,       3,   n==1 ? 0 : (n>=2 && n<=4) ? 1 : 2);
    INFO_DECL(sl,       4,   n%100==1 ? 0 : n%100==2 ? 1 : (n%100==3 || n%100==4) ? 2 : 3);

    const Info& getInfo(const std::string &locale)
    {
        if(locale == "es_AR") return info_2_ne1();
        if(locale == "pt_BR") return info_2_gt1();

        const std::string loc = locale.substr(0, locale.find('_'));

        if(loc == "ach") return info_2_gt1();
        if(loc == "af")  return info_2_ne1();
        if(loc == "ak")  return info_2_gt1();
        if(loc == "am")  return info_2_gt1();
        if(loc == "an")  return info_2_ne1();
        if(loc == "anp") return info_2_ne1();
        if(loc == "ar")  return info_ar();
        if(loc == "arn") return info_2_gt1();
        if(loc == "as")  return info_2_ne1();
        if(loc == "ast") return info_2_ne1();
        if(loc == "ay")  return info_1();
        if(loc == "az")  return info_2_ne1();
        if(loc == "be")  return info_slavic();
        if(loc == "bg")  return info_2_ne1();
        if(loc == "bn")  return info_2_ne1();
        if(loc == "bo")  return info_1();
        if(loc == "br")  return info_2_gt1();
        if(loc == "brx") return info_2_ne1();
        if(loc == "bs")  return info_slavic();
        if(loc == "ca")  return info_2_ne1();
        if(loc == "cgg") return info_1();
        if(loc == "cs")  return info_cs();
        if(loc == "csb") return info_csb_pl();
        if(loc == "cy")  return info_cy();
        if(loc == "da")  return info_2_ne1();
        if(loc == "de")  return info_2_ne1();
        if(loc == "doi") return info_2_ne1();
        if(loc == "dz")  return info_1();
        if(loc == "el")  return info_2_ne1();
        if(loc == "en")  return info_2_ne1();
        if(loc == "eo")  return info_2_ne1();
        if(loc == "es")  return info_2_ne1();
        if(loc == "et")  return info_2_ne1();
        if(loc == "eu")  return info_2_ne1();
        if(loc == "fa")  return info_2_gt1();
        if(loc == "ff")  return info_2_ne1();
        if(loc == "fi")  return info_2_ne1();
        if(loc == "fil") return info_2_gt1();
        if(loc == "fo")  return info_2_ne1();
        if(loc == "fr")  return info_2_gt1();
        if(loc == "fur") return info_2_ne1();
        if(loc == "fy")  return info_2_ne1();
        if(loc == "ga")  return info_ga();
        if(loc == "gd")  return info_gd();
        if(loc == "gl")  return info_2_ne1();
        if(loc == "gu")  return info_2_ne1();
        if(loc == "gun") return info_2_gt1();
        if(loc == "ha")  return info_2_ne1();
        if(loc == "he")  return info_2_ne1();
        if(loc == "hi")  return info_2_ne1();
        if(loc == "hne") return info_2_ne1();
        if(loc == "hr")  return info_slavic();
        if(loc == "hu")  return info_2_ne1();
        if(loc == "hy")  return info_2_ne1();
        if(loc == "ia")  return info_2_ne1();
        if(loc == "id")  return info_1();
        if(loc == "is")  return info_is();
        if(loc == "it")  return info_2_ne1();
        if(loc == "ja")  return info_1();
        if(loc == "jbo") return info_1();
        if(loc == "jv")  return info_jv();
        if(loc == "ka")  return info_1();
        if(loc == "kk")  return info_2_ne1();
        if(loc == "kl")  return info_2_ne1();
        if(loc == "km")  return info_1();
        if(loc == "kn")  return info_2_ne1();
        if(loc == "ko")  return info_1();
        if(loc == "ku")  return info_2_ne1();
        if(loc == "kw")  return info_kw();
        if(loc == "ky")  return info_2_ne1();
        if(loc == "lb")  return info_2_ne1();
        if(loc == "ln")  return info_2_gt1();
        if(loc == "lo")  return info_1();
        if(loc == "lt")  return info_lt();
        if(loc == "lv")  return info_lv();
        if(loc == "mai") return info_2_ne1();
        if(loc == "me")  return info_slavic();
        if(loc == "mfe") return info_2_gt1();
        if(loc == "mg")  return info_2_gt1();
        if(loc == "mi")  return info_2_gt1();
        if(loc == "mk")  return info_mk();
        if(loc == "ml")  return info_2_ne1();
        if(loc == "mn")  return info_2_ne1();
        if(loc == "mni") return info_2_ne1();
        if(loc == "mnk") return info_mnk();
        if(loc == "mr")  return info_2_ne1();
        if(loc == "ms")  return info_1();
        if(loc == "mt")  return info_mt();
        if(loc == "my")  return info_1();
        if(loc == "nah") return info_2_ne1();
        if(loc == "nap") return info_2_ne1();
        if(loc == "nb")  return info_2_ne1();
        if(loc == "ne")  return info_2_ne1();
        if(loc == "nl")  return info_2_ne1();
        if(loc == "nn")  return info_2_ne1();
        if(loc == "no")  return info_2_ne1();
        if(loc == "nso") return info_2_ne1();
        if(loc == "oc")  return info_2_gt1();
        if(loc == "or")  return info_2_ne1();
        if(loc == "pa")  return info_2_ne1();
        if(loc == "pap") return info_2_ne1();
        if(loc == "pl")  return info_csb_pl();
        if(loc == "pms") return info_2_ne1();
        if(loc == "ps")  return info_2_ne1();
        if(loc == "pt")  return info_2_ne1();
        if(loc == "rm")  return info_2_ne1();
        if(loc == "ro")  return info_ro();
        if(loc == "ru")  return info_slavic();
        if(loc == "rw")  return info_2_ne1();
        if(loc == "sah") return info_1();
        if(loc == "sat") return info_2_ne1();
        if(loc == "sco") return info_2_ne1();
        if(loc == "sd")  return info_2_ne1();
        if(loc == "se")  return info_2_ne1();
        if(loc == "si")  return info_2_ne1();
        if(loc == "sk")  return info_sk();
        if(loc == "sl")  return info_sl();
        if(loc == "so")  return info_2_ne1();
        if(loc == "son") return info_2_ne1();
        if(loc == "sq")  return info_2_ne1();
        if(loc == "sr")  return info_slavic();
        if(loc == "su")  return info_1();
        if(loc == "sv")  return info_2_ne1();
        if(loc == "sw")  return info_2_ne1();
        if(loc == "ta")  return info_2_ne1();
        if(loc == "te")  return info_2_ne1();
        if(loc == "tg")  return info_2_gt1();
        if(loc == "th")  return info_1();
        if(loc == "ti")  return info_2_gt1();
        if(loc == "tk")  return info_2_ne1();
        if(loc == "tr")  return info_2_gt1();
        if(loc == "tt")  return info_1();
        if(loc == "ug")  return info_1();
        if(loc == "uk")  return info_slavic();
        if(loc == "ur")  return info_2_ne1();
        if(loc == "uz")  return info_2_gt1();
        if(loc == "vi")  return info_1();
        if(loc == "wa")  return info_2_gt1();
        if(loc == "wo")  return info_1();
        if(loc == "yo")  return info_2_ne1();
        if(loc == "zh")  return info_1();

        return info_err();
    }

    Info::Info():
        func(func_err){
    }

    Info::Info(Func func, size_t count):
        func(func),
        count(count){
    }

}}
