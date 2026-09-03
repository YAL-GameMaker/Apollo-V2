function scr_test_calls(q) {
	//#mark call
	lua_add_code(q, "function f(a, b) return a + b end");
	assert(lua_call(q, "f", 40, 1), 41);
	assert(lua_call_w(q, "f", pack(40, 1)), 41);

	//#mark subcall
	lua_add_code(q, "function ret_1() return 1 end");
	lua_add_code(q, "function ret_ret_1() return call_name('ret_1') end");
	assert(lua_call(q, "ret_ret_1"), 1);

	//#mark multi
	lua_add_code(q, "function f(a, b) return a + 1, b + 1 end");
	assert(lua_call_m(q, "f", 7, 10), pack(8, 11));
	w = pack(0,0,0,0);
	assert(lua_call_xm(q, "f", w, 7, 10), 2);
	assert(w, pack(8, 11, 0, 0));
	assert(lua_call_wm(q, "f", pack(7, 10)), pack(8, 11));
	w = pack(0,0,0,0);
	assert(lua_call_wxm(q, "f", pack(7, 11), w), 2);
	assert(w, pack(8, 12, 0, 0));

	//#mark return
	lua_add_code(q, "out = add(1, 2);");
	assert(lua_global_get(q, "out"), 3);

	// multi-return:
	lua_add_code(q, "function f(t) return lua_return(t, t + 1, t + 2) end");
	assert(lua_call_m(q, "f", 4), pack(4, 5, 6));

	// add-return:
	lua_add_code(q, "function f(t) return lua_return_add(t, t + 1, t + 2) end");
	assert(lua_call_m(q, "f", 4), pack(5, 6, 7));
}