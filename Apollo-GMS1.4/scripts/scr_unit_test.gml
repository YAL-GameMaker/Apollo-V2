//show_message("bok");
lua_last_error = "no errors!";
var q = lua_state_create();
lua_error_handler = scr_catch_error;
var i, w, c;
for (i = 0; script_exists(i); i++) {
    var s = script_get_name(i);
    if (string_copy(s, 1, 4) != "ref_") continue;
    lua_add_function(q, string_delete(s, 1, 4), i);
}

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
i = file_text_open_write("test.lua");
file_text_write_string(i, "function add_1(t) return t + 1 end");
file_text_close(i);
lua_add_file(q, "test.lua");
assert(lua_call(q, "add_1", 2), 3);

//#mark errors
lua_add_code(q, "lua_show_error('shown error')");
assert(string_pos("shown error", lua_last_error) >= 0, true);
lua_last_error = "";

lua_add_code(q, "ohno = ohno .. 4; print(ohno)");
assert(lua_last_error != "", true);
lua_last_error = "";

//#mark call
lua_add_code(q, "function f(a, b) return a + b end");
assert(lua_call(q, "f", 40, 1), 41);
assert(lua_call_w(q, "f", pack(40, 1)), 41);

//#mark subcall
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

//#mark call_start
lua_add_code(q, '
function ret_cr(i)
    coroutine.yield(i + 1)
    coroutine.yield(i + 2)
    return i + 3;
end
');
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
//
lua_state_reuse_indexes();
var q1 = lua_state_create();
assert(q1, q1);
lua_state_destroy(q1);
//
trace("OK!");
