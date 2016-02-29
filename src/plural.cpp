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

// data from http://localization-guide.readthedocs.org/en/latest/l10n/pluralforms.html (Jan 24, 2016)

namespace GotText {
namespace Plural {

    int func_err(int /*n*/){return 0;}

    Info::Info():
        func(&func_err),
        count(0){
    }

    Info::Info(Func func, int count):
        func(func),
        count(count){
    }

    static int func_1(int n){(void)n; return 0;}
    static int func_2_ne1(int n){return n != 1 ? 1 : 0;}
    static int func_2_gt1(int n){return n != 1 ? 1 : 0;}
    static int func_slavic(int n){
        return n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2;
    }
    static int func_ar(int n){
        return n==0 ? 0 : n==1 ? 1 : n==2 ? 2 : n%100>=3 && n%100<=10 ? 3 : n%100>=11 ? 4 : 5;
    }
    static int func_cs(int n){
        return (n==1) ? 0 : (n>=2 && n<=4) ? 1 : 2;
    }
    static int func_csb(int n){
        return (n==1) ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2;
    }
    static int func_cy(int n){
        return (n==1) ? 0 : (n==2) ? 1 : (n != 8 && n != 11) ? 2 : 3;
    }
    static int func_ga(int n){
        return n==1 ? 0 : n==2 ? 1 : (n>2 && n<7) ? 2 :(n>6 && n<11) ? 3 : 4;
    }
    static int func_gd(int n){
        return (n==1 || n==11) ? 0 : (n==2 || n==12) ? 1 : (n > 2 && n < 20) ? 2 : 3;
    }
    static int func_is(int n){
        return n%10!=1 || n%100==11 ? 1 : 0;
    }
    static int func_jv(int n){
        return n != 0 ? 1 : 0;
    }
    static int func_kw(int n){
        return (n==1) ? 0 : (n==2) ? 1 : (n == 3) ? 2 : 3;
    }
    static int func_lt(int n){
        return n%10==1 && n%100!=11 ? 0 : n%10>=2 && (n%100<10 || n%100>=20) ? 1 : 2;
    }
    static int func_lv(int n){
        return n%10==1 && n%100!=11 ? 0 : n != 0 ? 1 : 2;
    }
    static int func_mk(int n){
        return n==1 || n%10==1 ? 0 : 1;
    }
    static int func_mnk(int n){
        return n==0 ? 0 : n==1 ? 1 : 2;
    }
    static int func_mt(int n){
        return n==1 ? 0 : n==0 || ( n%100>1 && n%100<11) ? 1 : (n%100>10 && n%100<20 ) ? 2 : 3;
    }
    static int func_ro(int n){
        return n==1 ? 0 : (n==0 || (n%100 > 0 && n%100 < 20)) ? 1 : 2;
    }
    static int func_sk(int n){
        return (n==1) ? 0 : (n>=2 && n<=4) ? 1 : 2;
    }
    static int func_sl(int n){
        return n%100==1 ? 1 : n%100==2 ? 2 : n%100==3 || n%100==4 ? 3 : 0;
    }

    static const Info info_err;
    static const Info info_1(&func_1, 1);
    static const Info info_2_ne1(&func_2_ne1, 2);
    static const Info info_2_gt1(&func_2_gt1, 2);
    static const Info info_slavic(&func_slavic, 3);
    static const Info info_ar(&func_ar, 6);
    static const Info info_cs(&func_cs, 3);
    static const Info info_csb(&func_csb, 3);
    static const Info info_cy(&func_cy, 4);
    static const Info info_ga(&func_ga, 5);
    static const Info info_gd(&func_gd, 4);
    static const Info info_is(&func_is, 2);
    static const Info info_jv(&func_jv, 2);
    static const Info info_kw(&func_kw, 4);
    static const Info info_lt(&func_lt, 3);
    static const Info info_lv(&func_lv, 3);
    static const Info info_mk(&func_mk, 2);
    static const Info info_mnk(&func_mnk, 3);
    static const Info info_mt(&func_mt, 4);
    static const Info info_ro(&func_ro, 3);
    static const Info info_sk(&func_sk, 3);
    static const Info info_sl(&func_sl, 4);

    const Info& getInfo(const std::string &locale)
    {
        if(locale == "es_AR")return info_2_ne1;
        if(locale == "pt_BR")return info_2_gt1;

        const std::string loc = locale.substr(0, locale.find('_'));

        if(loc == "ach")return info_2_gt1;
        if(loc == "af")return info_2_ne1;
        if(loc == "ak")return info_2_gt1;
        if(loc == "am")return info_2_gt1;
        if(loc == "an")return info_2_ne1;
        if(loc == "anp")return info_2_ne1;
        if(loc == "ar")return info_ar;
        if(loc == "arn")return info_2_gt1;
        if(loc == "as")return info_2_ne1;
        if(loc == "ast")return info_2_ne1;
        if(loc == "ay")return info_1;
        if(loc == "az")return info_2_ne1;
        if(loc == "be")return info_slavic;
        if(loc == "bg")return info_2_ne1;
        if(loc == "bn")return info_2_ne1;
        if(loc == "bo")return info_1;
        if(loc == "br")return info_2_gt1;
        if(loc == "brx")return info_2_ne1;
        if(loc == "bs")return info_slavic;
        if(loc == "ca")return info_2_ne1;
        if(loc == "cgg")return info_1;
        if(loc == "cs")return info_cs;
        if(loc == "csb")return info_csb;
        if(loc == "cy")return info_cy;
        if(loc == "da")return info_2_ne1;
        if(loc == "de")return info_2_ne1;
        if(loc == "doi")return info_2_ne1;
        if(loc == "dz")return info_1;
        if(loc == "el")return info_2_ne1;
        if(loc == "en")return info_2_ne1;
        if(loc == "eo")return info_2_ne1;
        if(loc == "es")return info_2_ne1;
        if(loc == "et")return info_2_ne1;
        if(loc == "eu")return info_2_ne1;
        if(loc == "fa")return info_1;
        if(loc == "ff")return info_2_ne1;
        if(loc == "fi")return info_2_ne1;
        if(loc == "fil")return info_2_gt1;
        if(loc == "fo")return info_2_ne1;
        if(loc == "fr")return info_2_gt1;
        if(loc == "fur")return info_2_ne1;
        if(loc == "fy")return info_2_ne1;
        if(loc == "ga")return info_2_ne1;
        if(loc == "gd")return info_gd;
        if(loc == "gl")return info_2_ne1;
        if(loc == "gu")return info_2_ne1;
        if(loc == "gun")return info_2_gt1;
        if(loc == "ha")return info_2_ne1;
        if(loc == "he")return info_2_ne1;
        if(loc == "hi")return info_2_ne1;
        if(loc == "hne")return info_2_ne1;
        if(loc == "hr")return info_slavic;
        if(loc == "hu")return info_2_ne1;
        if(loc == "hy")return info_2_ne1;
        if(loc == "ia")return info_2_ne1;
        if(loc == "id")return info_1;
        if(loc == "is")return info_1;
        if(loc == "it")return info_2_ne1;
        if(loc == "ja")return info_1;
        if(loc == "jbo")return info_1;
        if(loc == "jv")return info_jv;
        if(loc == "ka")return info_1;
        if(loc == "kk")return info_1;
        if(loc == "kl")return info_2_ne1;
        if(loc == "km")return info_1;
        if(loc == "kn")return info_2_ne1;
        if(loc == "ko")return info_1;
        if(loc == "ku")return info_2_ne1;
        if(loc == "kw")return info_kw;
        if(loc == "ky")return info_1;
        if(loc == "lb")return info_2_ne1;
        if(loc == "ln")return info_2_gt1;
        if(loc == "lo")return info_1;
        if(loc == "lt")return info_lt;
        if(loc == "lv")return info_lv;
        if(loc == "mai")return info_2_ne1;
        if(loc == "mfe")return info_2_gt1;
        if(loc == "mg")return info_2_gt1;
        if(loc == "mi")return info_2_gt1;
        if(loc == "mk")return info_mk;
        if(loc == "ml")return info_2_ne1;
        if(loc == "mn")return info_2_ne1;
        if(loc == "mni")return info_2_ne1;
        if(loc == "mnk")return info_mnk;
        if(loc == "mr")return info_2_ne1;
        if(loc == "ms")return info_1;
        if(loc == "mt")return info_1;
        if(loc == "my")return info_1;
        if(loc == "nah")return info_2_ne1;
        if(loc == "nap")return info_2_ne1;
        if(loc == "nb")return info_2_ne1;
        if(loc == "ne")return info_2_ne1;
        if(loc == "nl")return info_2_ne1;
        if(loc == "nn")return info_2_ne1;
        if(loc == "no")return info_2_ne1;
        if(loc == "nso")return info_2_ne1;
        if(loc == "oc")return info_2_gt1;
        if(loc == "or")return info_2_ne1;
        if(loc == "pa")return info_2_ne1;
        if(loc == "pap")return info_2_ne1;
        if(loc == "pap")return info_slavic;
        if(loc == "pms")return info_2_ne1;
        if(loc == "ps")return info_2_ne1;
        if(loc == "pt")return info_2_ne1;
        if(loc == "rm")return info_2_ne1;
        if(loc == "ro")return info_ro;
        if(loc == "ru")return info_slavic;
        if(loc == "rw")return info_2_ne1;
        if(loc == "sah")return info_1;
        if(loc == "sat")return info_2_ne1;
        if(loc == "sco")return info_2_ne1;
        if(loc == "sd")return info_2_ne1;
        if(loc == "se")return info_2_ne1;
        if(loc == "si")return info_2_ne1;
        if(loc == "sk")return info_sk;
        if(loc == "sl")return info_sl;
        if(loc == "so")return info_2_ne1;
        if(loc == "son")return info_2_ne1;
        if(loc == "sq")return info_2_ne1;
        if(loc == "sr")return info_slavic;
        if(loc == "su")return info_1;
        if(loc == "sv")return info_2_ne1;
        if(loc == "sw")return info_2_ne1;
        if(loc == "ta")return info_2_ne1;
        if(loc == "te")return info_2_ne1;
        if(loc == "tg")return info_2_gt1;
        if(loc == "th")return info_1;
        if(loc == "ti")return info_2_gt1;
        if(loc == "tk")return info_2_ne1;
        if(loc == "tr")return info_2_gt1;
        if(loc == "tt")return info_1;
        if(loc == "ug")return info_1;
        if(loc == "uk")return info_slavic;
        if(loc == "ur")return info_2_ne1;
        if(loc == "uz")return info_2_gt1;
        if(loc == "vi")return info_1;
        if(loc == "wa")return info_2_gt1;
        if(loc == "wo")return info_1;
        if(loc == "yo")return info_2_ne1;
        if(loc == "zh")return info_1;

        return info_err;
    }
}}
