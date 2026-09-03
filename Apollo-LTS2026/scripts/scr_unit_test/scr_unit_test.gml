function scr_unit_test() {
	//show_message("bok");
	lua_last_error = "no errors!";
	lua_error_handler = scr_catch_error;
	var i, w, c;
	
	
	scr_test_run(scr_test_add_code);
	scr_test_run(scr_test_errors);
	scr_test_run(scr_test_calls);
	scr_test_run(scr_test_arrays);
	scr_test_run(scr_test_byref);
	scr_test_run(scr_test_methods);
	scr_test_run(scr_test_coroutine);
	//
	var q = lua_state_create();
	lua_state_reuse_indexes();
	var q1 = lua_state_create();
	assert(q1, q1);
	lua_state_destroy(q1);
	//
	trace("OK!");
}
function scr_test_run(fn/*:function<any;void>*/) {
	trace(script_get_name(fn) + "...");
	var q = lua_state_create();
	for (var i = 100000; script_exists(i); i++) {
		var s = script_get_name(i);
		if (string_copy(s, 1, 4) != "ref_") continue;
		lua_add_function(q, string_delete(s, 1, 4), i);
	}
	
	fn(q);
	lua_state_destroy(q);
}