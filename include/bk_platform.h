//
// Created by lkq on 2020/9/18.
//

#ifndef BKBASE_BK_PLATFORM_H
#define BKBASE_BK_PLATFORM_H

#include "bk_error.h"
#include "bk_types.h"

#ifdef _MSC_VER
#	define COMPILER_MSVC
#elif defined(__GCC__)
#	define COMPILER_GCC
#endif

#ifdef _WIN32
#	define OS_WINDOWS
#elif defined(__LINUX__)
#	define OS_LINUX
#endif

#ifdef OS_WINDOWS
#	ifdef _WIN64
#	define ARCH_X64 
#	else
#	define ARCH_X86 
#	endif
#endif

#ifdef OS_LINUX
#	if defined(__x86_64__) 
#	define ARCH_X64 
#	elif defined(__i386__)
#	define ARCH_X86 
#	endif
#endif

#endif //BKBASE_BK_PLATFORM_H
