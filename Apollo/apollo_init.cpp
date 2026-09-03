// apollo_init.cpp:
#include "stdafx.h"
#include "apollo_ref.h"

struct lua_init_t {
	gml_script_id array_getter;
	gml_script_id array_setter;
	gml_script_id array_length;
	gml_script_id struct_getter;
	gml_script_id struct_setter;
	gml_script_id struct_length;
	gml_script_id struct_keys;
};
dllx double lua_init_raw(lua_init_t* init) {
	apollo_array::getter = init->array_getter;
	apollo_array::setter = init->array_setter;
	apollo_array::length = init->array_length;
	apollo_struct::getter = init->struct_getter;
	apollo_struct::setter = init->struct_setter;
	apollo_struct::length = init->struct_length;
	apollo_struct::gml_keys = init->struct_keys;
	return true;
}