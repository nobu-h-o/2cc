# 2cc - C Compiler with Flex and Bison

A C compiler that introduces professional compiler-building tools: Flex for lexical analysis and Bison for parsing. 2cc targets ARM64 architecture and represents a major evolution from hand-written compiler components to industry-standard tools.

## Compiler Evolution Series

2cc is the second iteration in a compiler learning series:

### 1cc → **2cc** → 3cc

| Stage | 1cc | **2cc** | 3cc |
|-------|-----|---------|-----|
| **Lexer** | Hand-written tokenizer | **Flex** ⭐ | Flex |
| **Parser** | Recursive descent | **Bison** ⭐ | Bison |
| **Language** | C | **C** | C++ |
| **Code Gen** | x86-64 Assembly | **ARM64 Assembly** ⭐ | LLVM IR |
| **Target** | x86-64 only | **ARM64 only** | Multi-arch |
| **Build** | Make | **Make** | CMake |

## What's New in 2cc

The key innovation in 2cc is the introduction of **Flex and Bison**:

### From Hand-Written to Generated Code

**1cc Approach (Hand-written):**
```c
// tokenize.c - Manual character scanning (100+ lines)
while (*p) {
    if (isspace(*p)) { p++; continue; }

    if (isdigit(*p)) {
        cur = cur->next = new_token(TK_NUM, p, p);
        cur->val = strtoul(p, &p, 10);
        continue;
    }

    if (strncmp(p, "return", 6) == 0 && !isalnum(p[6])) {
        cur = cur->next = new_token(TK_RETURN, p, p + 6);
        p += 6;
        continue;
    }

    if (strncmp(p, "while", 5) == 0 && !isalnum(p[5])) {
        cur = cur->next = new_token(TK_WHILE, p, p + 5);
        p += 5;
        continue;
    }

    // ... many more manual checks
}
```

**2cc Approach (Flex):**
```flex
/* lexer.l - Declarative pattern matching (40 lines) */
%%
"return"    { return RETURN; }
"while"     { return WHILE; }
"for"       { return FOR; }
[0-9]+      { yylval.number = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.string = strdup(yytext); return IDENTIFIER; }
"+"         { return ADD; }
"-"         { return SUB; }
%%
```

**Benefits:**
- **Concise**: 40 lines vs 100+ lines
- **Declarative**: Pattern matching instead of procedural code
- **Maintainable**: Adding new keywords is trivial
- **Error-free**: No manual pointer arithmetic
- **Generated code**: Flex generates optimized C code

### Parser: From Recursive Descent to Bison

**1cc Approach (Recursive Descent):**
```c
// parse.c - Manual recursive functions
Node *add(Token **rest, Token *tok) {
    Node *node = mul(&tok, tok);
    while (equal(tok, "+") || equal(tok, "-")) {
        Token *op = tok;
        tok = tok->next;
        if (equal(op, "+"))
            node = new_binary(ND_ADD, node, mul(&tok, tok));
        else
            node = new_binary(ND_SUB, node, mul(&tok, tok));
    }
    *rest = tok;
    return node;
}

Node *mul(Token **rest, Token *tok) {
    Node *node = unary(&tok, tok);
    while (equal(tok, "*") || equal(tok, "/")) {
        Token *op = tok;
        tok = tok->next;
        if (equal(op, "*"))
            node = new_binary(ND_MUL, node, unary(&tok, tok));
        else
            node = new_binary(ND_DIV, node, unary(&tok, tok));
    }
    *rest = tok;
    return node;
}
```

**2cc Approach (Bison):**
```yacc
/* parser.y - Formal grammar specification */
%%
expr:
    expr ADD term { $$ = ast_binary_op(AST_ADD, $1, $3); }
    | expr SUB term { $$ = ast_binary_op(AST_SUB, $1, $3); }
    | term { $$ = $1; }
    ;

term:
    term MUL factor { $$ = ast_binary_op(AST_MUL, $1, $3); }
    | term DIV factor { $$ = ast_binary_op(AST_DIV, $1, $3); }
    | factor { $$ = $1; }
    ;
%%
```

**Benefits:**
- **Formal grammar**: BNF-style specification
- **Self-documenting**: Grammar IS the documentation
- **Correct precedence**: Automatic operator precedence handling
- **Extensible**: Easy to add new language constructs
- **Error handling**: Built-in error recovery

## Features

2cc supports a C-like language with the following features:

- **Data Types**: 32-bit signed integers
- **Variables**: Local and global variables
- **Operators**:
  - Arithmetic: `+`, `-`, `*`, `/`
  - Comparison: `<`, `>`, `<=`, `>=`, `==`, `!=`
- **Control Flow**:
  - `while` loops
  - `for` loops
- **Functions**:
  - Function definitions with parameters
  - Function calls with argument passing
  - Recursive functions
- **I/O**: Built-in `print()` function
- **Return statements**: `return` keyword

## Requirements

- **C Compiler**: gcc or clang
- **Flex**: Fast lexical analyzer generator
- **Bison**: GNU parser generator
- **Make**: Build automation tool

### Installing Dependencies

**macOS (using Homebrew):**
```bash
brew install flex bison
```

**Ubuntu/Debian:**
```bash
sudo apt-get install flex bison gcc make
```

## Building

```bash
cd 2cc
make
```

This will:
1. Run Flex on `lexer.l` to generate `lex.yy.c`
2. Run Bison on `parser.y` to generate `parser.tab.c` and `parser.tab.h`
3. Compile the generated code with your C compiler
4. Link everything into the `2cc` executable

## Usage

```bash
./2cc "<source_code>" [output_file]
```

### Examples

**Simple return value:**
```bash
./2cc "main() { return 42; }" > program.s
as -o program.o program.s
ld -o program program.o -lSystem -arch arm64
./program
echo $?  # Prints: 42
```

**Arithmetic:**
```bash
./2cc "main() { x = 5; y = 10; return x + y; }" > program.s
as -o program.o program.s
ld -o program program.o -lSystem -arch arm64
./program
echo $?  # Prints: 15
```

**Loops:**
```bash
./2cc "main() { sum = 0; for (i = 1; i <= 10; i = i + 1) sum = sum + i; return sum; }" > program.s
as -o program.o program.s
ld -o program program.o -lSystem -arch arm64
./program
echo $?  # Prints: 55
```

**Recursive functions:**
```bash
./2cc "fact(n) { if (n <= 1) return 1; return n * fact(n-1); } main() { return fact(5); }" > program.s
as -o program.o program.s
ld -o program program.o -lSystem -arch arm64
./program
echo $?  # Prints: 120
```

**Print function:**
```bash
./2cc "main() { for (i = 1; i <= 5; i = i + 1) print(i); return 0; }" > program.s
as -o program.o program.s
ld -o program program.o -lSystem -arch arm64
./program  # Prints: 1 2 3 4 5
```

## Language Syntax

### Variables

Variables are implicitly declared on first assignment:

```c
main() {
    x = 5;
    y = 10;
    return x + y;
}
```

### Global Variables

```c
g = 100;

add(x) {
    return x + g;
}

main() {
    return add(5);  // Returns 105
}
```

### Control Flow

**While loops:**
```c
main() {
    sum = 0;
    i = 1;
    while (i <= 10) {
        sum = sum + i;
        i = i + 1;
    }
    return sum;
}
```

**For loops:**
```c
main() {
    sum = 0;
    for (i = 1; i <= 10; i = i + 1) {
        sum = sum + i;
    }
    return sum;
}
```

### Functions

```c
mul(a, b) {
    return a * b;
}

main() {
    return mul(6, 7);  // Returns 42
}
```

### Recursive Functions

```c
fib(n) {
    if (n <= 1) return n;
    return fib(n-1) + fib(n-2);
}

main() {
    return fib(7);  // Returns 13
}
```

## Project Structure

```
2cc/
├── 2cc/
│   ├── lexer.l         # Flex lexer specification
│   ├── parser.y        # Bison parser grammar
│   ├── ast.h/.c        # Abstract Syntax Tree
│   ├── symtab.h/.c     # Symbol table
│   ├── codegen.h/.c    # ARM64 code generator
│   └── main.c          # Compiler driver
├── calc/
│   ├── calc.l          # Simple calculator lexer (demo)
│   └── calc.y          # Simple calculator parser (demo)
├── Makefile            # Build configuration
└── README.md           # This file
```

## Understanding Flex and Bison

### Flex (Fast Lexical Analyzer)

Flex takes a specification file (`.l`) and generates a C function `yylex()` that:
- Reads input character by character
- Matches patterns using regular expressions
- Returns tokens to the parser

**Example from `lexer.l`:**
```flex
%{
#include "parser.tab.h"
%}

%%
"return"    { return RETURN; }
[0-9]+      { yylval.number = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.string = strdup(yytext); return IDENTIFIER; }
"+"         { return ADD; }
%%
```

### Bison (GNU Parser Generator)

Bison takes a grammar specification (`.y`) and generates a C function `yyparse()` that:
- Calls `yylex()` to get tokens
- Applies grammar rules to build an AST
- Executes semantic actions (code in `{}`)

**Example from `parser.y`:**
```yacc
%token NUMBER IDENTIFIER ADD

%%
expr:
    expr ADD expr { $$ = ast_binary_op(AST_ADD, $1, $3); }
    | NUMBER { $$ = ast_number($1); }
    | IDENTIFIER { $$ = ast_identifier($1); }
    ;
%%
```

### How They Work Together

```
Source Code
    ↓
┌─────────────┐
│   Flex      │ (lexer.l → lex.yy.c)
│  yylex()    │ Tokenize: "x + 5" → [IDENT, ADD, NUM]
└──────┬──────┘
       │ tokens
       ↓
┌─────────────┐
│   Bison     │ (parser.y → parser.tab.c)
│  yyparse()  │ Parse: tokens → AST
└──────┬──────┘
       │ AST
       ↓
┌─────────────┐
│  Code Gen   │ (codegen.c)
│             │ AST → ARM64 Assembly
└─────────────┘
```

## ARM64 Assembly Output

2cc generates ARM64 assembly code for Apple Silicon Macs:

**Example input:**
```c
main() { return 42; }
```

**Generated assembly:**
```asm
.global _main
.align 2

_main:
    sub sp, sp, #16
    mov w0, #42
    str w0, [sp, 0]
    ldr w0, [sp, 0]
    add sp, sp, #16
    ret
```

## Running Tests

```bash
cd 2cc
make test
```

## Why This Transition Matters

### Educational Value

**1cc teaches you:**
- How tokenization works at a low level
- How recursive descent parsing works
- How to generate assembly from scratch

**2cc teaches you:**
- How to use industry-standard tools
- How to write formal grammars
- How to design maintainable compiler frontends

### Industry Practice

Real-world compilers use tools like Flex/Bison because:
- **Correctness**: Generated parsers are proven correct
- **Performance**: Generated lexers are highly optimized
- **Maintainability**: Grammar changes are trivial
- **Documentation**: The `.y` file IS the language spec

Examples:
- **Ruby**: Uses Bison for parsing
- **PHP**: Uses Bison for parsing
- **GCC**: Used to use Bison (now hand-written for performance)
- **Clang**: Hand-written but inspired by formal grammars

## Advantages Over 1cc

| Aspect | 1cc | 2cc |
|--------|-----|-----|
| **Lines of Code** | More verbose | More concise |
| **Maintainability** | Hard to modify | Easy to extend |
| **Error Handling** | Manual | Built-in |
| **Precedence** | Manual implementation | Declarative specification |
| **Testing** | Hard to verify | Grammar is provably correct |
| **Documentation** | Code is docs | Grammar is self-documenting |

## Preparing for 3cc

2cc establishes the frontend architecture (Flex + Bison) that 3cc will reuse. The next step (3cc) focuses on the backend:

- **From ARM64 to LLVM**: Replace `codegen.c` with LLVM API calls
- **From C to C++**: Modernize the codebase
- **From single-arch to multi-arch**: Target any architecture

The Flex/Bison frontend remains largely unchanged in 3cc, demonstrating the power of separating frontend from backend.

## Learning Resources

### Flex and Bison
- [Flex Manual](https://github.com/westes/flex)
- [Bison Manual](https://www.gnu.org/software/bison/manual/)
- [flex & bison](https://www.oreilly.com/library/view/flex-bison/9780596805418/) - O'Reilly book

### Compiler Construction
- [低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook)
- [Crafting Interpreters](https://craftinginterpreters.com/)
- [Engineering a Compiler](https://www.elsevier.com/books/engineering-a-compiler/cooper/978-0-12-088478-0)

### ARM64 Assembly
- [ARM64 Assembly Language](https://developer.arm.com/documentation/)
- [Learn ARM64 Assembly](https://github.com/below/HelloSilicon)

## Next Steps

After understanding 2cc:

1. **Modify the grammar**: Try adding new operators or statements
2. **Extend the language**: Add new features (arrays, structs, etc.)
3. **Study the generated code**: Run Flex/Bison with verbose output
4. **Compare with 1cc**: See how the same features are implemented differently
5. **Move to 3cc**: Learn about LLVM and modern compiler backends

## License

This is an educational project for learning compiler construction.
