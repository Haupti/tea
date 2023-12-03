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
* functions - these take arguments and return something
* instructions/procedures or sth. - these may or may not take arguments but do not return something
* lists or at least something like it, i have a slightly different idea for that
* variables
* update syntax checker when problem occurs
* update sanity checker when problem occurs
* io
### things i think about
* using LLVM (not at the moment, i want to do it myself once)

## version feature list (short)
* **0.0.1 (not tagged)** evaluation of expressions containing '(', ')', '1', '0', '|' and '&'
* **0.0.2** not modifier '!' to prefix groups (brackets), values or values with modifier: `!1 -> 0`, `!!1 -> 0`, `!(!1) | !(0) -> 1`, `!(0 & 1) -> 1` etc..
* **0.0.3** reimplemented the algorithm: its way faster now. still hard to read.
* **0.0.4** implement comments and constants. this works now: `set a = 0; (a | set b = 1 b) -- this is a comment` and evaluates to 1
* **0.0.5** reimplement the algorith again: it actually works correctly now and is easier to understand
* **0.0.6** add sanity checker so more problems are 'pre-execution time errors' (see long description)
* **0.0.7** add support conditional statement `if 0 then 1 else 0 end`
* **0.1.0** add functions ... however see long description

## version feature list (long)
### 0.0.6
i know this is an interpreted language. \
however, it works in several phases:
1. the lexing/tokenizing (pretty standart)
2. syntax verification, this checks for:
    - unclosed brackets
    - closed brackets before open
    - incomplete assignments
    - assignments directly before a combinator (&,|)
    - directly chained combinators (1 | | 1)
    - some other stuff
3. sanity check, this checks for:
    - use before assignment of values
    - duplicate assignment/ overwrite: `set a = 1; set a = 0; a` is not allowed, same for `set a = (set a = 1; a); a`
    - some other stuff
4. building the binary tree starting from the entrypoint
5. THEN the tree is evaluated
so when i write 'pre-execution time' i mean any phase before actuall evaluating the tree.

## 0.1.0
functions work but to make it work i had to heavily cut from the syntax verifier\
i do not like that.\
also i did not add additional verifications for functions.\
that basically means: if you do it right, it will work. if you do it wrong, you will probably not be wared beforehand\
and either get an error that basically tells you nothing OR you will get some random wrong result\

