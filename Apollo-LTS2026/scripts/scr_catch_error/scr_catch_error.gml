globalvar lua_last_error, lua_log_errors;
lua_log_errors = true;
function scr_catch_error(q, e) {
	lua_last_error = e;
	if (lua_log_errors) trace(e);
}
