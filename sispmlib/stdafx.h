#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tchar.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif
    #include <hidsdi.h>
    #include <setupapi.h>
#ifdef __cplusplus
}
#endif

#ifdef _UNICODE
#define RUNDLL32EXPORT __pragma(comment(linker, "/EXPORT:" __FUNCTION__ "W=" __FUNCDNAME__))
#else
#define RUNDLL32EXPORT __pragma(comment(linker, "/EXPORT:" __FUNCTION__ "A=" __FUNCDNAME__))
#endif
