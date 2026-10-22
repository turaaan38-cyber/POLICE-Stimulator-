# PatrolScript 0.1 — initial language specification

Status: the syntax/runtime described below is implemented and covered by unit tests where noted; it is not a mission DSL yet.

## Source and diagnostics

- UTF-8-compatible byte-oriented source text; identifiers currently accept ASCII letters/underscore followed by ASCII letters, digits, or underscores.
- `//` starts a comment through end-of-line.
- Errors are reported as `path:line:column: error: message` by `patrolc`.
- String escapes: `\\`, `\"`, `\n`, `\r`, and `\t`.

## Grammar (EBNF)

```ebnf
program       = declaration* EOF ;
declaration   = functionDecl | variableDecl | statement ;
functionDecl  = "fn" IDENTIFIER "(" parameters? ")" block ;
parameters    = IDENTIFIER ("," IDENTIFIER)* ;
variableDecl  = "let" IDENTIFIER ("=" expression)? ";" ;
statement     = "print" expression ";"
              | "if" "(" expression ")" statement ("else" statement)?
              | "while" "(" expression ")" statement
              | "return" expression? ";"
              | block
              | expression ";" ;
block         = "{" declaration* "}" ;
expression    = assignment ;
assignment    = logicOr ("=" assignment)? ;
logicOr       = logicAnd ("||" logicAnd)* ;
logicAnd      = equality ("&&" equality)* ;
equality      = comparison (("==" | "!=") comparison)* ;
comparison    = term ((">" | ">=" | "<" | "<=") term)* ;
term          = factor (("-" | "+") factor)* ;
factor        = unary (("/" | "*") unary)* ;
unary         = ("!" | "-") unary | call ;
call          = primary ("(" arguments? ")")* ;
arguments     = expression ("," expression)* ;
primary       = NUMBER | STRING | "true" | "false" | "nil"
              | IDENTIFIER | "(" expression ")" ;
```

The parser currently caps parameters and arguments at 64 per function/call. `while` is guarded by a one-million-iteration runtime limit per loop execution. This is a basic protection, not a full sandbox; recursion and total instruction/memory quotas remain unimplemented.

## Runtime values and semantics

- Values: `number` (IEEE-754 `double`), `string`, `boolean`, `nil`, and function.
- Variables declared by `let` default to `nil` if no initializer is given.
- Blocks create lexical scopes; assignment updates the nearest existing binding and fails for an undefined name.
- `+` accepts two numbers or two strings. Other arithmetic and comparisons require numbers. Division by zero is a runtime error.
- `&&` and `||` short-circuit and return an operand value. `nil` and `false` are falsey; other values are truthy.
- Functions capture their declaration environment; `return` transfers a value to the caller. `return` outside a function is an error.
- `print` writes one formatted value per line.
- Initial native functions: `clock()` returns seconds since this runtime instance began; `abs(number)` returns absolute value.

## Not in 0.1

No arrays, maps/objects, classes, imports/modules, exceptions in user programs, `mission`/`on_start`/`on_complete` blocks, async tasks, or game-engine APIs. Those require later language/runtime design and tests before they can be advertised as available.
