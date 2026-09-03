// apollo_buffer.cpp:
#include "apollo_buffer.h"
#include "apollo_method.h"
#include "apollo_ref.h"

void buffer::read_to(lua_State* q) {
	switch (read_type()) {
		case lua_btype_bool: lua_pushboolean(q, read<int8>()); break;
		case lua_btype_int32: lua_pushinteger(q, read<int32>()); break;
		case lua_btype_int64: lua_pushinteger(q, read<int64>()); break;
		case lua_btype_real: lua_pushnumber(q, read<real>()); break;
		case lua_btype_string: lua_pushstring(q, read_string()); break;
		case lua_btype_array: {
			lua_newtable(q);
			auto t = lua_gettop(q);
			auto n = read<uint32>();
			for (auto i = 0u; i < n; i++) {
				read_to(q);
				lua_rawseti(q, t, (lua_Integer)i + 1);
			}
		} break;
		case lua_btype_struct: {
			lua_newtable(q);
			auto t = lua_gettop(q);
			auto n = read<uint32>();
			for (auto i = 0u; i < n; i++) {
				lua_pushstring(q, read_string());
				read_to(q);
				lua_rawset(q, t);
			}
		} break;
		case lua_btype_script: {
			lua_pushnumber(q, read<int32>());
			lua_pushcclosure(q, lua_script_closure, 1);
		} break;
		case lua_btype_method: {
			auto index = read<int64>();
			auto name = read_string();
			apollo_method::create(q, index, name);
		} break;
		case lua_btype_ref: {
			auto index = read<int64>();
			auto kind = read<int8>();
			auto rec = read<int8>() != 0;
			if (kind == 0) {
				apollo_array::create(q, index, rec);
			} else {
				apollo_struct::create(q, index, rec);
			}
		} break;
		default: lua_pushnil(q);
	}
}

void buffer::write_from(lua_State* q, int i) {
	switch (lua_type(q, i)) {
		case LUA_TBOOLEAN:
			write_type(lua_btype_bool);
			write<int8>(lua_toboolean(q, i));
			break;
		case LUA_TNUMBER:
			write_type(lua_btype_real);
			write<real>(lua_tonumber(q, i));
			break;
		case LUA_TSTRING:
			write_type(lua_btype_string);
			write_string(lua_tostring(q, i));
			break;
		case LUA_TTABLE: {
			auto len = lua_rawlen(q, i); // lua_objlen in <= 5.1
			if (len > 0) {
				write_type(lua_btype_array);
				write<uint32>((uint32)len);
				for (auto k = 1; k <= len; k++) {
					lua_rawgeti(q, i, k);
					write_from(q, lua_gettop(q));
					lua_pop(q, 1);
				}
			} else {
				write_type(lua_btype_struct);
				auto foundAt = writeStore<uint32>(0);
				auto found = 0u;
				lua_pushnil(q);
				while (lua_next(q, i)) {
					auto key = lua_tostring(q, -2);
					if (key) {
						found++;
						write_string(key);
						write_from(q, lua_gettop(q));
					}
					lua_pop(q, 1);
				}
				*foundAt = found;
			}
		} break;
		case LUA_TUSERDATA:
			if (auto mtd = (apollo_method::impl*)luaL_testudata(q, i, apollo_method::metaName)) {
				write_type(lua_btype_method);
				write<int64>(mtd->index);
			} else if (auto arr = (apollo_array::impl*)luaL_testudata(q, i, apollo_array::metaName)) {
				write_type(lua_btype_ref);
				write<int64>(arr->index);
			} else if (auto obj = (apollo_struct::impl*)luaL_testudata(q, i, apollo_struct::metaName)) {
				write_type(lua_btype_ref);
				write<int64>(obj->index);
			}
			else write_type(lua_btype_nil);
			break;
		default: write_type(lua_btype_nil);
	}
}