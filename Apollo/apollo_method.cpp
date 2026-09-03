// apollo_method.cpp:
#include "apollo_method.h"
#include "apollo_buffer.h"
#include <queue>
namespace apollo_method {
	std::queue<int64> recycle{};
	static impl* toImpl(lua_State* q, int index) {
		luaL_checktype(q, index, LUA_TUSERDATA);
		impl* box = (impl*)luaL_checkudata(q, index, metaName);
		if (box == nullptr) luaL_typeerror(q, index, metaName);
		return box;
	}
	static int gc(lua_State* q) {
		auto box = toImpl(q, 1);
		if (!box) return 0;
		recycle.push(box->index);
		return 0;
	}
	static int toString(lua_State* q) {
		auto box = toImpl(q, 1);
		if (!box) return 0;
		lua_pushstring(q, box->nameStart);
		return 1;
	}
	static int call(lua_State* q) {
		auto box = toImpl(q, 1);
		if (!box) return 0;
		//
		int argc = lua_gettop(q) - 1;
		buffer b(lua_outbuf);
		b.write<int64>(box->index);
		b.write<int32>(argc);
		for (int i = 0; i < argc; i++) b.write_from(q, i + 2);
		lua_pop(q, argc + 1);
		//
		lua_yield_status = lua_status_callmethod;
		return lua_yield(q, 0);
	}
	luaL_Reg meta[] = {
		{"__gc", gc},
		{"__tostring", toString},
		{"__call", call},
		{0, 0}
	};
	void init(lua_State* q) {
		luaL_newmetatable(q, metaName);
		luaL_setfuncs(q, meta, 0);
		lua_pop(q, 1);
	}
	void create(lua_State* q, int64 index, const char* fname) {
		auto fnamen = strlen(fname);
		auto size = sizeof(impl) + fnamen + 1;
		auto ptr = (impl*)lua_newuserdata(q, size);
		ptr->index = index;
		char* nameStart = ptr->nameStart;
		nameStart[fnamen] = 0;
		strncpy(nameStart, fname, fnamen);
		luaL_getmetatable(q, metaName);
		lua_setmetatable(q, -2);
	}
}

dllx double lua_update_method_gc(int64* out, double _max) {
	auto n = apollo_method::recycle.size();
	auto max = (size_t)_max;
	if (n > max) n = max;
	for (auto i = 0u; i < n; i++) {
		out[i] = apollo_method::recycle.front();
		apollo_method::recycle.pop();
	}
	return (double)n;
}

lua_status_t lua_yield_status;
int lua_script_closure(lua_State* q) {
	// Push the closure context from the pseudoindex (4.2)
	lua_pushvalue(q, lua_upvalueindex(1));

	// stack: [...args, script]
	int argc = lua_gettop(q) - 1;
	int script = (int)lua_tonumber(q, argc + 1);
	buffer b(lua_outbuf);
	b.write<int32>(script);
	b.write<int32>(argc);
	for (int i = 0; i < argc; i++) b.write_from(q, i + 1);
	lua_pop(q, argc + 1);
	//
	lua_yield_status = lua_status_call;
	return lua_yield(q, 0);
}