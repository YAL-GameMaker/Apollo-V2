scr_unit_test();

var q = lua_state_create();
lua_add_code(q, @'
function tab(t)
	for i,v in pairs(t) do print(i,v) end
	for i,v in pairs(t.struct) do print(i,v) end
	for i,v in ipairs(t.array) do print(i,v) end
	return {
		one = 1,
		two = 2,
		table = { c = 3, d = 4 },
		array = { 5, 6, 7 }
	}
end
');
var r = lua_call(q, "tab", table(
	"one", 1,
	"two", 2,
	"struct", table("c", 3, "d", 4),
	"array", [5, 6, 7]
));
for (var i = 0; i < array_length_2d(r, 0); i++) {
	trace(r[1, i], r[0, i]);
}

/*state = lua_state_create();
lua_add_function(state, "add", scr_add);
lua_add_function(state, "multi", scr_multi);
lua_add_code(state, "function q(t) return multi() end");
trace(lua_call_m(state, "q", "test"));

/* */
/*  */
