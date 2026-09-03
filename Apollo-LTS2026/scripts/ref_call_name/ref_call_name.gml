function ref_call_name(argument0) {
	var s = argument0;
	var r = lua_call(lua_current, s);
	//show_error("?",0);
	//trace("call_name", s, r);
	return r;



}
