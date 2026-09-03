// apollo_ref.cpp
#include "apollo_buffer.h"
#include "apollo_ref.h"

#include <queue>
static std::queue<int64> ref_recycle{};
#define apollo_ref
namespace apollo_array {
	#include "apollo_ref_shared.h"
	static int len(lua_State* q) {
		auto box = toImpl(q, 1);
		if (!box) return 0;
		//
		buffer b(lua_outbuf);
		b.write<gml_script_id>(length);
		b.write<int32>(1);
		b.write_lua_int64(box->index);
		lua_pop(q, lua_gettop(q));
		lua_yield_status = lua_status_call;
		return lua_yield(q, 0);
	}

	static int toString(lua_State* q) {
		auto box = toImpl(q, 1);
		if (!box) return 0;
		lua_pushfstring(q, "gml_array#%I", box->index);
		return 1;
	}

	luaL_Reg meta[] = {
		{"__gc", gc},
		{"__tostring", toString},
		{"__index", get},
		{"__newindex", set},
		{"__len", len},
		{0, 0}
	};
	void init(lua_State* q) {
		luaL_newmetatable(q, metaName);
		luaL_setfuncs(q, meta, 0);
		luaL_dostring(q, R"(return function(arr) -- pairs
	local i = 1
	local n = nil
	return function()
		if n == nil then
			n = #arr or 0
		end
		local k = i
		i = i + 1
		if k <= n then
			return k, arr[k]
		else
			return nil, nil
		end
	end, q, nil
end)");
		lua_setfield(q, -2, "__pairs");
		lua_pop(q, 1);
	}
}

namespace apollo_struct {
	#include "apollo_ref_shared.h"
	gml_script_id gml_keys;
	static int toString(lua_State* q) {
		auto box = toImpl(q, 1);
		if (!box) return 0;
		lua_pushfstring(q, "gml_struct#%I", box->index);
		return 1;
	}

	static int keys(lua_State* q) {
		auto box = toImpl(q, 1);
		if (!box) return 0;
		//
		buffer b(lua_outbuf);
		b.write<gml_script_id>(gml_keys);
		b.write<int32>(1);
		b.write_lua_int64(box->index);
		lua_pop(q, lua_gettop(q));
		lua_yield_status = lua_status_call;
		return lua_yield(q, 0);
	}

	luaL_Reg meta[] = {
		{"__gc", gc},
		{"__tostring", toString},
		{"__index", get},
		{"__newindex", set},
		{"__gml_keys", keys},
		{0, 0}
	};
	void init(lua_State* q) {
		luaL_newmetatable(q, metaName);
		luaL_setfuncs(q, meta, 0);
		luaL_dostring(q, R"(return function(q) -- pairs
	local keys = nil
	local i = 1
	local n
	return function()
		if keys == nil then
			keys = getmetatable(q).__gml_keys(q)
			n = #keys
		end
		local k = keys[i]
		i = i + 1
		if k ~= nil then
			return k, q[k]
		else
			return nil, nil
		end
	end, q, nil
end)");
		lua_setfield(q, -2, "__pairs");
		lua_pop(q, 1);
	}
}
#undef apollo_ref

dllx double lua_update_ref_gc(int64* out, double _max) {
	auto n = ref_recycle.size();
	auto max = (size_t)_max;
	if (n > max) n = max;
	for (auto i = 0u; i < n; i++) {
		out[i] = ref_recycle.front();
		ref_recycle.pop();
	}
	return (double)n;
}
