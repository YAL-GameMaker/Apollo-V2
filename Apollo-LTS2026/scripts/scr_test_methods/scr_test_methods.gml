function scr_test_methods(q) {
	//#mark methods
	lua_add_code(q, @'function f(m)
		return m()
	end');
	var fn = function() {
		return "OK!"
	}
	assert(lua_call(q, "f", fn), "OK!")
	assert(lua_call(q, "f", lua_byref(fn)), "OK!")
	
	var fn2 = function() {
		return function() { return "OK2!" }
	}
	lua_add_function(q, "get_ok2", fn2);
	lua_add_code(q, @'function f2(m)
		local _f = get_ok2()
		return _f()
	end');
	assert(lua_call(q, "f2", fn), "OK2!")
	
	lua_global_set(q, "an_ok", fn);
	lua_add_code(q, @'function f3() return an_ok() end');
	assert(lua_call(q, "f3"), "OK!")
}