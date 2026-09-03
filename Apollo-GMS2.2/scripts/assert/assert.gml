var a = string(argument0);
var b = string(argument1);
if (a != b) show_error("Assertion failed!"
    + chr(10) + "-> " + a + chr(10) + "<- " + b, 1);
