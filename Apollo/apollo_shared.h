// apollo_shared.h
#pragma once

#if defined(WIN32)
#define dllx extern "C" __declspec(dllexport)
#elif defined(GNUC)
#define dllx extern "C" __attribute__ ((visibility("default"))) 
#else
#define dllx extern "C"
#endif

extern "C" {
	#include "./../Lua/lua.h"
	#include "./../Lua/lualib.h"
	#include "./../Lua/lauxlib.h"
}

#pragma region typedefs
typedef char int8;
typedef int int32;
typedef long long int64;
typedef unsigned int uint32;
typedef double real;
typedef int32 gml_script_id;
extern void* lua_outbuf;
#pragma endregion

///~
enum lua_status_t {
	lua_status_amiss = 0,
	lua_status_no_state,
	lua_status_no_func,
	lua_status_done,
	lua_status_error,
	lua_status_call,
	lua_status_yield,
	lua_status_callmethod,
};

#define trace(...) { printf(__VA_ARGS__); printf("\n"); fflush(stdout); }