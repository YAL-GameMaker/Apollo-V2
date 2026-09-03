// apollo_ref_shared.h
// note: partial!

gml_script_id getter;
gml_script_id setter;
gml_script_id length;
#ifndef apollo_ref
#include "apollo_ref.h"
extern const char* metaName;
#endif
static int toString(lua_State* q);

static impl* toImpl(lua_State* q, int index) {
	luaL_checktype(q, index, LUA_TUSERDATA);
	impl* box = (impl*)luaL_checkudata(q, index, metaName);
	if (box == nullptr) luaL_typeerror(q, index, metaName);
	return box;
}
static int gc(lua_State* q) {
	auto box = toImpl(q, 1);
	if (!box) return 0;
	ref_recycle.push(box->index);
	return 0;
}
static int get(lua_State* q) {
	auto box = toImpl(q, 1);
	if (!box) return 0;
	//
	buffer b(lua_outbuf);
	b.write<gml_script_id>(getter);
	b.write<int32>(3);
	b.write_lua_int64(box->index);
	b.write_from(q, 2);
	b.write_lua_bool(box->rec);
	lua_pop(q, lua_gettop(q));
	lua_yield_status = lua_status_call;
	return lua_yield(q, 0);
}
static int set(lua_State* q) {
	auto box = toImpl(q, 1);
	if (!box) return 0;
	//
	buffer b(lua_outbuf);
	b.write<gml_script_id>(setter);
	b.write<int32>(3);
	b.write_lua_int64(box->index);
	b.write_from(q, 2);
	b.write_from(q, 3);
	lua_pop(q, lua_gettop(q));
	lua_yield_status = lua_status_call;
	return lua_yield(q, 0);
}

void create(lua_State* q, int64 index, bool rec) {
	impl* box = (impl*)lua_newuserdata(q, sizeof(impl));
	box->index = index;
	box->rec = rec;
	luaL_getmetatable(q, metaName);
	lua_setmetatable(q, -2);
}