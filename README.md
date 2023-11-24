# TEA
its called tea because i made tea while i had the idea for the project

## What is tea?
at the moment its just an interpreter for boolean logic.\
It can only evaluate AND, OR, 1 and 0 combinations grouped by brackets ().\
for example:\
`(1 & 0)` evaluates to 0\
so does `1 & 0` and `!(1 | 0)`

`(1 | 0) & 1` evalutes to 1\
and `1 | 1 & 0` raises an exception because its not properly grouped.\
`1 steve|20` evaluates to 1 because everything except '(', ')', '1', '0', '&', '|' and '!' is ignored.

## Future plans
* exceptions when finding characters that are now allowed (maybe ?)
* update syntax checker when problem occurs
* lists
* variables
* functions
* io
there is no need for types. everything is 1 or 0

## version feature list (short)
* **0.0.1 (not tagged)** evaluation of expressions containing '(', ')', '1', '0', '|' and '&'
* **0.0.2** not modifier '!' to prefix groups (brackets), values or values with modifier: `!1 -> 0`, `!!1 -> 0`, `!(!1) | !(0) -> 1`, `!(0 & 1) -> 1` etc..
