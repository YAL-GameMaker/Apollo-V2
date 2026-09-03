// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
#pragma once

#ifdef APOLLO_WINAPI

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <string.h>
#endif

#include "apollo_shared.h"
