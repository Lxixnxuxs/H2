# H2

## Abstract
H2 is a turing-complete object-oriented programming language. 
Moreover, the H2 compiler is capable of analyzing the runtime of functions
and loops described in Landau notation. The goal is to make smart decisions on which
implementation to use for a given algorithm.

## Usage and Building
The H2 compiler uses the CMake system for building and testing. To compile the compiler
by yourself just run

```cmake . && make```
 
inside the root directory. Use the `compiler` executable on a text file
containing H2 code as 

```compiler [PATH]```

The generated
## Basic Control Structures
Without specifying a complete syntax at this point, we provide a few examples of how to write valid H2 code.
Assignment Statements, If-Statements and While-Loops are working as one could expect:
```c++
int a = 5;
if (a<0) {
    return a;
} else {
    return 2*a;
}

int res = 0;
int i = r;
while (0 < i) {
    res = res + i;
    i = i + 1;
}
    
```


```c++
def exp(int a) -> void /% _O(a) %/  {
    if (a<0) {
        return a;
    } else {
        return 2 * exp(a - 1);
    }
}
```
## Runtime Analysis

## Class System