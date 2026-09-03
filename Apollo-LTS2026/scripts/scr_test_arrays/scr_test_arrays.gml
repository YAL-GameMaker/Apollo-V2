function scr_test_arrays(q) {
	//#mark arrays
	lua_add_code(q, @'
		arr_get = function(a) return a[2] end
		arr_len = function(a) return #a end
	');
	var arr = [4, 5, 6];
	assert(lua_call(q, "arr_get", arr), 5);
	assert(lua_call(q, "arr_len", arr), 3);
	
	//#mark structs
	lua_add_code(q, @'
		obj_get1 = function(o) return o.one end
		obj_get2 = function(o) return o.two end
	');
	var obj = { one: 1, etc: 0 };
	assert(lua_call(q, "obj_get1", obj), 1);
	assert(lua_call(q, "obj_get2", obj), undefined);
}