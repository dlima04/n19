/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef N19_MINWIN_HPP
#define N19_MINWIN_HPP
#ifdef N19_WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOSERVICE
#define NOSERVICE
#endif

#ifndef NOMCX
#define NOMCX
#endif

#ifndef NOIME
#define NOIME
#endif

#ifndef NOSOUND
#define NOSOUND
#endif

#ifndef NOKANJI
#define NOKANJI
#endif

#ifndef NOPROXYSTUB
#define NOPROXYSTUB
#endif

#ifndef NOIMAGE
#define NOIMAGE
#endif

#ifndef NOGDI
#define NOGDI
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef NOTAPE
#define NOTAPE
#endif

#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif

#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#endif

#ifndef WINVER
#define WINVER 0x0A00
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>

#endif //N19_WIN32
#endif //N19_MINWIN_HPP
