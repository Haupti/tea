# TEA
its called tea because i made tea while i had the idea for the project

## What is tea?
at the moment its just an interpreter for boolean logic.\
It can only evaluate AND, OR, 1 and 0 combinations grouped by brackets ().\
for example:\
`(1 & 0)` evaluates to 0\
so does `1 & 2`

`(1 | 0) & 1` evalutes to 1\
and `1 | 1 & 0` raises an exception because its not properly grouped.\
`1 steve|20` evaluates to 1 because everything except '(', ')', '1', '0', '&' and '|' is ignored.

## Future plans
* exceptions when finding characters that are now allowed (maybe ?)
* not operator
* syntax checker that runs over the tokens
* lists
* variables
* functions
* io
there is no need for types. everything is 1 or 0
