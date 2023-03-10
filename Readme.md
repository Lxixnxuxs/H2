# H2

## Abstract
H2 is a statically typed, turing-complete and object-oriented programming language. 
The H2 compiler is capable of analyzing the runtime of functions
and loops described in Landau notation. The goal is to make smart decisions on which
implementation to use for a given algorithm.

## Usage and Building
The H2 compiler uses the CMake system for building and testing. To compile the compiler
by yourself just run

```cmake . && make```
 
inside the root directory. Use the `compiler` executable on a text file
containing H2 code as follows:
 
```compiler [PATH]```

This will generate a `.s` file containing `x86_64` assembly code 
which then can be assembled and linked with the `gcc` toolchain

## Basic Control Structures
Without specifying a complete syntax at this point, we instead provide a few examples of how to write valid H2 code.
Assignment Statements, If-Statements and While-Loops are working as one would expect. 
The following code snippet for example implements a recursion-based algorithm for computing the powers of two:  

```c++
def exp(int a) -> void {
    if (a <= 0) {
        return 1;
    } else {
        return 2 * exp(a - 1);
    }
}
```
While most programmers will find the H2-syntax very intuitive, there are some peculiarities
of H2:
- A function signature is of the form: `def [name] ([parameter_type] [parameter_name], ...) -> [return_type]` 
- Conditional blocks must be complete, i.e. there cannot be an `if`-block without a corresponding `else`-block
- H2 supports the usual bitwise operators `&, |, <<, >>` and the xor operator `^`

Another code-example demonstrating the usage of variable assignments and `while` loops is given below:
```c++
int a = 5;

if (a < 0) {
    return a;
} else {
    return a * a;
}

int res = 0;
int i = r;

while (0 < i) {
    res = res + i;
    i = i + 1;
}
```
## Runtime Analysis

## Class System