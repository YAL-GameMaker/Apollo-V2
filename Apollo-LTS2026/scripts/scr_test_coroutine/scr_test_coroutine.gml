function scr_test_coroutine(q) {
	//#mark call_start
	lua_add_code(q, @'function ret_cr(i)
		coroutine.yield(i + 1)
		coroutine.yield(i + 2)
		return i + 3;
	end');
	var th = lua_thread_create(q);
	assert(lua_call_start(th, "ret_cr", 4), 1);
	assert(lua_call_next(th), 1);
	assert(lua_call_result, 5);
	assert(lua_call_next(th), 1);
	assert(lua_call_result, 6);
	assert(lua_call_next(th), 0);
	assert(lua_call_result, 7);
	lua_thread_destroy(th);
	//
	assert(lua_state_get_interop_depth(q), 0);
	lua_state_destroy(q);
}