//
// Created by lkq on 2020/9/18.
//

#ifndef BKBASE_BK_PLATFORM_H
#define BKBASE_BK_PLATFORM_H

#ifdef _MSC_VER
#define COMPILER_MSVC
#elif defined(__GCC__)
#define COMPILER_GCC
#endif

#ifdef _WIN32
#define OS_WINDOWS
#elif defined(__LINUX__)
#define OS_LINUX
#endif

#endif //BKBASE_BK_PLATFORM_H
