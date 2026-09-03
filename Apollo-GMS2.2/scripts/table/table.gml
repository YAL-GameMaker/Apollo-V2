/// table(...key_value_pairs)
var n = argument_count >> 1;
var k = 0;
var r = [];
r[2, 0] = undefined;
for (var i = 0; i < n; i++) {
	r[1, i] = argument[k++];
	r[0, i] = argument[k++];
}
return r;