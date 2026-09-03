function pack() {
	var i = argument_count;
	var r = array_create(i);
	while (--i >= 0) r[i] = argument[i];
	return r;



}
