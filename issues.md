# issues

* self referencing assignments `set a = a;` compile but dont evaluate -> check that before evaluation so it is a 'compile time error'
* multiple assignments are weird `set a = 1; set a = 0; a` evaluates to `1` instead of the expected `0`;
    - either dont allow reassignment (in the same scope)
    - or check on reassignment if a identfier with that name already exists
