#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define PLATFORM_WINDOWS
#endif

#ifndef PLATFORM_WINDOWS
static_assert(false, __FILE__ " may only be included on a Windows Platform");
#endif

#ifndef NOMINMAX 
#define NOMINMAX 
#endif  

#include <Windows.h>