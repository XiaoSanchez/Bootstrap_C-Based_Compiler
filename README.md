
# Bootstrap Compiler

A bootstrap Capable Compiler (Demo) that supports some core features.
## Design Concept

- Front-end and back-end in one - Omit intermediate optimization code
- Target code based on Custom Virtual Machine
- One-Pass Parse: Source code -> Parse & CodeGen -> V.M. executable code

 #### I. Virtual Machine Design
- Calculations based on: Register & Stack
- Register: PC / SP / BP / AX
- MeM: Code / Data / Stack
- Instruction set: S&L / (a/b/c) Operations / Branch Jumping / Native-call

 #### II. Virtual Machine Operation Principle
           +-----+ +-----+
    PC ->  | NSVA| | 'H' |  <-bp      ALU:(ax)+(sp)
           +-----+ +-----+
           |  I  | | 'E' |  <-sp
           +-----+ +-----+
           | IMM | | 'L' |
           +-----+ +-----+
           | 433.| | 'L' |    |  ^
           +-----+ +-----+    |  |
           | PUSH| | 'O' |    |  |
           +-----+ +-----+    |  |
           | PRTF| | ' ' |    |  |
           +-----+ +-----+    |  |
           | DSAR| | 'W' |    |  |
           +-----+ +-----+    |  |
           |  I  | | 'O' |    |  |
           +-----+ +-----+    |  |
           | IMM | | 'R' |    v  |
           +-----+ +-----+
           |  O  | | 'L' | 
           +-----+ +-----+
    Max    | RET | | 'D' |
           +-----+ +-----+
            Code -> Data -> Stack

 #### III. Parse Statement:
    · if_statement
    · while_statement
    · for_statement
    · do_while_statement
    · break_statement
    · continue_statement
    · {statement}
    · return [expression]
    · [expression]
    · id : statement
    · goto id

 #### IV. Analytic function:
    +---------------+
    |    ....       | high address
    +---------------+
    | arg: param_a  |    new_bp + 3
    +---------------+
    | arg: param_b  |    new_bp + 2
    +---------------+
    |return address |    new_bp + 1
    +---------------+
    | old BP        | <- new BP
    +---------------+
    | local_1       |    new_bp - 1
    +---------------+
    | local_2       |    new_bp - 2
    +---------------+
    |    ....       |  low address
    +---------------+

 #### V. Allocation Memory:
    ...             High address        reletive address to bp
    variable a                          -1
    end of struct s                     -2
    ...
    begin of struct s                   -(1+sizeof(struct node)/sizeof(int)) <---- address of s
    variable b
    ...             Low addresss
## Features

- Types: `char`, `int`, `enum`, `pointer`
- Operators: `+`, `-`, `*`, `/`, `%`, `++`, `--`, `[]`, `()`, `,`, `=`, `?`, `:`, `||`, `&&`, `|`, `^`, `&`, `==`, `!`, `=`, `<`, `>`, `<=`, `>=`, `<<`, `>>`
- Control flow: `for` `do while` `break` `continue` `goto`
- Function:`union` and `struct`, and operator `. ->`
    - Global variable and local variable declarations.
    - All operations using pointers.
    - Forward declarations. Checks the return value type.
    - Align to `sizeof(int)` bytes.

## Installation

Get the BSCP to your local machine:

```bash
  git clone https://github.com/XiaoSanchez/bscp
  cd bscp
```
Try the following:
```bash
  make
  bscp [-d] [filename].c [args to main]
```
Refactoring and Testing:
macOS || Linux
```bash
  ./compile ./test/greeting.c
  ./compile ./test/bscp.c ./test/greeting.c
``` 
Windows
Execute `Compile.exe`
```bash
./test/greeting.c
./test/bscp.c ./test/greeting.c
```
## Related

Here are some related projects

[C4](https://github.com/rswier/c4)  
[The LEMON Parser Generator](http://www.hwaci.com/sw/lemon/)    
[C Interpreter 0](https://lotabout.me/2015/write-a-C-interpreter-0/)    
[Let's Build a Compiler](https://compilers.iecc.com/crenshaw/)

## To Be Fixed

- Fail to bootstrap with gcc. (FIXED)

- Only '*' can be used to declare an array; '[]' will not work. (NOT FIXED)

- Classes and tokens declared in comments in the form '/** **/' will still be executed. (NOT FIXED)

- ';' in a for loop will be interpreted as a separate statement. (NOT FIXED)

- FILE to be implemented. (NOT FIXED)
## License

[MIT](https://choosealicense.com/licenses/mit/)
