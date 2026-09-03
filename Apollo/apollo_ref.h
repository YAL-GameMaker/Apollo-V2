// apollo_ref.h
#pragma once
#include "stdafx.h"
#include "apollo_method.h"

namespace apollo_array {
	struct impl {
		int64 index;
		bool rec;
	};
	constexpr const char* metaName = "gml_array";
	extern gml_script_id getter;
	extern gml_script_id setter;
	extern gml_script_id length;
	void init(lua_State* q);
	void create(lua_State* q, int64 index, bool rec);
}
namespace apollo_struct {
	struct impl {
		int64 index;
		bool rec;
	};
	constexpr const char* metaName = "gml_struct";
	extern gml_script_id getter;
	extern gml_script_id setter;
	extern gml_script_id length;
	extern gml_script_id gml_keys;
	void init(lua_State* q);
	void create(lua_State* q, int64 index, bool rec);
}