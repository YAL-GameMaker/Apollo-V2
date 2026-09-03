scr_unit_test();
var q = lua_state_create();
lua_add_code(q, @'
function tab(t)
	for i,v in pairs(t) do print("lua:t." .. i .. " = " .. tostring(v)) end
	t.greet("Lua");
	--
	print("lua:#t.refArr = " .. #t.refArr)
	for i = 1, #t.refArr do
		print("lua:t.refArr["..i.."] = "..tostring(t.refArr[i]))
	end
	t.refArr[4] = "I come from Lua!"
	--
	print("lua:t.refStruct.e = " .. t.refStruct.e)
	t.refStruct.e = "lua!"
	t.refStruct.lua = "lua!!"
	--
	return {
		one = 1,
		two = 2,
		table = { c = 3, d = 4 },
		array = { 5, 6, 7 }
	}
end
');
name = "GML";
greet = function(who) {
	trace(sfmt("Hello to % from %!",who,name))
}
//show_message("h");
var arr = [5, 6, 7];
var obj = { e: 8 }
trace(lua_call(q, "tab", {
	one: 1,
	two: 2,
	struct: { c: 3, d: 4 },
	array: arr,
	greet: greet,
	refArr: lua_byref(arr),
	refStruct: lua_byref(obj),
}));
trace("post-lua arr:", arr);
trace("post-lua obj:", obj);
//
lua_global_set(q, "some", {a:1})
trace("lua obj:", lua_global_get(q, "some"));
// C-call boundary:
/*lua_add_function(q, "get_hi", function() {
	return "hi!";
})
lua_add_code(q, @'
a = {}
setmetatable(a, {
	__tostring = function() return get_hi() end
});
print(tostring(a))
');*/
//
lua_state_destroy(q);
/*state = lua_state_create();
lua_add_function(state, "add", scr_add);
lua_add_function(state, "multi", scr_multi);
lua_add_code(state, "function q(t) return multi() end");
trace(lua_call_m(state, "q", "test"));

/* */
/*  */
state = lua_state_create()
for (var f = 0; f < 100000; f++) {
    var fname = script_get_name(f);
    if (string_char_at(fname, 1) == "<") break; // no more functions, just "<undefined>"
    lua_add_function(state, fname, f);
}
lua_add_code(state, @'
    print("Lua print")
    show_debug_message("GML print")
')

if (0) {
lua_add_code(state, @"
   function test(num)
       for i = 1, num do
             coroutine.yield(i)
        end
    end
    function disrupt()
    end
");
show_message("");
th = lua_thread_create(state);
if (lua_call_start(th, "test", 10)) {
    while (lua_call_next(th)) {
        show_debug_message("yield: " + string(lua_call_result));
        for (var i = 0; i < 5; i++)
        {
             lua_call(state, "disrupt")
         }
    }
    show_debug_message("result: " + string(lua_call_result));
}
}
