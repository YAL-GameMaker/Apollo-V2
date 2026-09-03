// apollo_method.h:
#pragma once
#include "stdafx.h"

extern lua_status_t lua_yield_status;
int lua_script_closure(lua_State* q);

namespace apollo_method {
	constexpr const char* metaName = "gml_method";
	struct impl {
		int64 index;
		char nameStart[];
	};
	void init(lua_State* q);
	void create(lua_State* q, int64 index, const char* fname);
}