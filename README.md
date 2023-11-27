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

### definitive plans
* variables
* update syntax checker when problem occurs
* functions
* lists or at least something like it, i have a slightly different idea for that
* io
### things i think about
* using LLVM (not at the moment, i want to do it myself once)

## version feature list (short)
* **0.0.1 (not tagged)** evaluation of expressions containing '(', ')', '1', '0', '|' and '&'
* **0.0.2** not modifier '!' to prefix groups (brackets), values or values with modifier: `!1 -> 0`, `!!1 -> 0`, `!(!1) | !(0) -> 1`, `!(0 & 1) -> 1` etc..
* **0.0.3** reimplemented the algorithm: its way faster now. still hard to read.
* **0.0.4** implement comments and constants. this works now: `set a = 0; (a | set b = 1 b) -- this is a comment` and evaluates to 1
* **0.0.5** reimplement the algorith again: it actually works correctly now and is easier to understand
