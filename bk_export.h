//
// Created by lkq on 2020/9/18.
//

#ifndef BKBASE_BK_EXPORT_H
#define BKBASE_BK_EXPORT_H

#include "bk_platform.h"

#ifdef BK_EXPORT_IMPLEMENTS
#   ifdef COMPILER_MSVC
#   define BK_EXPORT __declspec(dllexport)
#   else
#   define BK_EXPORT __attribute__((dllexport))
#   endif
#else
#   ifdef COMPILER_MSVC
#   define BK_EXPORT __declspec(dllimport)
#   else
#   define BK_EXPORT __attribute__((dllimport))
#   endif
#endif

#endif //BKBASE_BK_EXPORT_H
