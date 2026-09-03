// apollo_core.cpp:
/// @author YellowAfterlife
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include <vector>
#include <queue>
#include <stack>
#include <map>
#ifdef APOLLO_WINAPI
#include <codecvt>
#else
#include <unistd.h>
#endif
#include "apollo_buffer.h"
#include "apollo_method.h"
#include "apollo_ref.h"
using namespace std;

void* lua_outbuf;

#ifdef APOLLO_WINAPI // codepage conversion (Windows)
wstring_convert<codecvt_utf8_utf16<wchar_t>> ccvt;
string ccvt_utf8;
char* ret_string(wstring& ws) {
	ccvt_utf8 = ccvt.to_bytes(ws);
	return (char*)ccvt_utf8.c_str();
}
char* ret_string(wchar_t* ws) {
	ccvt_utf8 = ccvt.to_bytes(ws);
	return (char*)ccvt_utf8.c_str();
}
#endif
char* ret_string(const char* s) {
	static char* buf = nullptr;
	static size_t buf_size = 0;
	size_t n = strlen(s) + 1;
	if (buf_size < n) {
		auto tmp = realloc(buf, n);
		if (tmp) {
			buf = (char*)tmp;
			buf_size = n;
		}
	}
	if (n > buf_size) {
		strncpy((char*)buf, s, buf_size - 1);
		buf[buf_size - 1] = 0;
	} else strcpy((char*)buf, s);
	return (char*)buf;
}

#pragma region states
struct lua_state_t {
	// The actual Lua state (or thread)
	lua_State* state;

	// If this is a thread, indicates the parent state. Otherwise is null
	lua_State* parent;

	// This is only for errors thrown via lua_show_error - rethrows into state upon resuming
	char* error_text = nullptr;

	// A specific set of conditions urges us to have a stack of sub-states:
	// - To be able to suspend Lua calls and return to GML, it must be a coroutine
	// - If an error occurs in a coroutine, it is pronounced dead and can no longer be used
	// - Calls can be nested (GML->Lua->GML->Other function in same Lua state)
	// So we'll maintain a stack of temporary states and create/clean them up as we might
	stack<lua_State*> substates;

	lua_state_t(lua_State* q, lua_State* p) : state(q), parent(p) {}
};
vector<lua_state_t*> lua_state_vec;
queue<size_t> lua_state_reusable;
stack<lua_state_t*> lua_state_stack;
lua_State* lua_state_find(double index) {
	size_t i = (size_t)index;
	return i < lua_state_vec.size() ? lua_state_vec[i]->state : nullptr;
}
lua_state_t* lua_state_find_t(double index) {
	size_t i = (size_t)index;
	return i < lua_state_vec.size() ? lua_state_vec[i] : nullptr;
}
///
dllx double lua_show_error(char* text) {
	if (lua_state_stack.empty()) return false;
	lua_state_t* qt = lua_state_stack.top();
	if (qt->error_text == nullptr) {
		char* s = (char*)malloc(strlen(text) + 1);
		strcpy(s, text);
		qt->error_text = s;
		return true;
	} else return false;
}
#pragma endregion

/// Destroys every single state at once
dllx double lua_reset() {
	size_t n = lua_state_vec.size();
	for (size_t i = 0; i < n; i++) {
		auto q = lua_state_vec[i];
		if (q != nullptr) {
			// child states are GC-ed, no need to dealloc them
			if (q->parent == nullptr) lua_close(q->state);
			if (q->error_text != nullptr) free(q->error_text);
			delete q;
		}
	}
	lua_state_vec.clear();
	lua_state_reusable = queue<size_t>();
	return true;
}

#pragma region Current working directory
dllx const char* lua_get_cwd() {
	#ifdef APOLLO_WINAPI
	auto dir = _wgetcwd(NULL, 0);
	if (dir != NULL) {
		char* result = ret_string(dir);
		free(dir);
		return result;
	} else return "";
	#else
	auto dir = getcwd(NULL, 0);
	if (dir != NULL) {
		char* result = ret_string(dir);
		free(dir);
		return result;
	} else return "";
	#endif
}
dllx double lua_set_cwd(char* path) {
	#ifdef APOLLO_WINAPI
	int wsize = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
	WCHAR* wpath = new WCHAR[wsize + 1];
	MultiByteToWideChar(CP_UTF8, 0, path, -1, wpath, wsize);
	auto result = (_wchdir(wpath) == 0);
	delete wpath;
	return result;
	#else
	return chdir(path) == 0;
	#endif
}
#pragma endregion

#pragma region Naming helpers
#define sprintf_exec(s, i) sprintf(s, "__Apollo_auto_%zu", i)
#define sprintf_thread(s, i) sprintf(s, "__Apollo_thread_%zu", i)
#pragma endregion

#pragma region State
size_t lua_state_create_impl(lua_state_t* qt) {
	size_t i;
	if (lua_state_reusable.empty()) {
		i = lua_state_vec.size();
		lua_state_vec.push_back(qt);
	} else {
		i = lua_state_reusable.front();
		lua_state_reusable.pop();
		lua_state_vec[i] = qt;
	}
	return i;
}
///
dllx double lua_state_create() {
	auto q = luaL_newstate();
	auto qt = new lua_state_t(q, nullptr);
	auto i = lua_state_create_impl(qt);
	luaL_openlibs(q);
	apollo_method::init(q);
	apollo_array::init(q);
	apollo_struct::init(q);
	return (double)i;
}
///
dllx double lua_state_destroy(double state_id) {
	size_t i = (size_t)state_id;
	size_t n = lua_state_vec.size();
	if (i < n) {
		auto q = lua_state_vec[i];
		if (q != nullptr) {
			if (q->parent != nullptr) {
				char name[32];
				sprintf_thread(name, i);
				lua_pushnil(q->parent);
				lua_setglobal(q->parent, name);
			} else {
				lua_close(q->state);
				// remove children:
				for (size_t k = 0; k < n; k++) {
					auto qk = lua_state_vec[k];
					if (qk != nullptr && qk->parent == q->state) {
						if (qk->error_text != nullptr) free(qk->error_text);
						delete qk;
						lua_state_vec[k] = nullptr;
					}
				}
			}
			if (q->error_text != nullptr) free(q->error_text);
			delete q;
			lua_state_vec[i] = nullptr;
			return true;
		} else return false;
	} else return false;
}
///
dllx double lua_thread_create(double state_id) {
	auto ot = lua_state_find_t(state_id);
	if (ot == nullptr) return -1;
	auto o = ot->state;
	auto q = lua_newthread(o);
	auto qt = new lua_state_t(q, o);
	auto i = lua_state_create_impl(qt);
	// put the thread from the top of stack in base to a global so that GC won't eat it:
	char name[32];
	sprintf_thread(name, i);
	lua_setglobal(o, name);
	//
	return (double)i;
}
///
dllx double lua_thread_destroy(double state_id) {
	return lua_state_destroy(state_id);
}
///
dllx double lua_state_exists(double state_id) {
	return lua_state_find_t(state_id) != nullptr;
}
// Allows the indexes of all currently destroyed states/threads to be reused for new ones
///
dllx double lua_state_reuse_indexes() {
	lua_state_reusable = queue<size_t>();
	auto n = lua_state_vec.size();
	auto r = 0;
	for (size_t i = 0; i < n; i++) {
		if (lua_state_vec[i] == nullptr) {
			lua_state_reusable.push(i);
			r += 1;
		}
	}
	return r;
}
#pragma endregion

#pragma region function
dllx double lua_add_function_raw(double state_id, char* name, double script_id) {
	auto q = lua_state_find(state_id);
	if (q == nullptr) return lua_status_no_state;
	lua_pushnumber(q, script_id);
	lua_pushcclosure(q, lua_script_closure, 1);
	lua_setglobal(q, name);
	return lua_status_done;
}
#pragma endregion

#pragma region sub-state management
lua_State* lua_state_exec_start(lua_state_t* qt) {
	lua_State* q = lua_newthread(qt->state);
	char v[32];
	sprintf_exec(v, qt->substates.size());
	lua_setglobal(qt->state, v);
	qt->substates.push(q);
	return q;
}
void lua_state_exec_end(lua_state_t* qt) {
	qt->substates.pop();
	char v[32];
	sprintf_exec(v, qt->substates.size());
	lua_pushnil(qt->state);
	lua_setglobal(qt->state, v);
}
// Returns how many layers of interop (GML->Lua->GML->...) a state is deep (debug info)
///
dllx double lua_state_get_interop_depth(double state_id) {
	lua_state_t* qt = lua_state_find_t(state_id);
	if (qt == nullptr) return 0;
	return (double)qt->substates.size();
}
#pragma endregion

#pragma region calls
void lua_state_throw_error(lua_State *q, lua_Debug *ar) {
	lua_sethook(q, lua_state_throw_error, 0, 0);
	auto qt = lua_state_stack.top();
	if (qt->error_text != nullptr) {
		luaL_where(q, 1);
		lua_pushstring(q, qt->error_text);
		lua_concat(q, 2);
		free(qt->error_text);
		qt->error_text = nullptr;
		lua_error(q); // we don't return from there
	}
}
lua_status_t lua_state_exec(lua_state_t* qt, lua_State* q, void* data, bool start) {
	lua_outbuf = data;
	buffer b(data);
	int argc = b.read<int32>();
	for (int i = 0; i < argc; i++) b.read_to(q);
	lua_yield_status = lua_status_yield;
	//
	if (qt->error_text != nullptr) {
		// here's the deal: if we want LuaJIT compatibility (5.1/5.2 equivalent),
		// we can't use lua_yieldk, so... I guess just bind a one-use hook?
		lua_sethook(q, lua_state_throw_error, LUA_MASKCOUNT, 1);
	}
	//
	argc = lua_gettop(q);
	if (start) argc -= 1; // if it's the first invocation, first argument is the function to run
	int retc = 0;
	int result = lua_resume(q, NULL, argc, &retc);
	switch (result) {
		case LUA_OK: {
			// we finished execution! let's get the returned values out of there:
			b = buffer(data);
			b.write<int32>(retc);
			for (int i = 0; i < retc; i++) b.write_from(q, i + 1);
			lua_pop(q, retc);
			// remove the call-coroutine and we're good
			lua_state_exec_end(qt);
			lua_state_stack.pop();
			return lua_status_done;
		};
		case LUA_YIELD: {
			if (lua_yield_status != lua_status_yield) {
				return lua_yield_status;
			} else {
				b = buffer(data);
				b.write<int32>(retc);
				for (int i = 0; i < retc; i++) b.write_from(q, i + 1);
				lua_pop(q, retc);
				lua_state_stack.pop();
				return lua_status_yield;
			}
		};
		default: {
			auto text = lua_tostring(q, -1);
			luaL_traceback(q, q, text, 0);
			text = lua_tostring(q, -1);
			buffer b(data);
			b.write_string(text);
			lua_pop(q, lua_gettop(q));
			lua_state_exec_end(qt);
			lua_state_stack.pop();
			return lua_status_error;
		};
	}
}

dllx double lua_state_exec_raw(void* data) {
	if (lua_state_stack.empty()) return lua_status_no_state;
	auto qt = lua_state_stack.top();
	return lua_state_exec(qt, qt->substates.top(), data, false);
}

dllx double lua_call_raw(double state_id, char* name, void* data) {
	auto qt = lua_state_find_t(state_id);
	if (qt == nullptr) return lua_status_no_state;
	auto q = lua_state_exec_start(qt);
	//
	lua_getglobal(q, name);
	if (lua_type(q, -1) != LUA_TFUNCTION) {
		lua_state_exec_end(qt);
		return lua_status_no_func;
	}
	lua_state_stack.push(qt);
	return lua_state_exec(qt, q, data, true);
}
#pragma endregion

#pragma region add code
dllx double lua_add_code_raw(double state_id, char* code, void* data) {
	auto qt = lua_state_find_t(state_id);
	if (qt == nullptr) return lua_status_no_state;
	lua_state_stack.push(qt);
	auto q = lua_state_exec_start(qt);
	luaL_loadstring(q, code);
	return lua_state_exec(qt, q, data, true);
}

dllx double lua_add_file_raw(double state_id, char* full_path, void* data) {
	auto qt = lua_state_find_t(state_id);
	if (qt == nullptr) return lua_status_no_state;
	lua_state_stack.push(qt);
	auto q = lua_state_exec_start(qt);
	luaL_loadfile(q, full_path);
	return lua_state_exec(qt, q, data, true);
}
#pragma endregion

#pragma region globals
dllx double lua_global_get_raw(double state_id, char* name, void* data) {
	auto q = lua_state_find(state_id);
	if (q == nullptr) return false;
	lua_getglobal(q, name);
	buffer b(data);
	b.write_from(q, 1);
	lua_pop(q, 1);
	return true;
}
dllx double lua_global_set_raw(double state_id, char* name, void* data) {
	auto q = lua_state_find(state_id);
	if (q == nullptr) return false;
	buffer b(data);
	b.read_to(q);
	lua_setglobal(q, name);
	return true;
}
///
enum lua_type_t {
	lua_type_none,
	lua_type_nil,
	lua_type_bool,
	lua_type_number,
	lua_type_string,
	lua_type_table,
	lua_type_function,
	lua_type_thread,
	lua_type_userdata,
	lua_type_lightuserdata,
	lua_type_unknown,
};
dllx double lua_global_type_raw(double state_id, char* name) {
	auto q = lua_state_find(state_id);
	if (q == nullptr) return -1;
	lua_getglobal(q, name);
	int t = lua_type(q, -1);
	lua_pop(q, 1);
	switch (t) {
		case LUA_TNONE: return lua_type_none;
		case LUA_TNIL: return lua_type_nil;
		case LUA_TBOOLEAN: return lua_type_bool;
		case LUA_TNUMBER: return lua_type_number;
		case LUA_TSTRING: return lua_type_string;
		case LUA_TTABLE: return lua_type_table;
		case LUA_TFUNCTION: return lua_type_function;
		case LUA_TTHREAD: return lua_type_thread;
		case LUA_TUSERDATA: return lua_type_userdata;
		case LUA_TLIGHTUSERDATA: return lua_type_lightuserdata;
		default: return lua_type_unknown;
	}
}
#pragma endregion

#pragma region coroutines
dllx double lua_call_start_raw(double state_id, char* name, void* data) {
	auto qt = lua_state_find_t(state_id);
	if (qt == nullptr) return lua_status_no_state;
	auto q = lua_state_exec_start(qt);
	lua_getglobal(q, name);
	if (lua_type(q, -1) != LUA_TFUNCTION) {
		lua_state_exec_end(qt);
		return lua_status_no_func;
	}
	//
	buffer b(data);
	auto argc = b.read<int32>();
	for (int i = 0; i < argc; i++) b.read_to(q);
	//
	return lua_status_done;
}
dllx double lua_call_next_raw(double state_id, void* data) {
	auto qt = lua_state_find_t(state_id);
	if (qt == nullptr) return lua_status_no_state;
	if (qt->substates.empty()) return lua_status_no_state;
	//
	lua_state_stack.push(qt);
	return lua_state_exec(qt, qt->substates.top(), data, true);
}
#pragma endregion