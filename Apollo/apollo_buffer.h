// apollo_buffer.h:
#pragma once
#include "stdafx.h"

///~
enum lua_btype {
	lua_btype_nil,
	lua_btype_bool,
	lua_btype_int32,
	lua_btype_int64,
	lua_btype_real,
	lua_btype_string,
	lua_btype_array,
	lua_btype_struct,
	lua_btype_script,
	lua_btype_method,
	lua_btype_ref,
};

struct buffer {
	int8* pos;
public:
	buffer(void* origin) : pos((char*)origin) {}
	template<class T> T read() {
		T r = *(T*)pos;
		pos += sizeof(T);
		return r;
	}
	template<class T> void write(T val) {
		*(T*)pos = val;
		pos += sizeof(T);
	}
	template<class T> T* writeStore(T val) {
		auto p = (T*)pos;
		*p = val;
		pos += sizeof(T);
		return p;
	}
	//
	lua_btype read_type() { return (lua_btype)read<char>(); }
	void write_type(lua_btype t) { write<int8>((int8)t); }
	void write_lua_bool(bool val) { write_type(lua_btype_bool); write(val); }
	void write_lua_real(double val) { write_type(lua_btype_real); write(val); }
	void write_lua_int64(long long val) { write_type(lua_btype_int64); write(val); }
	//
	const char* read_string() {
		const char* r = pos;
		while (*pos != 0) pos++;
		pos++;
		return r;
	}
	void write_string(const char* s) {
		if (s != nullptr) for (int i = 0; s[i] != 0; i++) write<char>(s[i]);
		write<char>(0);
	}
	/// Reads a value and pushes it to Lua state
	void read_to(lua_State* q);
	/// Writes i-th value on state's stack
	void write_from(lua_State* q, int i);
};
