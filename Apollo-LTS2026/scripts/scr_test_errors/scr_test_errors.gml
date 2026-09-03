function scr_test_errors(q) {
	//#mark errors
	lua_log_errors = false;
	lua_add_code(q, "lua_show_error('shown error')");
	assert(string_pos("shown error", lua_last_error) >= 0, true);
	lua_last_error = "";

	lua_add_code(q, "ohno = ohno .. 4; print(ohno)");
	assert(lua_last_error != "", true);
	lua_last_error = "";
	lua_log_errors = true;
}