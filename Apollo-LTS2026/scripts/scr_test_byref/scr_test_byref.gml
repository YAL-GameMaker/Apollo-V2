function scr_test_byref(q) {
	//#mark byref arrays
	lua_add_code(q, @'function f(arr)
		local sum = 0
		for i, v in pairs(arr) do
			sum = sum + v
			arr[i] = -1
		end
		return sum
	end');
	var arr = [4, 5, 6];
	assert(lua_call(q, "f", lua_byref(arr)), 15);
	assert(arr[0], -1);
	
	//#mark byref structs
	lua_add_code(q, @'function f(obj)
		local sum = 0
		for k, v in pairs(obj) do
			sum = sum + v
			obj[k] = -1
		end
		return sum
	end');
	var obj = { a: 1, b: 2, c: 3 };
	assert(lua_call(q, "f", lua_byref(obj)), 6);
	assert(obj.a, -1);
}