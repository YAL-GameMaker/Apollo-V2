function scr_test_add_code(q) {
	//#mark add_code
	lua_add_code(q, "out = 11");
	assert(lua_global_get(q, "out"), 11);
	lua_global_set(q, "out", 12);
	assert(lua_global_get(q, "out"), 12);
	assert(lua_global_type(q, "out"), lua_type_number);
	assert(lua_global_typeof(q, "out"), "number");

	lua_add_code(q, "function ret_1() return 1 end");
	assert(lua_global_type(q, "ret_1"), lua_type_function);
	assert(lua_call(q, "ret_1"), 1);

	//#mark add_file
	var i = file_text_open_write("test.lua");
	file_text_write_string(i, "function add_1(t) return t + 1 end");
	file_text_close(i);
	lua_add_file(q, "test.lua");
	assert(lua_call(q, "add_1", 2), 3);
}