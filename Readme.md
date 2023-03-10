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

For many problems, multiple algorithms exist and bring diffrent runtime complexities with them.
One goal of the H2-language is to define interfaces for algorithms and datastructures.
Based on the code to run, the compiler will choose the best implementation for the given problem.
In order to make this possible, the code needs to be analyzed for runtime complexity.
The result will be inserted into the corresponding function definition or loop, like
`/% _O(a) %/`. In this case, the function `exp` has a runtime complexity of `O(a)`:
```c++
def exp(int a) -> void /% _O(a) %/  {
    if (a <= 0) {
        return 1;
    } else {
        return 2 * exp(a - 1);
    }
}
```

Of course, not for all programs, the runtime complexity can be determined, as this
would even solve the halting problem. The goal is therefore to solve the common cases.
If the runtime complexity cannot be determined, the compiler will state the runtime
of an function `f(int x)` as `_O(func_f)`. It will never occur though, that the compiler
provides a wrong runtime complexity. 

One key feature is the possibility to define custom runtimes. This is done by writing
`O(...)` instead of `_O(...)`. If a custom runtime is defined, the compiler will treat
it as the true runtime complexity of the function, even if the analysis comes to 
a different result. This enables the user to help the compiler where it cannot proof
the runtime complexity by itself.
```c++
/* it is possible to provide custom runtime complexities, even if they are wrong */
def exp(int a) -> void /% O(a^3) %/  {
    return a;
}
```

The current state of runtime analysis is able to deal with loops, if-branches, simple
recursive functions and calls to other functions. Even with these nested loops:
```c++
def f(int x) -> int /% _O(x^5) %/  {
    while (x>=0) /% _O(x^5) %/  {
        x = (x - 1);
        int i = x;
        while (i>0) /% _O(i^3) %/  {
        while (i>0) /% _O((i^3) * x) %/  {
            i = (i - 1);
            int j = i;
            while (j>0) /% _O(j^2) %/  {
            while (j>0) /% _O((j^2) * x) %/  {
                j = (j - 1);
                int k = j;
                while (k>0) /% _O(k) %/  {
                while (k>0) /% _O(k * x) %/  {
                    k = (k - 1);
                    int a = g(x);
                }
            }
        }
```


## Class System